// NS_InGameStartMenu.cpp - 게임 중 ESC 키를 눌러 표시되는 일시 정지 메뉴 위젯 구현

#include "UI/NS_InGameStartMenu.h"
#include "Components/Button.h"
#include "UI/NS_MenuButtonWidget.h"
#include "UI/NS_MainMenu.h"
#include "Kismet/GameplayStatics.h"              // UGameplayStatics::OpenLevel() 
#include "OnlineSubsystem.h"                     // IOnlineSubsystem
#include "Interfaces/OnlineSessionInterface.h"   // IOnlineSessionPtr, DestroySession()
#include "Kismet/KismetSystemLibrary.h"          // KismetSystemLibrary::QuitGame()
#include "Blueprint/WidgetBlueprintLibrary.h"    // FInputModeGameOnly
#include "GameFlow/NS_GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "UI/NS_MasterMenuPanel.h"
#include "Zombie/NS_ZombieBase.h"
#include "UI/NS_UIManager.h"
#include "AsyncLoadingScreenLibrary.h"

void UNS_InGameStartMenu::NativeConstruct()
{
    // 부모 클래스의 NativeConstruct 호출
    Super::NativeConstruct();
    
    // 각 버튼에 클릭 이벤트 핸들러 바인딩
    if (BP_Resume)
        BP_Resume->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_InGameStartMenu::OnResumeClicked);
    if (BP_Settings)
        BP_Settings->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_InGameStartMenu::OnSettingsClicked);
    if (BP_Controls)
        BP_Controls->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_InGameStartMenu::OnControlsClicked);
    if (BP_MainMenu)
        BP_MainMenu->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_InGameStartMenu::On_MainMenuClicked);
    if (BP_Quit)
        BP_Quit->RootButton->OnClicked.AddUniqueDynamic(this, &UNS_InGameStartMenu::OnQuitClicked);
}
void UNS_InGameStartMenu::Init(UNS_BaseMainMenu* NsMainMenu)
{
    // 부모 클래스의 Init 호출
    Super::Init(NsMainMenu);
    
    // MainMenu가 null인지 로그로 확인
    if (!MainMenu)
    {
        UE_LOG(LogTemp, Error, TEXT("Init: MainMenu is still null after Super::Init"));
        return;
    }
    
    UE_LOG(LogTemp, Log, TEXT("Init: MainMenu is valid, registering submenus"));
    
    // 서브 메뉴들을 맵에 등록
    SubMenus.Add(EWidgetToggleType::SaveGame, MainMenu->GetWidget(EWidgetToggleType::SaveGame));
    SubMenus.Add(EWidgetToggleType::LoadGame, MainMenu->GetWidget(EWidgetToggleType::LoadGame));
    SubMenus.Add(EWidgetToggleType::Settings, MainMenu->GetWidget(EWidgetToggleType::Settings));
    SubMenus.Add(EWidgetToggleType::LoadMenuInGameOver, MainMenu->GetWidget(EWidgetToggleType::LoadMenuInGameOver));
    SubMenus.Add(EWidgetToggleType::Controls, MainMenu->GetWidget(EWidgetToggleType::Controls));
}

void UNS_InGameStartMenu::ShowWidget()
{
    // 부모 클래스의 ShowWidget 호출 전에 MainMenu가 null인지 확인
    if (!MainMenu)
    {
        UE_LOG(LogTemp, Error, TEXT("ShowWidget: MainMenu is null"));
        // MainMenu가 null이면 부모 클래스의 ShowWidget을 호출하지 않고 기본 동작만 수행
        SetVisibility(ESlateVisibility::Visible);
        return;
    }
    
    // 부모 클래스의 ShowWidget 호출
    Super::ShowWidget();
    
    // SubMenus가 비어있지 않은 경우에만 서브 메뉴 숨기기
    if (SubMenus.Num() > 0)
    {
        HideSubMenuWidget();
    }
}

void UNS_InGameStartMenu::OnResumeClicked()
{
    // 게임 인스턴스를 통해 UI 매니저 가져오기
    if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
    {
        if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
           UIManager->HideInGameMenuWidget(GetWorld());
    }
    
    // 서브 메뉴와 현재 메뉴 숨기기
    HideSubMenuWidget();
    HideWidget();
}

void UNS_InGameStartMenu::OnSettingsClicked()
{
    // 서브 메뉴 숨기기
    HideSubMenuWidget();
    
    // MainMenu가 null인지 확인
    if (!MainMenu)
    {
        UE_LOG(LogTemp, Error, TEXT("OnSettingsClicked: MainMenu is null"));
        
        // 게임 인스턴스에서 메인 메뉴 가져오기 시도
        if (UNS_GameInstance* GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
        {
            MainMenu = GameInstance->GetMainMenu();
            UE_LOG(LogTemp, Log, TEXT("OnSettingsClicked: Tried to get MainMenu from GameInstance: %s"), 
                   MainMenu ? TEXT("Success") : TEXT("Failed"));
        }
        
        if (!MainMenu)
            return;
    }
    
    // 현재 메뉴 숨기기
    HideWidget();
    
    // 설정 메뉴 표시
    if (UNS_MasterMenuPanel* Widget = MainMenu->GetWidget(EWidgetToggleType::Settings))
    {
        UE_LOG(LogTemp, Log, TEXT("OnSettingsClicked: Showing Settings widget"));
        Widget->ShowWidget();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OnSettingsClicked: Settings widget not found"));
    }
}
void UNS_InGameStartMenu::OnControlsClicked()
{
    // 서브 메뉴 숨기기
    HideSubMenuWidget();
    
    // MainMenu가 null인지 확인
    if (!MainMenu)
    {
        UE_LOG(LogTemp, Error, TEXT("OnControlsClicked: MainMenu is null"));
        
        // 게임 인스턴스에서 메인 메뉴 가져오기 시도
        if (UNS_GameInstance* GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
        {
            MainMenu = GameInstance->GetMainMenu();
            UE_LOG(LogTemp, Log, TEXT("OnControlsClicked: Tried to get MainMenu from GameInstance: %s"), 
                   MainMenu ? TEXT("Success") : TEXT("Failed"));
        }
        
        if (!MainMenu)
            return;
    }
    
    // 컨트롤 설정 메뉴 표시
    if (UNS_MasterMenuPanel* Widget = MainMenu->GetWidget(EWidgetToggleType::Controls))
    {
        UE_LOG(LogTemp, Log, TEXT("OnControlsClicked: Showing Controls widget"));
        Widget->ShowWidget();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("OnControlsClicked: Controls widget not found"));
    }
}
void UNS_InGameStartMenu::On_MainMenuClicked()
{
    // 현재 메뉴 숨기기
    HideWidget();

    // 에러 방지를 위해 try-catch 패턴 사용
    try
    {
        // 월드 객체 가져오기
        UWorld* World = GetWorld();
        if (!World)
        {
            UE_LOG(LogTemp, Error, TEXT("On_MainMenuClicked: World is null"));
            return;
        }

        // 모든 좀비 객체의 타이머 정리
        TArray<AActor*> FoundZombies;
        UGameplayStatics::GetAllActorsOfClass(World, ANS_ZombieBase::StaticClass(), FoundZombies);
        for (AActor* Actor : FoundZombies)
        {
            if (ANS_ZombieBase* Zombie = Cast<ANS_ZombieBase>(Actor))
            {
                // 좀비 객체의 타이머 정리
                World->GetTimerManager().ClearTimer(Zombie->AmbientSoundTimer);
                
                // 좀비 비활성화
                Zombie->SetActive_Multicast(false);
            }
        }

        // 온라인 세션 연결 해제 (에러 발생 가능성 있음)
        OnDisconnectClicked();
        
        // UI 매니저를 통해 인게임 메뉴 숨기기
        if (UNS_GameInstance* NS_GameInstance = Cast<UNS_GameInstance>(GetGameInstance()))
        {
            if (UNS_UIManager* UIManager = NS_GameInstance->GetUIManager())
                UIManager->HideInGameMenuWidget(World);
        }
        
        // 로딩 화면 설정 (에러 발생 가능성 있음)
        // 패키징된 빌드에서 문제가 있다면 이 부분을 주석 처리해보세요
        UAsyncLoadingScreenLibrary::SetEnableLoadingScreen(false);
        
        // 메인 타이틀 레벨 로드
        // 레벨 이름이 정확한지 확인하세요
        UE_LOG(LogTemp, Warning, TEXT("메인 메뉴로 이동 시도 중..."));
        UGameplayStatics::OpenLevel(World, FName(TEXT("MainTitle")));
    }
    catch (...)  // 사용하지 않는 변수 제거
    {
        // C++ 예외 처리 (UE4에서는 제한적으로 작동)
        UE_LOG(LogTemp, Error, TEXT("메인 메뉴 이동 중 오류 발생"));
    }
}

void UNS_InGameStartMenu::OnDisconnectClicked()
{
    // 안전한 세션 종료를 위한 null 체크 추가
    IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
    if (OnlineSub)
    {
        IOnlineSessionPtr SessionInterface = OnlineSub->GetSessionInterface();
        if (SessionInterface.IsValid())
        {
            // 세션이 실제로 존재하는지 확인 후 종료
            FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
            if (ExistingSession)
            {
                UE_LOG(LogTemp, Log, TEXT("게임 세션 종료 중..."));
                SessionInterface->DestroySession(NAME_GameSession);
            }
        }
    }
}

void UNS_InGameStartMenu::OnQuitClicked()
{
    // 온라인 세션 연결 해제
    OnDisconnectClicked();
    
    // 플레이어 컨트롤러 가져오기
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (PC)
        // 게임 완전히 종료
        UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, true);
}
