// Copyright 2017-2020 nasuVR All Rights Reserved.

#include "NetworkAccountComponent.h"
#include "KBEvent.h"
#include "KBEMain.h"
#include "Network/LoginBox.h"
#include "Network/KBE_Entity/Account.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Network/NetworkGameMode.h"


UNetworkAccountComponent::UNetworkAccountComponent( const FObjectInitializer& ObjectInitializer )
	:Super( ObjectInitializer ) , userName( TEXT( "nasuvr" ) ) , password( "1234" ) , 
	totalNum( 100 ) , curNum( 0 ) , isStartGame( false )
{
}

// Called when the game starts or when spawned
void UNetworkAccountComponent::BeginPlay()
{
	Super::BeginPlay();

	gameMode = Cast<ANetworkGameMode>( UGameplayStatics::GetGameMode( this ) );

	//login
	KBENGINE_REGISTER_EVENT( "onDisconnected" , onDisconnected );
	KBENGINE_REGISTER_EVENT( "onConnectionState" , onConnectionState );
	KBENGINE_REGISTER_EVENT( "onCreateAccountResult" , onCreateAccountResult );
	KBENGINE_REGISTER_EVENT( "onLoginFailed" , onLoginFailed );
	KBENGINE_REGISTER_EVENT( "onVersionNotMatch" , onVersionNotMatch );
	KBENGINE_REGISTER_EVENT( "onScriptVersionNotMatch" , onScriptVersionNotMatch );
	KBENGINE_REGISTER_EVENT( "onLoginBaseappFailed" , onLoginBaseappFailed );
	KBENGINE_REGISTER_EVENT( "onLoginSuccessfully" , onLoginSuccessfully );
	KBENGINE_REGISTER_EVENT( "onLoginBaseapp" , onLoginBaseapp );
	KBENGINE_REGISTER_EVENT( "Loginapp_importClientMessages" , Loginapp_importClientMessages );
	KBENGINE_REGISTER_EVENT( "Baseapp_importClientMessages" , Baseapp_importClientMessages );
	KBENGINE_REGISTER_EVENT( "Baseapp_importClientEntityDef" , Baseapp_importClientEntityDef );
}

void UNetworkAccountComponent::EndPlay( const EEndPlayReason::Type endPlayReason )
{
	Super::EndPlay( endPlayReason );

	if ( endPlayReason == EEndPlayReason::EndPlayInEditor || 
		endPlayReason == EEndPlayReason::Quit )
	{
		reqLogout();
	}
}

void UNetworkAccountComponent::autoLogin()
{
	if ( gameMode == nullptr )
	{
		ALoginBox *loginBox = Cast<ALoginBox>( GetOwner() );
		loginBox->OnOfflineGame();
	}
	else
	{
		FString realUsername = userName.Append( FString::FromInt( curNum ) );
		TArray<uint8> loginExtraData;
		bool ret = gameMode->getKBEMain()->login( realUsername , password , loginExtraData );
		if ( !ret )
		{
			ALoginBox *loginBox = Cast<ALoginBox>( GetOwner() );
			loginBox->OnOfflineGame();
		}
		else
			curNum++;
	}
}

void UNetworkAccountComponent::onGameStarted( bool isSuccess )
{
	//登陆到场景中
	if ( isSuccess )
	{
		ALoginBox *loginBox = Cast<ALoginBox>( GetOwner() );
		loginBox->OnStartGame();

		isStartGame = true;
	}
	else
	{
		reqLogout();
	}
}

void UNetworkAccountComponent::reqStartGame( const FString &name , uint8 authority , uint8 charType )
{
	if ( networkEntity == nullptr ) return;

	Account *account = static_cast<Account*>( networkEntity );
	account->reqStartGame( name , authority , charType );
}

void UNetworkAccountComponent::reqLogout()
{
	if ( networkEntity == nullptr ) return;
	if ( isStartGame ) return;

	Account *account = static_cast<Account*>( networkEntity );
	account->reqLogout();
}

void UNetworkAccountComponent::onDisconnected( const UKBEventData *pEventData )
{
	isStartGame = false;
}

void UNetworkAccountComponent::onConnectionState( const UKBEventData *pEventData )
{
	const UKBEventData_onConnectionState *pData = Cast<UKBEventData_onConnectionState>( pEventData );
	if ( !pData->success )//连接服务器失败（可能原因 1.没网 2.ip地址写错 3.服务器未启动）
	{
		ALoginBox *loginBox = Cast<ALoginBox>( GetOwner() );
		loginBox->OnOfflineGame();
	}
}

////////////////////////////////////////////////////////////////////////////////////////
void UNetworkAccountComponent::onCreateAccountResult( const UKBEventData *pEventData )
{
	const UKBEventData_onCreateAccountResult *realData = Cast<UKBEventData_onCreateAccountResult>( pEventData );
	if ( !realData ) return;

	if ( realData->errorCode > 0 )
	{
		UKismetSystemLibrary::PrintString( this ,
			FString::Printf( TEXT( "connect(%s,%s)error!" ) , *FString::FromInt( realData->errorCode ) , *realData->errorStr ) );
	}
	else
	{
		UKismetSystemLibrary::PrintString( this , FString( "create Account Successfully!!" ) );
	}
}

void UNetworkAccountComponent::onLoginFailed( const UKBEventData *pEventData )
{
	//登录失败（原因 用户重名需要换一个再试）
	const UKBEventData_onLoginFailed *realData = Cast<UKBEventData_onLoginFailed>( pEventData );
	if ( !realData ) return;

	UKismetSystemLibrary::PrintString( this ,
		FString::Printf( TEXT( "login failed,error=%s" ) , *realData->errorStr ) );

	int32 failCode = realData->failedcode;
	//8 操作繁忙 9 在另一处登陆了 10 拒绝再次登陆
	if ( failCode == 8 || failCode == 9 || failCode == 10 )
	{
		autoLogin();
	}
}

void UNetworkAccountComponent::onVersionNotMatch( const UKBEventData *pEventData )
{

}

void UNetworkAccountComponent::onScriptVersionNotMatch( const UKBEventData *pEventData )
{

}

void UNetworkAccountComponent::onLoginBaseappFailed( const UKBEventData *pEventData )
{
	const UKBEventData_onLoginBaseappFailed *realData = Cast<UKBEventData_onLoginBaseappFailed>( pEventData );
	if ( !realData ) return;

	UKismetSystemLibrary::PrintString( this ,
		FString::Printf( TEXT( "login Baseapp failed,error=%s" ) , *realData->errorStr ) );
}

void UNetworkAccountComponent::onLoginSuccessfully( const UKBEventData *pEventData )
{
	//登陆成功
	const UKBEventData_onLoginSuccessfully *realData = Cast<UKBEventData_onLoginSuccessfully>( pEventData );
	if ( !realData )return;

	EntityBase *networkEntity = static_cast<EntityBase *>( KBEngineApp::getSingleton().findEntity( realData->entity_id ) );
	if ( networkEntity )
	{
		networkEntity->linkToUnrealObject( this );
		this->linkToKBEObject( networkEntity , true );
	}

	ALoginBox *loginBox = Cast<ALoginBox>( GetOwner() );
	loginBox->OnLoginGame();
}

void UNetworkAccountComponent::onLoginBaseapp( const UKBEventData *pEventData )
{

}

void UNetworkAccountComponent::Loginapp_importClientMessages( const UKBEventData *pEventData )
{
	UKismetSystemLibrary::PrintString( this , FString( "Loginapp_importClientMessages..." ) );
}

void UNetworkAccountComponent::Baseapp_importClientMessages( const UKBEventData *pEventData )
{
	UKismetSystemLibrary::PrintString( this , FString( "Baseapp_importClientMessages.." ) );
}

void UNetworkAccountComponent::Baseapp_importClientEntityDef( const UKBEventData *pEventData )
{
	UKismetSystemLibrary::PrintString( this , FString( "Baseapp_importClientEntityDef.." ) );
}
