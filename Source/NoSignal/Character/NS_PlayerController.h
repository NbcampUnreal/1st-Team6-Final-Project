#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NS_PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class NOSIGNAL_API ANS_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANS_PlayerController();

	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext; // IMC

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction; // Move

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction; // Jump

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction; // Look

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SprintAction; // Sprint

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* CrouchAction; // Crouch

protected:
	virtual void BeginPlay() override;
};
