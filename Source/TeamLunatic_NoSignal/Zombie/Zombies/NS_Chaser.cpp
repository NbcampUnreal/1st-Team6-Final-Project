#include "NS_Chaser.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Animation/AnimInstance.h"                 // AnimInstance 사용을 위해 추가
#include "NS_ChaserAnimInstance.h"                // AnimInstance C++ 클래스 헤더 포함
#include "Zombie/AIController/NS_ChaserController.h"  // 컨트롤러 헤더 파일 포함

ANS_Chaser::ANS_Chaser()
{
	PrimaryActorTick.bCanEverTick = true;

	// 기본값 초기화
	MaxHealth = 1000.0f;
	CurrentHealth = 1000.0f;
	bEnableAutoDamageTest = false;
	bIsActive = true;
}

void ANS_Chaser::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;

	if (bEnableAutoDamageTest)
	{
		GetWorld()->GetTimerManager().SetTimer(AutoDamageTimerHandle, this, &ANS_Chaser::ApplyAutoDamage, 1.0f, true, 1.0f);
	}
}

void ANS_Chaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANS_Chaser::ApplyAutoDamage()
{
	UGameplayStatics::ApplyDamage(this, 10.0f, GetController(), this, UDamageType::StaticClass());
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
	CurrentHealth = MaxHealth;
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