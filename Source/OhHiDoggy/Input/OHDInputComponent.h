// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OHDInputConfig.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MappableConfigPair.h"
#include "OhHiDoggy/FOHDGameplayTags.h"
#include "type_traits"
#include "OhHiDoggy/OHDLogChannels.h"
#include "OHDInputComponent.generated.h"

struct FGameplayTag;
/**
 * 
 */
UCLASS()
class OHHIDOGGY_API UOHDInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	/* Capture some additional data args if object method needs them */
	template <typename UserClass, typename FuncType, typename... VarTypes>
	auto MakeLambda(UserClass* Obj, FuncType Func, VarTypes... Vars)
	{
		return [Obj, Func, Vars...](const FInputActionValue& Value) -> decltype(auto)
		{
			return (Obj->*Func)(Value, Vars...);
		};
	}
	
	/* We can bind a UObject but such what will also take some extra arguments aside from those bound to it, maybe here I could bind lambda that would capture vars, and accept value arg as argument  */
	template< class FuncType, class UserClass, typename... VarTypes >
	FEnhancedInputActionEventBinding& BindActionWithValues(const UInputAction* Action, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, VarTypes... Vars)
	{
		TUniquePtr<FEnhancedInputActionEventDelegateBinding<FEnhancedInputActionHandlerValueSignature>> AB = MakeUnique<FEnhancedInputActionEventDelegateBinding<FEnhancedInputActionHandlerValueSignature>>(Action, TriggerEvent);
		AB->Delegate.MakeDelegate().BindLambda(MakeLambda(Object, Func, Vars...));
		return *EnhancedActionEventBindings.Add_GetRef(MoveTemp(AB));
	}
	
public:

	UOHDInputComponent(const FObjectInitializer& ObjectInitializer);

	void AddInputMappings(const UOHDInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMappings(const UOHDInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UOHDInputConfig* InputConfig, const FGameplayTag& InputTag,
	                                     ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func,
	                                     bool bLogIfNotFound);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UOHDInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);
	
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActionsWithInputData(const UOHDInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);

	void AddInputConfig(const FLoadedMappableConfigPair& ConfigPair, UEnhancedInputLocalPlayerSubsystem* InputSubsystem);
	void RemoveInputConfig(const FLoadedMappableConfigPair& ConfigPair, UEnhancedInputLocalPlayerSubsystem* InputSubsystem);

	/* Some fun lambdas to use one day some where maybe? :) */
	template <typename UserClass, typename PressedFuncType>
	auto MakeLambda(UserClass* Obj, PressedFuncType Func)
	{
		return [Obj, Func](auto&&... Args) -> decltype(auto)
		{
			return (Obj->*Func)(std::forward<decltype(Args)>(Args)...);
		};
	}

	//somehow fit this lambda signature with the signature of passed in function???
	template<typename TObj, typename TFunc, typename Arg>
	auto MakeLambdaAbilityWithTagAndInputValue(TObj* Obj, TFunc Func, Arg FirstArg)
	{
		return [Obj, Func, FirstArg](auto&&... Args) -> decltype(auto)
		{
			return (Obj->*Func)(FirstArg, std::forward<decltype(Args)>(Args)...);
		};
	}

	template <typename TObj, typename TFunc, typename... ArgsOuter>
	auto MakeLambdaDoubleBind(TObj* Obj, TFunc Func, ArgsOuter... Args)
	{
		auto args_outer = std::make_tuple(Args...);

		return [Obj, Func, args_outer](auto&&... ArgsInner) -> decltype(auto)
		{
			auto args_inner = std::make_tuple(std::forward<decltype(ArgsInner)>(ArgsInner)...);

			auto args = std::tuple_cat(args_outer, args_inner);

			return std::apply([&Obj, &Func](auto&&... Args) -> decltype(auto)
			{
				return (Obj->*Func)(std::forward<decltype(Args)>(Args)...);
			}, args);
		};
	}
};

/* Binds actions to input actions with tags, delegates and owners */
template<class UserClass, typename FuncType>
void UOHDInputComponent::BindNativeAction(const UOHDInputConfig* InputConfig,
                                                         const FGameplayTag& InputTag, ETriggerEvent TriggerEvent,
                                                         UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);//dynamic signature, does it mean that it can take all args or just a few of them?
	}
}

/* In upper function native input action is found by the provided tag, in put config is only needed as it contains all the input actions, when we have our input action IA retrieved, we bind it to trigger type, we pass the owner object, and the function,
 * somehow that function that we are binding has access to FInputActionValue, so maybe the same would be possible with ability? Ability delegate receives only tag, and from tag finds the ability,
 * contrary to bind native actions which received all input actions set and input tag and from in it found the exact input action existing in the game and bound one specific value from it to delegate function,
 * bind ability actions lookup from AbilityInputActions that return pair class (input action and associated tag) and bound one specific value of it (tag) to the delegate function (which ten uses it to find the ability),
 * there are three signatures in enhanced input for delegates, value, instance (handler for value: DECLARE_DELEGATE_OneParam(FEnhancedInputActionHandlerValueSignature, const FInputActionValue&);)
 * then the declared multicast signature is executed with all current arguments: inline void FEnhancedInputActionEventDelegateBinding<FEnhancedInputActionHandlerDynamicSignature>::Execute(const FInputActionInstance& ActionData) const
 * then MAYBE I can use a delegate that can have input value, and input tag, and with both i can find also the ability and pass the input value to it? ability binding uses simple action handler with zero args so how it binds tag to delegate???
 *
 * 1. Try doing two approaches: first could fail: if we bind the single signature it may be bound to just fixed non dynamic action value? because the input action with current values is retrievable only as passed into event that wraps the signature? Because only Execute method has
 *		access to the instanced (runtime) input action data and if signature allows it, only then it can be passed to it, and with Delegate.MakeDelegate, the original delegate is substituted by a new method that has no access to instanced IA(todo to check) or:
 * 2. Use Bind Action that allow access to input action in full, and by that allows to retrieve an ability by IA/Tag, and call event on it with input action value
 * 3. Try to use 2 but pass additional arg input tag.
 *
 * Difference between BindAction for native and for abilities is that first just binds delegate but the second one makes one.
 */

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UOHDInputComponent::BindAbilityActionsWithInputData(const UOHDInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FDoggyInputAction& Action : InputConfig->AbilityInputActionsWithInputData)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)//todo maybe pass also trigger event type???
			{
				UE_LOG(LogOHDAbilitySystem, Display, TEXT("Binding action input for tag [%s], input action name: [%s]"), *Action.InputTag.GetTagName().ToString(), *Action.InputAction->GetName());
				BindHandles.Add(BindActionWithValues(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag).GetHandle());//since it is signature with no args apparently, is it then bound to the last arg that is input tag apparently and called with it? maybe it is possible to pass there any number of args?
			}

			// if (ReleasedFunc)
			// {
			// 	BindHandles.Add(BindActionWithValues(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			// }
		}
	}
}

/* Binds abilities to input actions with tags, delegates and owners, the difference with actions is that it returned bounded handles (uint32) to bindings. */
template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UOHDInputComponent::BindAbilityActions(const UOHDInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FDoggyInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());//since it is signature with no args apparently, is it then bound to the last arg that is input tag apparently and called with it? maybe it is possible to pass there any number of args?
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}

