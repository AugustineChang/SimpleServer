// Copyright@2017

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WidgetComponent.h"
#include "KeyboardActor.generated.h"

UENUM(Blueprintable)
enum class keyType :uint8
{
	letters = 0,
	capitals = 1,
	numbers = 2,
	punctuations = 3,
	functions = 4,
	specialInputs = 5
};

UCLASS()
class BEIJINGBEIJING_API AKeyboardActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AKeyboardActor(const FObjectInitializer& objectInitializer);
	UPROPERTY(VisibleAnywhere)
		USceneComponent* sceneComponent;

	UPROPERTY(VisibleAnywhere , BlueprintReadOnly )
		UWidgetComponent* uiWidget;

protected:
	//UWidgetComponentgetComponentCalled when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UFUNCTION(BlueprintCallable, Category = keyboard)
		const TArray<FString>& getLettersArray() { return lettersArray; }

	UFUNCTION(BlueprintCallable, Category = keyboard)
		const TArray<FString>& getCapitalsArray() { return capitalsArray; }

	UFUNCTION(BlueprintCallable, Category = keyboard)
		const TArray<FString>& getNumbersArray() { return numbersArray; }

	UFUNCTION(BlueprintCallable, Category = keyboard)
		const TArray<FString>& getPunctuationsArray() { return punctuationsArray; }

	UFUNCTION(BlueprintCallable, Category = keyboard)
		const TArray<FString>& getFunctionsArray() { return functionsArray; }

	UFUNCTION(BlueprintCallable,Category=keyboard)
		const TArray<FString>& getSpecialInputsArray() { return specialInputsArray; }


	UFUNCTION(BlueprintCallable, Category = keyboard)
		FString setBoardText(FString preText, keyType insertType, int32 index);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = keyboard)
		void showKeyboard(bool isShow);
	UPROPERTY(BlueprintReadWrite,Category=keyboard)
		bool isShowKeyboard;

	UFUNCTION(BlueprintImplementableEvent, Category = keyboard)
		void changeLetterCase(bool isUp);//�л���Сд,����ͼ��ʵ�֣��ı������Ӧ��ö�٣��ı������Ӧ���ַ�

	UFUNCTION(BlueprintImplementableEvent, Category = keyboard)
		void switchPunctuation(bool isPunct);//�л���ĸ/���ܼ�������ͼ��ʵ�֣��ı������Ӧ��ö�٣��ı��ַ�
	UPROPERTY(BlueprintReadOnly)
		bool isCapital;

private:

	UPROPERTY()
		TArray<FString> lettersArray;
	UPROPERTY()
		TArray<FString> capitalsArray;
	UPROPERTY()
		TArray<FString> numbersArray;
	UPROPERTY()
		TArray<FString> punctuationsArray;
	UPROPERTY()
		TArray<FString> functionsArray;
	UPROPERTY()
		TArray<FString> specialInputsArray;

	UPROPERTY()
		bool isPunctuation;

	UFUNCTION()
		FString removeChar(FString & previousString);
	UFUNCTION()//�����
		FString outputInsertText(FString previousText, FString insertText);
	UFUNCTION()//���ܼ�
		FString outputFunctionText(FString previousText, int32 index);
	UFUNCTION()//����������
		FString outputSpecialInputText(FString previousText, int index);

	UFUNCTION()
		void leftShiftFunction();
	UFUNCTION()
		void symbolFunction();
	UFUNCTION()
		FString backspaceFunction(FString preText);
	UFUNCTION()
		void enterFunction();

};
