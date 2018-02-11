// Copyright@2017

#include "LoginBox.h"
#include "Network/NetworkComponent/NetworkAccountComponent.h"

// Sets default values
ALoginBox::ALoginBox()
{
	rootComp = CreateDefaultSubobject<USceneComponent>( TEXT( "RootComp" ) );
	RootComponent = rootComp;

	networkComp = CreateDefaultSubobject<UNetworkAccountComponent>( TEXT( "NetworkComp" ) );
	this->AddOwnedComponent( networkComp );
}

// Called when the game starts or when spawned
void ALoginBox::BeginPlay()
{
	Super::BeginPlay();
}

void ALoginBox::OnLoginGame_Implementation()
{

}

void ALoginBox::OnOfflineGame_Implementation()
{

}

void ALoginBox::OnStartGame_Implementation()
{

}
