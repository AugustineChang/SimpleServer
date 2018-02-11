// Copyright@2017

#pragma once

#include "VRCameraComponent.h"
#include "Interface/NetworkActorInterface.h"
#include "GameFramework/Character.h"
#include "MotionControllerBase.h"
#include "VRCharacterBase.generated.h"


UCLASS()
class BEIJINGBEIJING_API AVRCharacterBase : public ACharacter , public INetworkActorInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	
	virtual void Destroyed() override;

	UFUNCTION( BlueprintImplementableEvent , Category = Network )
	void OnJoinSpace();

public:
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Camera" )
	UVRCameraComponent *VRCamera;

	UPROPERTY( VisibleAnywhere , BlueprintReadOnly , Category = "MontionCharacter" )
	UAudioComponent *voiceAudioComponent;

	UPROPERTY( VisibleAnywhere , BlueprintReadOnly , Category = "MontionCharacter" )
	class ULipSyncMicInputComponent *lipSyncMicInput;

	UPROPERTY( VisibleAnywhere , BlueprintReadOnly , Category = "MontionCharacter" )
	class ULipSyncMorphTargetComponent *lipSyncMorphTarget;

	UPROPERTY( VisibleAnywhere , BlueprintReadOnly , Category = "MontionCharacter" )
	class UNetworkAvatarComponent *networkPlayer;

	UPROPERTY( EditAnywhere , Category = "MontionCharacter" )
	TSubclassOf<AMotionControllerBase> ControllerClass;

	UPROPERTY( BlueprintReadWrite , Category = "MontionCharacter" )
	AMotionControllerBase *leftController;

	UPROPERTY( BlueprintReadWrite , Category = "MontionCharacter" )
	AMotionControllerBase *rightController;

	UPROPERTY( EditAnywhere , Category = "MontionCharacter" )
	float MoveSpeed;

	UPROPERTY( EditAnywhere , Category = "MontionCharacter" )
	float HeadMoveSpeed;

	UPROPERTY( EditAnywhere , Category = "MontionCharacter" )
	float backDistance;

	UPROPERTY( EditAnywhere , Category = "MontionCharacter" )
	float TurnRightRate;

	UPROPERTY( BlueprintReadOnly , Category = "MontionCharacter" )
	float walkSpeed;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void onReceiveVoiceData( const TArray<uint8> &receiveData );

protected:

	void updateCharacterTransform();
	void MoveForward( float val );
	void TurnAround( float val );
	void PressTouchPad( bool isPress );

	//Input
	//////////////////////////////////////////////////////////////////////
	void TriggerLeftPress();
	void TriggerLeftRelease();

	void TriggerRightPress();
	void TriggerRightRelease();

	void GripLeftPress();
	void GripLeftRelease();

	void GripRightPress();
	void GripRightRelease();

	void ThumbstickLeftPress();
	void ThumbstickLeftRelease();

	void ThumbstickRightPress();
	void ThumbstickRightRelease();

	void ThumbstickLeft_X( float value );
	void ThumbstickLeft_Y( float value );

	void ThumbstickRight_X( float value );
	void ThumbstickRight_Y( float value );

	//////////////////////////////////////////////////////////////////////

private:

	void createSoundWave();
	void playVoiceData();

	float turnTimer;
	bool isPressLeftTouch;
	bool isOculus;
	
	TSharedPtr<class IVoiceDecoder> voiceDecoder;
	
	TArray<uint8> receiveVoiceData;
	
	int32 curReceiveVoiceData;
	int32 maxRawCaptureDataSize;
	
	FVector TempRelativeLocation;
	FVector TempWorldLocation;
};
