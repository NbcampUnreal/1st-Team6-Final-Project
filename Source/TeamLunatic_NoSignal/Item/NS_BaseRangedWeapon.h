#pragma once

#include "CoreMinimal.h"
#include "Item/NS_BaseWeapon.h"
#include "NS_BaseRangedWeapon.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_BaseRangedWeapon : public ANS_BaseWeapon
{
	GENERATED_BODY()
	
public:
	ANS_BaseRangedWeapon();

protected:
	virtual void BeginPlay() override;

public:
	virtual void BeginFocus() override;
	virtual void EndFocus() override;
};
