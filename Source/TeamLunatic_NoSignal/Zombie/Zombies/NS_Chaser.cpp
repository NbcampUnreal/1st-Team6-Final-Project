#include "NS_Chaser.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "NS_ChaserAnimInstance.h"
#include "Zombie/AIController/NS_ChaserController.h"

ANS_Chaser::ANS_Chaser()
{
	PrimaryActorTick.bCanEverTick = true;

	// 기본값 초기화
	MaxHealth = 1000.0f;
	CurrentHealth = 1000.0f;
	bEnableAutoDamageTest = false;

	//// 체이서 좀비는 항상 보이도록 설정
	//SetActorHiddenInGame(false);
}

void ANS_Chaser::BeginPlay()
{
	Super::BeginPlay();


	if (NavigationInvoker)
	{
		NavigationInvoker->SetAutoActivate(true);
	}

	NavigationInvoker->SetGenerationRadii(75000.f, 75000.f);

	CurrentHealth = MaxHealth;

	if (bEnableAutoDamageTest)
	{
		GetWorld()->GetTimerManager().SetTimer(AutoDamageTimerHandle, this, &ANS_Chaser::ApplyAutoDamage, 1.0f, true, 1.0f);
	}

	// 체이서 좀비 활성화 로그
	UE_LOG(LogTemp, Warning, TEXT("체이서 좀비 활성화: %s"), *GetName());
}

void ANS_Chaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANS_Chaser::ApplyAutoDamage()
{
	//UGameplayStatics::ApplyDamage(this, 10.0f, GetController(), this, UDamageType::StaticClass());
}

float ANS_Chaser::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// AnimBP를 가져와서 이미 무릎 꿇는 중인지 확인
	if (UNS_ChaserAnimInstance* Anim = Cast<UNS_ChaserAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		if (Anim->IsKneel)
		{
			return 0.0f; // 이미 무릎 꿇었으면 데미지 안받음
		}
	}

	CurrentHealth -= ActualDamage;
	UE_LOG(LogTemp, Warning, TEXT("Chaser damaged. Current Health: %f"), CurrentHealth);

	if (CurrentHealth <= 0.0f)
	{
		CurrentHealth = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("Chaser health depleted. Kneeling down."));

		// AnimInstance의 IsKneel 값을 true로 직접 변경
		if (UNS_ChaserAnimInstance* Anim = Cast<UNS_ChaserAnimInstance>(GetMesh()->GetAnimInstance()))
		{
			Anim->IsKneel = true;
		}

		GetWorld()->GetTimerManager().ClearTimer(AutoDamageTimerHandle);

		if (ANS_ChaserController* MyController = Cast<ANS_ChaserController>(GetController()))
		{
			UE_LOG(LogTemp, Warning, TEXT("Kneeling down, stopping all chase behaviors."));
			MyController->ResetChase();
		}

		GetWorld()->GetTimerManager().SetTimer(KneelRecoveryTimerHandle, this, &ANS_Chaser::RecoverFromKneel, 10.0f, false);
	}

	return ActualDamage;
}

void ANS_Chaser::RecoverFromKneel()
{
	UE_LOG(LogTemp, Warning, TEXT("Chaser recovered. Standing up."));

	// AnimInstance의 IsKneel 값을 false로 직접 변경
	if (UNS_ChaserAnimInstance* Anim = Cast<UNS_ChaserAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Anim->IsKneel = false;
	}

	if (bEnableAutoDamageTest)
	{
		GetWorld()->GetTimerManager().SetTimer(AutoDamageTimerHandle, this, &ANS_Chaser::ApplyAutoDamage, 1.0f, true, 1.0f);
	}
}

void ANS_Chaser::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANS_Chaser, CurrentHealth);
	DOREPLIFETIME(ANS_Chaser, MaxHealth);
}