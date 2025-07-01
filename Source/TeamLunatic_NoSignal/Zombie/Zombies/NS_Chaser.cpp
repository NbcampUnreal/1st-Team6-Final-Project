#include "NS_Chaser.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "NS_ChaserAnimInstance.h"
#include "Zombie/AIController/NS_ChaserController.h"
#include "GameFlow/NS_MainGamePlayerState.h"
#include "GameFlow/NS_GameState.h"

ANS_Chaser::ANS_Chaser()
{
	PrimaryActorTick.bCanEverTick = true;

	MaxHealth = 750.0f;
	CurrentHealth = 750.0f;
	bEnableAutoDamageTest = false;

}

void ANS_Chaser::BeginPlay()
{
	Super::BeginPlay(); 

	if (NavigationInvoker)
	{
		NavigationInvoker->SetAutoActivate(true);
	}

	NavigationInvoker->SetGenerationRadii(75000.f, 75000.f);

	if (GetLocalRole() == ROLE_Authority) 
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		if (GameState)
		{
			int32 NumPlayers = GameState->PlayerArray.Num();

			if (NumPlayers == 0)
			{
				NumPlayers = 1; 
			}

			MaxHealth = 750.0f * static_cast<float>(NumPlayers);
			CurrentHealth = MaxHealth; 

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ANS_Chaser (Server): Failed to get GameState. Setting default health to 1000.0f."));
			MaxHealth = 750.0f; 
			CurrentHealth = MaxHealth;
		}
	}


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
	//UGameplayStatics::ApplyDamage(this, 10.0f, GetController(), this, UDamageType::StaticClass());
}

float ANS_Chaser::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return 0.0f; 
	}

	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (UNS_ChaserAnimInstance* Anim = Cast<UNS_ChaserAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		if (Anim->IsKneel)
		{
			UE_LOG(LogTemp, Warning, TEXT("Chaser is already kneeling, no damage taken. Current Health: %f"), CurrentHealth);
			return 0.0f;
		}
	}

	CurrentHealth -= ActualDamage;

	if (CurrentHealth <= 0.0f)
	{
		CurrentHealth = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("Chaser health depleted. Kneeling down."));

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
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Chaser recovered. Standing up. Current Health: %f"), CurrentHealth);

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