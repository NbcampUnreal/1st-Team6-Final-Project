// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/TempGameInstance.h"
#include "UI/NS_UIManager.h"

UTempGameInstance::UTempGameInstance()
{
	//2025/0514 5분기록보드 문제 아직 해결안됌. 
	//if (!UIManagerClass)
	//{
	//	static ConstructorHelpers::FClassFinder<UNS_UIManager> BP_UIManager(TEXT("/Game/UI/Blueprints/BP_NS_UIManager"));
	//	if (BP_UIManager.Succeeded())										      
	//		UIManagerClass = BP_UIManager.Class;
	//}
}

void UTempGameInstance::Init()
{
	Super::Init();

	if (UIManagerClass)
	{
		UIManager = NewObject<UNS_UIManager>(this, UIManagerClass);
		UIManager->InitUi(GetWorld());
	}
}
