// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

UCLASS(Abstract, BlueprintType, Blueprintable)
class MYPROJECT_API AItemBase : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = 0.f))
	float ItemWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* TriggerVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"))
	class UTexture2D* ItemThumbnail;


	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", meta = (AllowPrivateAccess = "true"), meta = (ClampMin = 0.f))
	int32 ItemCost = 100;

	// Sets default values for this actor's properties
	AItemBase();

	UPROPERTY()
	class UInventory* InventoryRef;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FName GetItemName();

	virtual void Use(class AMyProjectCharacter* PlayerCharacter);

	UFUNCTION(BlueprintImplementableEvent)
	void OnUse(class AMyProjectCharacter* PlayerCharacter);

};
