#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NS_PlayerCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
struct FInputActionValue;
UCLASS()
class NOSIGNAL_API ANS_PlayerCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANS_PlayerCharacterBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	// 캐릭터 기본 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float DefaultWalkSpeed;
	
	// 달리기 속도 배율
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float SprintSpeedMultiplier;
	
	bool IsCanJump = true;

	UFUNCTION()
	void MoveAction(const FInputActionValue& value);
	UFUNCTION()
	void LookAction(const FInputActionValue& value);
	UFUNCTION()
	void JumpAction(const FInputActionValue& value);
	UFUNCTION()
	void CrouchAction(const FInputActionValue& value);
	UFUNCTION()
	void StartSprintAction(const FInputActionValue& value);
	UFUNCTION()
	void StopSprintAction(const FInputActionValue& value);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	

};
