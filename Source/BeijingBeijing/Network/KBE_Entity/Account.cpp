#include "Account.h"
#include "EntityBase.h"
#include "KBEngine.h"
#include "Network/NetworkComponent/NetworkAccountComponent.h"

ENTITYDEF_CLASS_REGISTER( Account, EntityBase )

Account::Account(): EntityBase()
{
}

Account::~Account()
{
}

void Account::__init__()
{
	// 触发登陆成功事件
	UKBEventData_onLoginSuccessfully* pEventData = NewObject<UKBEventData_onLoginSuccessfully>();
	pEventData->entity_uuid = KBEngineApp::getSingleton().entity_uuid();
	pEventData->entity_id = id();
	KBENGINE_EVENT_FIRE( "onLoginSuccessfully" , pEventData );
}

void Account::onDestroy()
{
	// 注销注册的所有事件
	KBENGINE_DEREGISTER_ALL_EVENT();
}

void Account::reqStartGame( const FString &name , uint8 authority , uint8 charType )
{
	baseCall( "startGame" , name , authority , charType );
}

void Account::reqLogout()
{
	baseCall( "logoutGame" );
}

void Account::onGameStarted( uint8 retCode )
{
	if ( !unrealObject ) return;
	UNetworkAccountComponent *accountComp = Cast<UNetworkAccountComponent>( unrealObject );
	accountComp->onGameStarted( retCode == 1 );
}
