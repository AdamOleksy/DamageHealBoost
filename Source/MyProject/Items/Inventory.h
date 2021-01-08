// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Templates/SubclassOf.h"
#include "Inventory.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UInventory : public UActorComponent
{
	GENERATED_BODY()


public:	
	// Sets default values for this component's properties
	UInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class AItemBase*> ItemsList;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inv widget")
	TSubclassOf<class UUserWidget> InventoryWidgetClass;

	UUserWidget* InventoryWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inv widget")
	TSubclassOf<class UUserWidget> InventoryShopWidgetClass;

	UUserWidget* InventoryShopWidget;

	TSubclassOf<class AItemBase> FirstItem;

	void AddItemToInventory(AItemBase* Item);

	void RemoveFromInventory(AItemBase* Item);

	void ShowInventory();

	void HideInventory();

	void ShowShopingInventory();

	void HideShopInventory();

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdate OnInventoryUpdate;
};
