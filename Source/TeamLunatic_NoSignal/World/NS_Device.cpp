// Fill out your copyright notice in the Description page of Project Settings.


#include "World/NS_Device.h"
#include "Net/UnrealNetwork.h"
#include "Inventory UI/NS_InventoryHUD.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ANS_Device::ANS_Device()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// 메쉬 컴포넌트 생성
	Frame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Frame"));
	DoorA = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorA"));
	DoorB = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorB"));

	// 루트 컴포넌트 설정
	SetRootComponent(Frame);

	// 계층 구조 설정 (도어들이 프레임에 붙어 있음)
	DoorA->SetupAttachment(Frame);
	DoorB->SetupAttachment(Frame);

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ANS_Device::BeginPlay()
{
	Super::BeginPlay();

	InteractableData = InstanceInteractableData;
}

void ANS_Device::BeginFocus()
{
	if (Frame)  Frame->SetRenderCustomDepth(true);
	if (DoorA)  DoorA->SetRenderCustomDepth(true);
	if (DoorB)  DoorB->SetRenderCustomDepth(true);
}

void ANS_Device::EndFocus()
{
	if (Frame)  Frame->SetRenderCustomDepth(false);
	if (DoorA)  DoorA->SetRenderCustomDepth(false);
	if (DoorB)  DoorB->SetRenderCustomDepth(false);
}
void ANS_Device::Interact_Implementation(AActor* InteractingActor)
{
}
// Called every frame
void ANS_Device::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


