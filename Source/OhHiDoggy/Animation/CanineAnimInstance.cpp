// Fill out your copyright notice in the Description page of Project Settings.
#include "CanineAnimInstance.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"


#if WITH_EDITOR
void UCanineAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	GameplayTagPropertyMap.Initialize(this, ASC);
}

EDataValidationResult UCanineAnimInstance::IsDataValid(TArray<FText>& ValidationErrors)
{
	Super::IsDataValid(ValidationErrors);

	GameplayTagPropertyMap.IsDataValid(this, ValidationErrors);

	return ((ValidationErrors.Num() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif // WITH_EDITOR

void UCanineAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void UCanineAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const ACharacter* Character = Cast<ACharacter>(GetOwningActor());
	if (!Character)
	{
		return;
	}

	// UCharacterMovementComponent* CharMoveComp = CastChecked<UCharacterMovementComponent>(Character->GetCharacterMovement());
	// const FLyraCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();
	// GroundDistance = GroundInfo.GroundDistance;
}