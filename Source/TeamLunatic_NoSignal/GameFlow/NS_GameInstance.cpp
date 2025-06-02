
#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include <Online/OnlineSessionNames.h>
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "UI/NS_UIManager.h"

UNS_GameInstance::UNS_GameInstance()
{
	static ConstructorHelpers::FClassFinder<UNS_UIManager> BP_UIManager(TEXT("/Game/UI/Blueprints/BP_NS_UIManager.BP_NS_UIManager_C"));
	if (BP_UIManager.Succeeded())
		UIManagerClass = BP_UIManager.Class;
}
//void UNS_GameInstance::Init()
//{
//	Super::Init();
//
//	if (UIManagerClass)
//	{
//		NS_UIManager = NewObject<UNS_UIManager>(this, UIManagerClass);
//		NS_UIManager->InitUi(GetWorld());
//	}
//}
void UNS_GameInstance::Init()
{
	Super::Init();

	if (UIManagerClass)
	{
		// NS_UIManager 생성까지만 하고
		NS_UIManager = NewObject<UNS_UIManager>(this, UIManagerClass);

		// InitUi()는 BeginPlay 시점으로 미루기
		// 예: 타이머로 0초 딜레이 호출
		FTimerHandle TempHandle;
		GetWorld()->GetTimerManager().SetTimer(TempHandle, [this]()
		{
			NS_UIManager->InitUi(GetWorld());
		}, 0.01f, false);
	}
}


void UNS_GameInstance::SetGameModeType(EGameModeType Type)
{
	GameModeType = Type;
	UE_LOG(LogTemp, Log, TEXT("[GameInstance] GameModeType set to %s"), *UEnum::GetValueAsString(Type));
}

void UNS_GameInstance::CreateDedicatedSessionViaHTTP(FName SessionName,int32 MaxPlayers)
{
	UE_LOG(LogTemp, Log, TEXT("[CreateDedicatedSessionViaHTTP] Sending HTTP POST: name=%s, max_players=%d"),
		*SessionName.ToString(), MaxPlayers);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://54.180.208.1:5000/create_session"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField("name", SessionName.ToString());
	JsonObject->SetNumberField("max_players", MaxPlayers);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	Request->SetContentAsString(RequestBody);
	Request->OnProcessRequestComplete().BindUObject(this, &UNS_GameInstance::OnCreateSessionResponse);
	Request->ProcessRequest();
}

void UNS_GameInstance::OnCreateSessionResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[OnCreateSessionResponse] HTTP 요청 실패"));
		return;
	}

	// 1. HTTP 응답 문자열 얻기
	FString ResponseString = Response->GetContentAsString();
	UE_LOG(LogTemp, Log, TEXT("[OnCreateSessionResponse] HTTP Response: %s"), *ResponseString);

	// 2. JSON 파싱
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[OnCreateSessionResponse] JSON 파싱 실패: %s"), *ResponseString);
		return;
	}

	// 3. ip, port 읽어서 "ip:port" 형태로 조합
	FString Ip = JsonObject->GetStringField("ip");
	int32   Port = JsonObject->GetIntegerField("port");
	FString Address = FString::Printf(TEXT("%s:%d"), *Ip, Port);
	UE_LOG(LogTemp, Log, TEXT("[OnCreateSessionResponse] 접속 주소: %s"), *Address);

	// 4. PlayerController로 ClientTravel 호출
	if (UWorld* World = GetWorld())
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
		{
			PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[OnCreateSessionResponse] PlayerController 획득 실패"));
		}
	}
}

void UNS_GameInstance::FindSessions()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) return;

	IOnlineSessionPtr Sessions = Subsystem->GetSessionInterface();
	if (!Sessions.IsValid()) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 100;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	Sessions->OnFindSessionsCompleteDelegates.AddUObject(this, &UNS_GameInstance::OnFindSessionsComplete);
	Sessions->FindSessions(0, SessionSearch.ToSharedRef());
}

void UNS_GameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr Sessions = Subsystem->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnFindSessionsCompleteDelegates(this);
		}
	}

	int32 NumResults = (SessionSearch.IsValid() ? SessionSearch->SearchResults.Num() : -1);
	UE_LOG(LogTemp, Log, TEXT("[OnFindSessionsComplete] Success: %d, Results: %d"), bWasSuccessful, NumResults);

	if (bWasSuccessful && SessionSearch.IsValid())
	{
		OnSessionSearchSuccess.Broadcast(SessionSearch->SearchResults);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[OnFindSessionsComplete] Session search failed."));
	}
}

void UNS_GameInstance::TryJoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) return;

	IOnlineSessionPtr Sessions = Subsystem->GetSessionInterface();
	if (!Sessions.IsValid()) return;

	Sessions->OnJoinSessionCompleteDelegates.AddUObject(this, &UNS_GameInstance::OnJoinSessionCompleteInternal);
	Sessions->JoinSession(0, NAME_GameSession, SessionResult);
}


void UNS_GameInstance::OnJoinSessionCompleteInternal(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (!Subsystem) return;

	IOnlineSessionPtr Sessions = Subsystem->GetSessionInterface();
	if (!Sessions.IsValid()) return;

	Sessions->ClearOnJoinSessionCompleteDelegates(this);

	FString ConnectString;
	if (Sessions->GetResolvedConnectString(SessionName, ConnectString))
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			PC->ClientTravel(ConnectString, TRAVEL_Absolute);
			OnJoinSessionComplete.Broadcast(true);
			return;
		}
	}

	OnJoinSessionComplete.Broadcast(false);
}
