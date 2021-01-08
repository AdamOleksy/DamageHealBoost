// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "ItemCube.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AItemCube : public AItemBase
{
	GENERATED_BODY()

protected:
	virtual void Use(class AMyProjectCharacter* PlayerCharacter) override;
	
};
