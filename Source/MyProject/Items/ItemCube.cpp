// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemCube.h"
#include "../MyProjectCharacter.h"

void AItemCube::Use(class AMyProjectCharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player %s use CubeItem"), *PlayerCharacter->GetName());
	}
}