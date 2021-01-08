// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "ItemBase.h"
#include "Blueprint/UserWidget.h"
#include "../MyProjectCharacter.h"
#include "ItemCube.h"

// Sets default values for this component's properties
UInventory::UInventory()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UInventory::BeginPlay()
{
	Super::BeginPlay();

	// ...

	if (InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UUserWidget>(GetWorld(), InventoryWidgetClass);
	}

	if (InventoryShopWidgetClass)
	{
		InventoryShopWidget = CreateWidget<UUserWidget>(GetWorld(), InventoryShopWidgetClass);
	}

}

void UInventory::AddItemToInventory(AItemBase* Item)
{
	if (Item)
	{
		Item->InventoryRef = this;
		ItemsList.Add(Item);
		OnInventoryUpdate.Broadcast();
	}
}

void UInventory::RemoveFromInventory(AItemBase* Item)
{
	if (Item)
	{
		Item->InventoryRef = nullptr;
		ItemsList.RemoveSingle(Item);
		OnInventoryUpdate.Broadcast();
	}
}

void UInventory::ShowInventory()
{
	if (InventoryWidget)
	{
		InventoryWidget->AddToViewport();
		AMyProjectCharacter* Player = Cast<AMyProjectCharacter>(GetOwner());
		if (Player)
		{
			APlayerController* PC = Cast<APlayerController>(Player->GetController());
			if (PC)
			{
				PC->bShowMouseCursor = true;
				PC->bEnableClickEvents = true;
				PC->bEnableMouseOverEvents = true;
			}

		}
		

	}
}

void UInventory::ShowShopingInventory()
{
	
	if (InventoryShopWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shop inventory"));
		InventoryShopWidget->AddToViewport();
		AMyProjectCharacter* Player = Cast<AMyProjectCharacter>(GetOwner());
		if (Player)
		{
			APlayerController* PC = Cast<APlayerController>(Player->GetController());
			if (PC)
			{
				PC->bShowMouseCursor = true;
				PC->bEnableClickEvents = true;
				PC->bEnableMouseOverEvents = true;
			}

		}
	}
}

void UInventory::HideInventory()
{
	if (InventoryWidget)
	{
		InventoryWidget->RemoveFromViewport();
		AMyProjectCharacter* Player = Cast<AMyProjectCharacter>(GetOwner());
		if (Player)
		{
			APlayerController* PC = Cast<APlayerController>(Player->GetController());
			if (PC)
			{
				PC->bShowMouseCursor = true;
				PC->bEnableClickEvents = true;
				PC->bEnableMouseOverEvents = true;
			}

		}
	}
}

void UInventory::HideShopInventory()
{
	if (InventoryShopWidget)
	{
		InventoryShopWidget->RemoveFromViewport();
		AMyProjectCharacter* Player = Cast<AMyProjectCharacter>(GetOwner());
		if (Player)
		{
			APlayerController* PC = Cast<APlayerController>(Player->GetController());
			if (PC)
			{
				PC->bShowMouseCursor = true;
				PC->bEnableClickEvents = true;
				PC->bEnableMouseOverEvents = true;
			}

		}
	}
}