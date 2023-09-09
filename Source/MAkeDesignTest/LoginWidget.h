// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Http.h"
#include "HttpModule.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class MAKEDESIGNTEST_API ULoginWidget : public UUserWidget
{
   GENERATED_BODY()
protected:
    virtual void NativeConstruct() override;

    // UMG widget pointers
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<class UButton> RequestButton;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<class UEditableTextBox> UsernameInput;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<class UEditableTextBox> PasswordInput;

    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    TObjectPtr<class UTextBlock> ResponseText;

    // Server address that can be edited in Blueprint
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MyWidget")
    FString ServerAddress;

public:
    // Function called when the Request button is clicked
    UFUNCTION(BlueprintCallable, Category = "MyWidget")
    void OnRequestButtonClicked();

    // Function for sending a GET request
    UFUNCTION(BlueprintCallable, Category = "MyWidget")
    void SendGetRequest();

    // Function to handle the HTTP request completion
    void OnHttpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess);
};
	
