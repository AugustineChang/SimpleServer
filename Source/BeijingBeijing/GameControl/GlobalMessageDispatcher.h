// Copyright@2017

#pragma once
 
#include "GlobalMessageDispatcher.generated.h"
 
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMessageDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam( FMessageDelegate_float , float , Val );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams( FMessageDelegate_string , const FString& , cmd , const FString& , paramStr );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams( FMessageDelegate_string_bool , const FString& , cmd , const FString& , paramStr , bool , isNetworkSend );

UCLASS(Blueprintable, BlueprintType)
class UGlobalMessageDispatcher : public UObject
{
	GENERATED_BODY()
public:
    UGlobalMessageDispatcher(const FObjectInitializer& ObjectInitializer);
 
	///////////Trigger/////////////

	UPROPERTY( BlueprintCallable , BlueprintAssignable, Category = "GlobalMessager")
	FMessageDelegate OnLeftTrigger_Pressed;

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate OnLeftTrigger_Released;

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate OnRightTrigger_Pressed;

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate OnRightTrigger_Released;

	///////////Grip/////////////

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate OnLeftGrip_Pressed;

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate OnLeftGrip_Released;

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate OnRightGrip_Pressed;

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate OnRightGrip_Released;

	///////////Thumbstick Button/////////////

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate OnLeftThumbstick_Pressed;

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate OnLeftThumbstick_Released;

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate OnRightThumbstick_Pressed;

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate OnRightThumbstick_Released;

	///////////Thumbstick Axis/////////////

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate_float LeftThumbstickAxisX;

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate_float LeftThumbstickAxisY;

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate_float RightThumbstickAxisX;

	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate_float RightThumbstickAxisY;
	
	///////////Custom Action/////////////
	
	//重要的自定义事件 可以用来做大部分事情 会自动发送到网络
	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate_string_bool DoCustomAction;

	//重要的自定义事件 接受送网络发来的事件 会自动调用调用DoCustomAction
	//大多数时候不需要绑定 也可以绑定这个事件 来做一些特殊操作
	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate_string OnGetCustomAction;

	//手部菜单点击事件 用来切换界面
	UPROPERTY( BlueprintCallable , BlueprintAssignable , Category = "GlobalMessager" )
	FMessageDelegate_string handWidgetAction;

	///////////Singleton/////////////

	UFUNCTION( BlueprintPure , BlueprintCallable , Category = "GlobalMessager" )
	static UGlobalMessageDispatcher *GetGlobalMessager( bool &isValid );
};