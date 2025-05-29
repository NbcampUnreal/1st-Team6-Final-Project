// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NS_SaveLoadHelper.h"
#include "Kismet/GameplayStatics.h"
#include "NS_SaveGame.h"
#include "UI/NS_CommonType.h"
#include "UI/NS_SaveGameMetaData.h"
#include "UI/NS_SaveGame.h"
#include "UI/TempGameInstance.h"

bool NS_SaveLoadHelper::SaveGame(const FString& SlotName, const FPlayerSaveData& PlayerData, const FLevelSaveData& LevelData)
{
    UNS_SaveGame* SaveObject = Cast<UNS_SaveGame>(
        UGameplayStatics::CreateSaveGameObject(UNS_SaveGame::StaticClass()));

    if (!SaveObject) 
    {
        UE_LOG(LogTemp, Warning, TEXT("Error Create NS_SaveGame"));
        return false;
    }
    SaveObject->PlayerSaves.Add(PlayerData);
    SaveObject->LevelSaves.Add(LevelData);

    //실제 SaveObject 저장에 성공했다면.!!!
    if (UGameplayStatics::SaveGameToSlot(SaveObject, SlotName, 0))
    {
        if (SaveGameMetaData(SlotName, LevelData.LevelName))
            return true;
    }
    return false;
}

bool NS_SaveLoadHelper::SaveGameMetaData(const FString& SlotName, const FString& LevelName)
{
    //1. 저장된 메타 데이터가 있으면 꺼내서 같은 SlotName을 찾아서 덮어쓰던지,없으면 추가
    //2. 저장된 메타 데이터가 없으면 메타 데이터 생성해서 "SlotName"으로 저장(Add)  

    UNS_SaveGameMetaData* SaveGameMetaData = Cast<UNS_SaveGameMetaData>(
        UGameplayStatics::DoesSaveGameExist(MetaSlotName, 0)
        ? UGameplayStatics::LoadGameFromSlot(MetaSlotName, 0)
        : UGameplayStatics::CreateSaveGameObject(UNS_SaveGameMetaData::StaticClass()));

    if (SaveGameMetaData)
    {
        FSaveMetaData NewMeta;
        NewMeta.SaveGameSlotName = SlotName;
        NewMeta.LevelName = LevelName;
        NewMeta.SaveTime = FDateTime::Now();//FormatDateTime_YMDHM(FDateTime::Now());
        UE_LOG(LogTemp, Warning, TEXT("Now = %s"), *NewMeta.SaveTime.ToString());

        int32 FoundIndex = INDEX_NONE;

        for (int32 i = 0; i < SaveGameMetaData->SaveMetaDataArray.Num(); ++i)
        {
            if (SaveGameMetaData->SaveMetaDataArray[i].SaveGameSlotName == SlotName)
            {
                FoundIndex = i;
                break;
            }
        }

        if (FoundIndex != INDEX_NONE)
            SaveGameMetaData->SaveMetaDataArray[FoundIndex] = NewMeta;
        else
            SaveGameMetaData->SaveMetaDataArray.Add(NewMeta);

        UGameplayStatics::SaveGameToSlot(SaveGameMetaData, MetaSlotName, 0);
		return true;
    }
    else
        return false;
}

bool NS_SaveLoadHelper::LoadGame(const FString& SlotName,  FPlayerSaveData& OutPlayerData, FLevelSaveData& OutLevelData)
{
    if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0)) return false;

    UNS_SaveGame* Loaded = Cast<UNS_SaveGame>( UGameplayStatics::LoadGameFromSlot(SlotName, 0));

    if (!Loaded || Loaded->PlayerSaves.Num() == 0 || Loaded->LevelSaves.Num() == 0) return false;

    OutPlayerData = Loaded->PlayerSaves[0];
    OutLevelData = Loaded->LevelSaves[0];
    return true;
}

bool NS_SaveLoadHelper::FindExistingSave(const FString& SlotName) //같은 슬롯(이름)이 저장되어 있는지 체크
{
    if (!UGameplayStatics::DoesSaveGameExist(MetaSlotName, 0)) return false;

    auto* MetaData = Cast<UNS_SaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(MetaSlotName, 0));
    if (!MetaData) return false;

    for (const auto& Entry : MetaData->SaveMetaDataArray)
    {
        if (Entry.SaveGameSlotName == SlotName)
            return true;
    }
    return false;
}
bool NS_SaveLoadHelper::DeleteExistingSave(const FString& SlotName) // Delete! Save Slot and Save Meta Data
{
    if (!UGameplayStatics::DoesSaveGameExist(MetaSlotName, 0))
        return  false;

    UNS_SaveGameMetaData* MetaData = Cast<UNS_SaveGameMetaData>(
        UGameplayStatics::LoadGameFromSlot(MetaSlotName, 0));

    if (!MetaData)
        return false;

    int32 FoundIndex = INDEX_NONE;

    for (int32 i = 0; i < MetaData->SaveMetaDataArray.Num(); ++i)
    {
        if (MetaData->SaveMetaDataArray[i].SaveGameSlotName == SlotName)
        {
            FoundIndex = i;
            break;
        }
    }

    if (FoundIndex != INDEX_NONE)
    {
        MetaData->SaveMetaDataArray.RemoveAt(FoundIndex);
        UGameplayStatics::SaveGameToSlot(MetaData, MetaSlotName, 0);
    }

    // 실제 세이브 데이터 삭제
    if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
    {
        UGameplayStatics::DeleteGameInSlot(SlotName, 0);
        return true;
    }
    return false;
}
FString NS_SaveLoadHelper::FormatDateTime_YMDHM(const FDateTime& InDateTime)
{
	FString FormattedDateTime = FString::Printf(TEXT("%04d-%02d-%02d %02d:%02d"),
		InDateTime.GetYear(),
		InDateTime.GetMonth(),
		InDateTime.GetDay(),
		InDateTime.GetHour(),
		InDateTime.GetMinute());
    return FormattedDateTime;
}
UNS_SaveGameMetaData* NS_SaveLoadHelper::LoadSaveMetaData()
{
    if (!UGameplayStatics::DoesSaveGameExist(NS_SaveLoadHelper::MetaSlotName, 0)) 
        return nullptr;

    return Cast<UNS_SaveGameMetaData>(UGameplayStatics::LoadGameFromSlot(NS_SaveLoadHelper::MetaSlotName, 0));
}
void NS_SaveLoadHelper::FixSaveData()
{
    if (!UGameplayStatics::DoesSaveGameExist(MetaSlotName, 0))
    {
        UE_LOG(LogTemp, Warning, TEXT("메타데이터 파일이 없습니다."));
        return;
    }

    UNS_SaveGameMetaData* MetaData = Cast<UNS_SaveGameMetaData>(
        UGameplayStatics::LoadGameFromSlot(MetaSlotName, 0));

    if (!MetaData)
    {
        UE_LOG(LogTemp, Warning, TEXT("메타데이터를 불러올 수 없습니다."));
        return;
    }

    TArray<int32> IndicesToRemove;

    for (int32 i = 0; i < MetaData->SaveMetaDataArray.Num(); ++i)
    {
        const FString& SlotName = MetaData->SaveMetaDataArray[i].SaveGameSlotName;

        if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
        {
            UE_LOG(LogTemp, Warning, TEXT("SlotName %s 의 실제 세이브 파일이 없어 메타데이터에서 제거 예정"), *SlotName);
            IndicesToRemove.Add(i);
        }
    }

    for (int32 i = IndicesToRemove.Num() - 1; i >= 0; --i)
    {
        MetaData->SaveMetaDataArray.RemoveAt(IndicesToRemove[i]);
    }

    if (IndicesToRemove.Num() > 0)
    {
        UGameplayStatics::SaveGameToSlot(MetaData, MetaSlotName, 0);
        UE_LOG(LogTemp, Warning, TEXT("메타데이터 정리 완료 (정리된 항목 수: %d)"), IndicesToRemove.Num());
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("정리할 필요가 없었습니다."));
    }
}
void NS_SaveLoadHelper::DeleteAllSaves()
{
    if (UGameplayStatics::DoesSaveGameExist(MetaSlotName, 0))
    {
        UNS_SaveGameMetaData* MetaData = Cast<UNS_SaveGameMetaData>(
            UGameplayStatics::LoadGameFromSlot(MetaSlotName, 0));

        if (MetaData)
        {
            for (const FSaveMetaData& Meta : MetaData->SaveMetaDataArray)
            {
                const FString& SlotName = Meta.SaveGameSlotName;
                if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
                {
                    UGameplayStatics::DeleteGameInSlot(SlotName, 0);
                    UE_LOG(LogTemp, Warning, TEXT("세이브 파일 삭제: %s"), *SlotName);
                }
            }
        }

        UGameplayStatics::DeleteGameInSlot(MetaSlotName, 0);
        UE_LOG(LogTemp, Warning, TEXT("메타데이터 파일 삭제: %s"), MetaSlotName);
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("메타데이터가 존재하지 않아 삭제할 항목이 없습니다."));
    }
}