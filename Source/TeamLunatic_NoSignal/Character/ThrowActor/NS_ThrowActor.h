#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NS_ThrowActor.generated.h"

UCLASS()
class TEAMLUNATIC_NOSIGNAL_API ANS_ThrowActor : public AActor
{
	GENERATED_BODY()

public:
	ANS_ThrowActor();

	// 병 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Throw")
	UStaticMeshComponent* BottleMesh;
};
