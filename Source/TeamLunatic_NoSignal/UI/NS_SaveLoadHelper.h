#pragma once

#include "CoreMinimal.h"

struct FPlayerSaveData;
struct FLevelSaveData;
class UNS_SaveGameMetaData;

class TEAMLUNATIC_NOSIGNAL_API NS_SaveLoadHelper
{
public:
	static constexpr const TCHAR* MetaSlotName = TEXT("SaveGameMetaData");
	static constexpr const TCHAR* GameLevelName = TEXT("Showcase");//Showcase MainWorld


	static bool SaveGame(const FString& SlotName, const FPlayerSaveData& PlayerData, const FLevelSaveData& LevelData);
	static bool SaveGameMetaData(const FString& SlotName, const FString& LevelName);
	static bool LoadGame(const FString& SlotName,  FPlayerSaveData& OutPlayerData, FLevelSaveData& OutLevelData);
	static UNS_SaveGameMetaData* LoadSaveMetaData();

	static bool FindExistingSave(const FString& SlotName);//같은 슬롯(이름)이 저장되어 있는지 체크
	static bool DeleteExistingSave(const FString& SlotName);// Delete SaveData and SaveMetaData

	static FString FormatDateTime_YMDHM(const FDateTime& InDateTime);

	NS_SaveLoadHelper() = delete;
	~NS_SaveLoadHelper() = delete;
};
