#include "GameFlow/NS_GameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "UI/NS_UIManager.h"

UNS_GameInstance::UNS_GameInstance()
{
	static ConstructorHelpers::FClassFinder<UNS_UIManager> BP_UIManager(TEXT("/Game/UI/Blueprints/BP_NS_UIManager.BP_NS_UIManager_C"));
	if (BP_UIManager.Succeeded())
	{
		UIManagerClass = BP_UIManager.Class;
	}

	CharacterList.Add(TEXT("/Game/Character/Blueprints/Character/BP_NS_Male1"));
	CharacterList.Add(TEXT("/Game/Character/Blueprints/Character/BP_NS_Male2"));
	CharacterList.Add(TEXT("/Game/Character/Blueprints/Character/BP_NS_Male3"));
	CharacterList.Add(TEXT("/Game/Character/Blueprints/Character/BP_NS_Female1"));
}

void UNS_GameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UNS_GameInstance::OnLevelLoaded);
	if (UIManagerClass)
	{
		NS_UIManager = NewObject<UNS_UIManager>(this, UIManagerClass);
		NS_UIManager->InitUi(GetWorld());
	}
}

void UNS_GameInstance::OnLevelLoaded(UWorld* LoadedWorld)
{
	if (NS_UIManager)
		NS_UIManager->CloseLoadingUI();
}

void UNS_GameInstance::SetGameModeType(EGameModeType Type)
{
	GameModeType = Type;
	UE_LOG(LogTemp, Log, TEXT("[GameInstance] GameModeType set to %s"), *UEnum::GetValueAsString(Type));
}

void UNS_GameInstance::CreateDedicatedSessionViaHTTP(FName SessionName, int32 MaxPlayers)
{
	UE_LOG(LogTemp, Log, TEXT("[CreateDedicatedSessionViaHTTP] Sending HTTP POST: name=%s, max_players=%d"),
		*SessionName.ToString(), MaxPlayers);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://121.163.249.108:5000/create_session"));
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

	FString ResponseString = Response->GetContentAsString();
	UE_LOG(LogTemp, Log, TEXT("[OnCreateSessionResponse] HTTP Response: %s"), *ResponseString);

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[OnCreateSessionResponse] JSON 파싱 실패: %s"), *ResponseString);
		return;
	}

	FString Ip = JsonObject->GetStringField("ip");
	int32 Port = JsonObject->GetIntegerField("port");
	FString Address = FString::Printf(TEXT("%s:%d"), *Ip, Port);
	MyServerPort = Port;

	UE_LOG(LogTemp, Log, TEXT("[OnCreateSessionResponse] 접속 주소: %s"), *Address);

	// 10초마다 heartbeat 보낸다.
	GetWorld()->GetTimerManager().SetTimer(HeartbeatTimerHandle, this, &UNS_GameInstance::SendHeartbeat, 10.0f, true);

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

void UNS_GameInstance::SendHeartbeat()
{
	if (MyServerPort <= 0)
		return;

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://121.163.249.108:5000/heartbeat"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
	Json->SetNumberField("port", MyServerPort);

	FString Body;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
	FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);
	Request->SetContentAsString(Body);

	Request->OnProcessRequestComplete().BindLambda([](FHttpRequestPtr Req, FHttpResponsePtr Res, bool bSuccess)
	{
		if (!bSuccess || !Res.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("[Heartbeat] Failed to reach server."));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[Heartbeat] Response: %s"), *Res->GetContentAsString());
		}
	});

	Request->ProcessRequest();
}


void UNS_GameInstance::RequestSessionListFromServer()
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://121.163.249.108:5000/session_list"));
	Request->SetVerb(TEXT("GET"));
	Request->OnProcessRequestComplete().BindUObject(this, &UNS_GameInstance::OnReceiveSessionList);
	Request->ProcessRequest();
}

void UNS_GameInstance::OnReceiveSessionList(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[OnReceiveSessionList] HTTP 요청 실패"));
		return;
	}

	FString ResponseString = Response->GetContentAsString();
	UE_LOG(LogTemp, Log, TEXT("[OnReceiveSessionList] HTTP Response: %s"), *ResponseString);

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
	if (!FJsonSerializer::Deserialize(Reader, JsonArray))
	{
		UE_LOG(LogTemp, Error, TEXT("[OnReceiveSessionList] JSON 배열 파싱 실패"));
		return;
	}

	TArray<TSharedPtr<FJsonObject>> ParsedSessions;
	for (const TSharedPtr<FJsonValue>& Value : JsonArray)
	{
		TSharedPtr<FJsonObject> SessionObj = Value->AsObject();
		if (SessionObj.IsValid())
		{
			ParsedSessions.Add(SessionObj);
		}
	}

	OnSessionListReceived.Broadcast(ParsedSessions);
}

void UNS_GameInstance::SetCurrentSaveSlot(FString SlotNameInfo)
{
	CurrentSaveSlotName = SlotNameInfo;
}
