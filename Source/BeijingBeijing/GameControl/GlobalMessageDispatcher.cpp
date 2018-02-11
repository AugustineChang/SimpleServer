// Copyright@2017

#include "GlobalMessageDispatcher.h" 

UGlobalMessageDispatcher::UGlobalMessageDispatcher(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UGlobalMessageDispatcher *UGlobalMessageDispatcher::GetGlobalMessager( bool &isValid )
{
	isValid = false;
	UGlobalMessageDispatcher* DataInstance = Cast<UGlobalMessageDispatcher>( GEngine->GameSingleton );

	if ( !DataInstance ) return nullptr;
	if ( !DataInstance->IsValidLowLevel() ) return nullptr;

	isValid = true;
	return DataInstance;
}