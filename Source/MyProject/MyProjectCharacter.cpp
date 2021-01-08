// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProjectCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "DestructibleCharacter.h"
#include "Math/Vector.h"
#include "Items/ActorGrenade.h"
#include "Items/ItemBase.h"
#include "Items/Inventory.h"
#include "Net/UnrealNetwork.h"
#include "Trading/Shop.h"


//////////////////////////////////////////////////////////////////////////
// AMyProjectCharacter

AMyProjectCharacter::AMyProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = PlayerBaseWalkSpeed;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	PlayerCurrentHP = PlayerBaseHP;

	GrenadeSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Grenade spawn"));
	GrenadeSpawnPoint->SetupAttachment(RootComponent);

	CharacterInventory = CreateDefaultSubobject<UInventory>(TEXT("Inventory"));

	bReplicates = true;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMyProjectCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyProjectCharacter::MoveRight);

	PlayerInputComponent->BindAction("PlayerAttack", IE_Pressed, this, &AMyProjectCharacter::PlayerAttack);

	PlayerInputComponent->BindAction("Throw", IE_Pressed, this, &AMyProjectCharacter::Throw);

	PlayerInputComponent->BindAction("InventoryOnOff", IE_Pressed, this, &AMyProjectCharacter::InventoryOnOff);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMyProjectCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyProjectCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AMyProjectCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AMyProjectCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AMyProjectCharacter::OnResetVR);
}


void AMyProjectCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMyProjectCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AMyProjectCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AMyProjectCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMyProjectCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMyProjectCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyProjectCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AMyProjectCharacter::OnDamageTaken(float Damage)
{
	PlayerCurrentHP -= Damage;
	UE_LOG(LogTemp, Error, TEXT("HP = %f"), PlayerCurrentHP);
	if (PlayerCurrentHP <= 0.f)
	{
		KillPlayer();
	}
}

void AMyProjectCharacter::OnHealed(float Heal)
{
	if (PlayerCurrentHP >= PlayerBaseHP)
	{
		return;
	}
	else if (PlayerCurrentHP + Heal >= PlayerBaseHP)
	{
		PlayerCurrentHP = PlayerBaseHP;
	}
	else
	{
		PlayerCurrentHP += Heal;
	}

}

void AMyProjectCharacter::KillPlayer_Implementation()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, (TEXT("Player %s is dead"), GetName() ), true, FVector2D(1.f,1.f));
	Destroy();
}

void AMyProjectCharacter::PickUp(EItemClass Item)
{
	if (Item == EItemClass::SPEED_BOOSTER)
	{
		GetCharacterMovement()->MaxWalkSpeed = PlayerBaseWalkSpeed * SpeedMultipier;
		if (!GetWorldTimerManager().IsTimerActive(BoostTimeHandle))
		{
			BoostCountDown = BoostDuration;
			GetWorldTimerManager().SetTimer(BoostTimeHandle, this, &AMyProjectCharacter::CheckBoostTimeLeft, BoostCountDownInterval, false);
		}
	}
}

void AMyProjectCharacter::ResetBoost()
{
	GetCharacterMovement()->MaxWalkSpeed = PlayerBaseWalkSpeed;
}

void AMyProjectCharacter::CheckBoostTimeLeft()
{
	BoostCountDown -= BoostCountDownInterval;
	if (BoostCountDown <= 0)
	{
		ResetBoost();
	}
	else
	{
		GetWorldTimerManager().SetTimer(BoostTimeHandle, this, &AMyProjectCharacter::CheckBoostTimeLeft, BoostCountDownInterval, false);
	}
}

void AMyProjectCharacter::PlayerAttack()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Player attack!!!!"), true, FVector2D(1.f, 1.f));
	FVector InitLocation = GetActorLocation();
	FVector PlayerDirectionVector = GetActorForwardVector();
	FCollisionShape CollisionShere = FCollisionShape::MakeSphere(500.f);
	//DrawDebugSphere(GetWorld(), InitLocation, CollisionShere.GetSphereRadius(), 10, FColor::Red, false, 1.f);
	TArray<FHitResult> MultipleHits;
	bool isHit = GetWorld()->SweepMultiByChannel(MultipleHits, InitLocation, InitLocation, FQuat::Identity, ECC_MAX, CollisionShere);

	if (isHit)
	{

		for (auto &Hit : MultipleHits)
		{
			ADestructibleCharacter* DC = Cast<ADestructibleCharacter>(Hit.GetActor());
			if (DC)
			{
				FVector PlayerToEnemyVector = GetActorLocation() - DC->GetActorLocation();

				PlayerToEnemyVector.Normalize(0);
				float Dot = FVector::DotProduct(PlayerDirectionVector, PlayerToEnemyVector);
				float angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
				if (angle + PlayerAttackTolerance >= 180.f)
				{
					DC->OnDamageTaken(10.f);
				}
			}
			
		}
	}


}

void AMyProjectCharacter::GetLifetimeReplicatedProps(
	TArray < FLifetimeProperty >& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyProjectCharacter, bIsTrowing);
}


void AMyProjectCharacter::PickUpItem(AItemBase* Item)
{
	if (Item && CharacterInventory)
	{
		CharacterInventory->AddItemToInventory(Item);
	}
}

void AMyProjectCharacter::ThrowOutItem(AItemBase* Item)
{
	if (Item && CharacterInventory)
	{
		CharacterInventory->RemoveFromInventory(Item);
	}
}

void AMyProjectCharacter::UseItem(AItemBase* Item)
{
	if (bIsShoping && Item)
	{
		if (ActiveShop)
		{
			Money += Item->ItemCost;
			CharacterInventory->RemoveFromInventory(Item);
			UE_LOG(LogTemp, Warning, TEXT("you have %i"), Money);
			ActiveShop->CharacterInventory->AddItemToInventory(Item);
		}
	}
	else if (!bIsShoping && Item)
	{
		Item->Use(this);
		Item->OnUse(this);
	}
}

void AMyProjectCharacter::InventoryOnOff()
{
	
	if (bIsShoping)
	{
		CharacterInventory->ShowShopingInventory();
		return;
	}

	bIsInventoryActivated = !bIsInventoryActivated;

	if (bIsInventoryActivated && CharacterInventory)
	{
		CharacterInventory->ShowInventory();
	}
	else if (!bIsInventoryActivated && CharacterInventory)
	{
		CharacterInventory->HideInventory();
	}
}

void AMyProjectCharacter::Throw_Implementation()
{

}


bool AMyProjectCharacter::Throw_Validate()
{
	bIsTrowing = !bIsTrowing;

	SpawnGrenade_Validate();
	
	return true;
}

void AMyProjectCharacter::PlayThrowAnimation_Implementation()
{

}

bool AMyProjectCharacter::PlayThrowAnimation_Validate()
{
	if (ThrowAnimationRef)
	{
		PlayAnimMontage(ThrowAnimationRef, 1.f, NAME_None);
	}
	
	return true;
}


void AMyProjectCharacter::SpawnGrenade_Implementation()
{

}

bool AMyProjectCharacter::SpawnGrenade_Validate()
{
	FVector SpawnLocation = GrenadeSpawnPoint->GetComponentLocation();
	FRotator SpawnRotation = GrenadeSpawnPoint->GetComponentRotation();

	AActorGrenade* TempProjetile = GetWorld()->SpawnActor<AActorGrenade>(GrenadeBP, SpawnLocation, SpawnRotation);

	return true;
}

void AMyProjectCharacter::ResetIsTrowing()
{
	bIsTrowing = false;
}

void AMyProjectCharacter::SetActiveShop(AShop* NewShop)
{
	ActiveShop = NewShop;
	bIsShoping = true;
}

void AMyProjectCharacter::LeaveShop()
{
	ActiveShop = nullptr;
	bIsShoping = false;
}