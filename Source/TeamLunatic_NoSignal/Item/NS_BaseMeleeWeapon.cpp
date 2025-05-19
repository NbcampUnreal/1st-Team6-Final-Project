#include "Item/NS_BaseMeleeWeapon.h"
#include "Components/StaticMeshComponent.h"


ANS_BaseMeleeWeapon::ANS_BaseMeleeWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
}
