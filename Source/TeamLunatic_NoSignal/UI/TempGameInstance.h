// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TempGameInstance.generated.h"


class UNS_UIManager;
UCLASS()
class TEAMLUNATIC_NOSIGNAL_API UTempGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UTempGameInstance();

public:

	UPROPERTY(BlueprintReadOnly, Category = "SaveGame")
	FString CurrentSaveSlotName;

	void SetCurrentSaveSlot(FString SlotNameInfo);

	UFUNCTION(BlueprintCallable, Category = "UI")
	UNS_UIManager* GetUIManager() const { return UIManager; };

protected:
	UPROPERTY()
	UNS_UIManager* UIManager;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UNS_UIManager> UIManagerClass;

	virtual void Init() override;

};
