// Copyright@2017

#include "KeyboardActor.h"

// Sets default values
AKeyboardActor::AKeyboardActor(const FObjectInitializer& objectInitializer)
	:Super(objectInitializer),isPunctuation(false),isShowKeyboard(false),isCapital(false)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("sceneComponent"));
	RootComponent = sceneComponent;

	uiWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("UIWidget"));
	uiWidget->AttachToComponent(sceneComponent, FAttachmentTransformRules::KeepRelativeTransform);

	lettersArray = { "a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z" };
	capitalsArray = { "A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z" };
	numbersArray = { "1","2","3","4","5","6","7","8","9","0" };
	punctuationsArray = { "~","!","/","#","$","%","^","&","*","_","-","+","=","\"","(",")","[","]","|",":",";","'","<",">","?","\\"};
	functionsArray = { "LestShift","Enter","Symbol" };//leftshft(大小写),enter(确认,键盘收回),Symbol(符号)
	specialInputsArray = { "Backspace" ,"@","dot" ,"spacebar","com","comma" };//backspace(删除)comma(,)

}

// Called when the game starts or when spawned
void AKeyboardActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AKeyboardActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


FString AKeyboardActor::outputInsertText(FString previousText, FString insertText)
{
	previousText += insertText;
	return previousText;
}

FString AKeyboardActor::outputFunctionText(FString previousText, int32 index)
{
	switch (index)
	{
	case 0://大小写
		leftShiftFunction();
		return previousText;

	case 1://enter
		enterFunction();
		return previousText;

	case 2://Symbol
		symbolFunction();
		return previousText;
	default:
		break;
	}
	return " ";
}

FString AKeyboardActor::outputSpecialInputText(FString previousText, int index)
{
	switch (index)
	{
	case 0://backapace
		return backspaceFunction(previousText);

	case 1://@
		return outputInsertText(previousText, "@");

	case 2://dot
		return outputInsertText(previousText, ".");

	case 3://spacebar
		return outputInsertText(previousText, " ");

	case 4://.com
		return outputInsertText(previousText, ".com");

	case 5://comma
		return outputInsertText(previousText, ",");

	default:
		break;
	}
	return " ";
}

FString AKeyboardActor::setBoardText(FString preText, keyType insertType, int32 index)
{
	switch (insertType)
	{
	case keyType::letters:
		if (!lettersArray.IsValidIndex(index)) break;
		return outputInsertText(preText, lettersArray[index]);

	case keyType::capitals:
		if (!capitalsArray.IsValidIndex(index))break;
		return outputInsertText(preText, capitalsArray[index]);

	case keyType::numbers:
		if (!numbersArray.IsValidIndex(index))break;
		return outputInsertText(preText, numbersArray[index]);

	case keyType::punctuations:
		if (!punctuationsArray.IsValidIndex(index))break;
		return outputInsertText(preText, punctuationsArray[index]);

	case keyType::functions:
		return outputFunctionText(preText, index);

	case  keyType::specialInputs:
		return outputSpecialInputText(preText, index);

	default:
		break;
	}
	return " ";
}

//切换大小写,绑定到对用的键上
void AKeyboardActor::leftShiftFunction()
{
	isCapital = !isCapital;
	changeLetterCase(isCapital);
}

//按键功能切换
void AKeyboardActor::symbolFunction()
{
	isPunctuation = !isPunctuation;
	switchPunctuation(isPunctuation);
}

//键盘收回操作
void AKeyboardActor::enterFunction()
{
	if (isShowKeyboard)
	{
		isShowKeyboard = !isShowKeyboard;
		showKeyboard(isShowKeyboard);
	}
}

FString AKeyboardActor::backspaceFunction(FString previousText)
{
	return removeChar(previousText);
}

FString AKeyboardActor::removeChar(FString & previousString)
{
	TArray<FString> tempStringArray;
	if (previousString.IsValidIndex(previousString.Len()-1))
	{
		previousString.RemoveAt(previousString.Len() - 1, 1, true);
	}
	return previousString;
}