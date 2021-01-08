// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemBase.h"
#include "Components/BoxComponent.h"
#include "../MyProjectCharacter.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(FName("TriggerVolume"));
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnBoxBeginOverlap);
}

void AItemBase::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyProjectCharacter* Player = Cast < AMyProjectCharacter>(OtherActor);
	if (Player)
	{
		Player->PickUpItem(this);
	}
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FName AItemBase::GetItemName()
{
	return ItemName;
}

void AItemBase::Use(AMyProjectCharacter* PlayerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("Using item"));
}
