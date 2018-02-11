// Copyright@2017

#include "NetworkGameMode.h"
#include "KBEvent.h"
#include "KBEngine.h"
#include "KBEMain.h"
#include "NetworkComponent/NetworkBaseComponent.h"
#include "Interface/NetworkActorInterface.h"
#include "GameControl/VRGameInstance.h"

ANetworkGameMode::ANetworkGameMode() : mapKey( 1 )
{
	gameModeRoot = CreateDefaultSubobject<USceneComponent>( TEXT( "Root" ) );
	RootComponent = gameModeRoot;

	mainComponent = CreateDefaultSubobject<UKBEMain>( TEXT( "MainComponent" ) );
	this->AddOwnedComponent( mainComponent );
}

void ANetworkGameMode::BeginPlay()
{
	Super::BeginPlay();

	if ( isLoadFromConfig )
	{
		mainComponent->ip = ipAddress;
		mainComponent->port = port;
		mainComponent->SEND_BUFFER_MAX = sendBufferSize;
		mainComponent->RECV_BUFFER_MAX = recvBufferSize;

		mainComponent->updateKBEData();
	}

	KBENGINE_REGISTER_EVENT( "onKicked" , onKicked );
	KBENGINE_REGISTER_EVENT( "onDisconnected" , onDisconnected );

	// in world
	KBENGINE_REGISTER_EVENT( "onEnterWorld" , onEnterWorld );
	KBENGINE_REGISTER_EVENT( "onLeaveWorld" , onLeaveWorld );
	KBENGINE_REGISTER_EVENT( "onEnterSpace" , onEnterSpace );
	KBENGINE_REGISTER_EVENT( "onLeaveSpace" , onLeaveSpace );

	checkOpenLevel();

	APawn *firstPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
	if ( firstPawn )
	{
		allPlayers.Add( firstPawn );
	}
}

void ANetworkGameMode::EndPlay( const EEndPlayReason::Type EndPlayReason )
{
	KBENGINE_DEREGISTER_ALL_EVENT();
	Super::EndPlay( EndPlayReason );
}

UNetworkBaseComponent* ANetworkGameMode::findGameEntity( int entityID )
{
	UNetworkBaseComponent** pGameEntity = networkEntityMap.Find( entityID );
	if (pGameEntity)
	{
		//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("  networkEntityMap add id:  %d "), entityID));
		return *pGameEntity;
	}
	return nullptr;
}

void ANetworkGameMode::addGameEntity( int entityID , UNetworkBaseComponent* entity )
{
	networkEntityMap.Add( entityID , entity );
}

void ANetworkGameMode::removeGameEntity( int entityID )
{
	networkEntityMap.Remove( entityID );
}

void ANetworkGameMode::checkOpenLevel()
{
	UVRGameInstance *gameInstance = Cast<UVRGameInstance>( UGameplayStatics::GetGameInstance( this ) );
	if ( !gameInstance ) return;
	if ( !gameInstance->isOpenLevel ) return;

	gameInstance->isOpenLevel = false;

	Entity *entity = KBEngineApp::getSingleton().player();
	Avatar *avatar = static_cast<Avatar*>( entity );
	if ( avatar )
	{
		avatar->loginToSpace( mapKey );
	}
}

const TArray<AActor*> & ANetworkGameMode::getAllPlayers()
{
	return allPlayers;
}

///////////////////////////////////////////////////////////////////////////////////////
void ANetworkGameMode::onKicked( const UKBEventData *pEventData )
{

}

void ANetworkGameMode::onDisconnected( const UKBEventData *pEventData )
{
}

///////////////////////////////////////////////////////////////////////////////////////
void ANetworkGameMode::onEnterWorld( const UKBEventData* pEventData )
{
	const UKBEventData_onEnterWorld *pData = Cast<UKBEventData_onEnterWorld>( pEventData );
	if ( pData->isPlayer ) return;

	UNetworkBaseComponent *entity = findGameEntity( pData->entityID );
	if ( entity ) return;
	
	FRotator Rot;
	KBDir2UE4Dir( Rot , pData->direction );
	FTransform SpawnTransform( Rot , pData->position );

	const FString &className = pData->entityClassName;
	if ( className == TEXT( "Avatar" ) )
	{
		createAvatar( pData->entityID , false , SpawnTransform );
		checkUnhandledHands( pData->entityID );
	}
	else if ( className == TEXT( "Hand" ) )
	{
		createHand( pData->entityID );
	}
	else if ( className == TEXT( "GeneralObj" ) )
	{

	}
}

void ANetworkGameMode::onLeaveWorld( const UKBEventData* pEventData )
{
	const UKBEventData_onLeaveWorld *pData = Cast<UKBEventData_onLeaveWorld>( pEventData );
	if ( pData->isPlayer ) return;

	destroyActorByID( pData->entityID , false );
}

void ANetworkGameMode::onEnterSpace( const UKBEventData* pEventData )
{
	const UKBEventData_onEnterSpace *pData = Cast<UKBEventData_onEnterSpace>( pEventData );
	if ( !pData->isPlayer ) return;

	UNetworkBaseComponent *entity = findGameEntity( pData->entityID );
	if ( entity ) return;

	APlayerController *player = UGameplayStatics::GetPlayerController( this , 0 );
	APawn *oldPawn = player->GetPawn();

	allPlayers.Remove( oldPawn );
	createAvatar( pData->entityID , true , oldPawn->GetActorTransform() );
	APawn *newPawn = Cast<APawn>( findGameEntity( pData->entityID )->GetOwner() );
	
	if ( player && newPawn )
	{
		player->Possess( newPawn );
		oldPawn->Destroy();
	}

	checkUnhandledHands( pData->entityID );
}

void ANetworkGameMode::onLeaveSpace( const UKBEventData* pEventData )
{
	const UKBEventData_onLeaveSpace *pData = Cast<UKBEventData_onLeaveSpace>( pEventData );
	if ( !pData->isPlayer ) return;

	destroyActorByID( pData->entityID , true );
}

UNetworkBaseComponent *ANetworkGameMode::createActorByName( const FString& className , const FTransform &SpawnTransform , int32 entityID , bool isLocal )
{
	UClass **actorClass = name2Class.Find( className );
	if ( !actorClass ) return nullptr;

	AActor *createdActor = GetWorld()->SpawnActor( *actorClass , &SpawnTransform );
	UNetworkBaseComponent *networkComp = Cast<UNetworkBaseComponent>( createdActor->GetComponentByClass( UNetworkBaseComponent::StaticClass() ) );
	EntityBase *networkEntity = static_cast<EntityBase *>( KBEngineApp::getSingleton().findEntity( entityID ) );

	if ( networkComp != nullptr && networkEntity != nullptr )
	{
		networkComp->linkToKBEObject( networkEntity , isLocal );
		networkEntity->linkToUnrealObject( networkComp );
	}
	return networkComp;
}

void ANetworkGameMode::destroyActorByID( int32 entityID , bool isPlayer )
{
	UNetworkBaseComponent *networkComp = findGameEntity( entityID );
	if ( networkComp != nullptr )
	{
		removeGameEntity( entityID );
		networkComp->breakFromKBEObject();
		networkComp->GetOwner()->Destroy();

		if ( isPlayer )
		{
			allPlayers.Remove( networkComp->GetOwner() );
		}
	}

	EntityBase *networkEntity = static_cast<EntityBase *>( KBEngineApp::getSingleton().findEntity( entityID ) );
	if ( networkEntity != nullptr )
	{
		networkEntity->breakFromUnrealObject();
	}
}

void ANetworkGameMode::createAvatar( int32 entityID , bool isLocal , const FTransform& spawnTrans )
{
	Avatar *avatar = static_cast<Avatar*>( KBEngineApp::getSingleton().player() );
	bool isGuide = avatar->authority == 1;

	FString avatarClassName = isLocal ? ( isGuide ? TEXT( "AvatarLocalGuide" ) : TEXT( "AvatarLocal" ) ) : TEXT( "AvatarRemote" );
	UNetworkBaseComponent *createdComp = createActorByName( avatarClassName , spawnTrans , entityID , isLocal );
	addGameEntity( entityID , createdComp );

	AActor *owner = createdComp->GetOwner();
	INetworkActorInterface *netInterface = Cast<INetworkActorInterface>( owner );
	if ( netInterface )
	{
		netInterface->OnJoinSpace();
	}

	allPlayers.Add( owner );
}

void ANetworkGameMode::createHand( int32 entityID )
{
	Hand *networkHand = static_cast<Hand *>( KBEngineApp::getSingleton().findEntity( entityID ) );
	if ( networkHand )
	{
		int32 avatarID = networkHand->avatarEntityID;

		bool isLocal = avatarID == KBEngineApp::getSingleton().entity_id();
		bool isLeft = networkHand->isLeft == 1;
		
		UNetworkBaseComponent *netComp = findGameEntity( avatarID );
		if ( netComp )//能找到 手所属的Pawn
		{
			AVRCharacterBase *pawn = Cast<AVRCharacterBase>( netComp->GetOwner() );
			if ( pawn )
			{
				Avatar *handAvatar = static_cast<Avatar *>( KBEngineApp::getSingleton().findEntity( avatarID ) );

				UNetworkBaseComponent *netHandComp = nullptr;
				if ( isLeft )
				{
					netHandComp = Cast<UNetworkBaseComponent>(
						pawn->leftController->GetComponentByClass( UNetworkBaseComponent::StaticClass() ) );
					pawn->leftController->updatePlayerIndex( handAvatar->playerIndex );
				}
				else
				{
					netHandComp = Cast<UNetworkBaseComponent>(
						pawn->rightController->GetComponentByClass( UNetworkBaseComponent::StaticClass() ) );
					pawn->rightController->updatePlayerIndex( handAvatar->playerIndex );
				}
				
				addGameEntity( entityID , netHandComp );
				netHandComp->linkToKBEObject( networkHand , isLocal );
				networkHand->linkToUnrealObject( netHandComp );
			}
		}
		else//找不到Pawn 只能先把手存起来 等pawn创建出来再说
		{
			TArray<int32> *ret = unhandledHands.Find( avatarID );
			if ( ret )
			{
				ret->Add( entityID );
			}
			else
			{
				unhandledHands.Add( avatarID , { entityID } );
			}
		}
	}
}

void ANetworkGameMode::checkUnhandledHands( int avatarID )
{
	TArray<int32>* ret = unhandledHands.Find( avatarID );
	if ( ret )
	{
		for ( int32 val : *ret )
		{
			createHand( val );
		}
		unhandledHands.Remove( avatarID );
	}
}
