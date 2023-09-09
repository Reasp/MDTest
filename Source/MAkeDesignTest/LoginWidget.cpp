// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "HttpModule.h"
#include "JsonUtilities.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


void ULoginWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // Привязываем функцию SendRequest к событию OnClicked кнопки RequestButton
    if (RequestButton)
    {
        RequestButton->OnClicked.AddDynamic(this, &ULoginWidget::SendGetRequest);
    }
}

void ULoginWidget::OnRequestButtonClicked()
{
    // Call the function to send a GET request when the button is clicked
    SendGetRequest();
}

void ULoginWidget::SendGetRequest()
{
    // Get the username and password from the input fields
    FString Username = UsernameInput->GetText().ToString();
    FString Password = PasswordInput->GetText().ToString();

    // URL for the GET request
    FString Url = ServerAddress + "?login=" + Username + "&password=" + Password;

    // Create an HTTP client
    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetVerb("GET");
    Request->SetURL(Url);

    // Set a handler for the response
    Request->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::OnHttpRequestComplete);

    // Send the request
    Request->ProcessRequest();
}

void ULoginWidget::OnHttpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bSuccess)
{
    if (bSuccess && Response.IsValid())
    {
        // Check the response status
        if (Response->GetResponseCode() == EHttpResponseCodes::Ok)
        {
            // Parse the JSON response
            FString ResponseString = Response->GetContentAsString();
            TSharedPtr<FJsonObject> JsonObject;
            TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseString);

            if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
            {
                // JSON processing successful
                FString FormattedResponse;

                // Iterate through all fields of the JSON object
                for (const auto& Field : JsonObject->Values)
                {
                    FString FieldName = Field.Key;
                    FString FieldValue;
                    Field.Value->TryGetString(FieldValue);

                    // Format the string as "field_name : value" and add it to FormattedResponse
                    FormattedResponse += FieldName + " : " + FieldValue + "\n";
                }

                // Set the formatted response in the text field
                ResponseText->SetText(FText::FromString(FormattedResponse));
            }
            else
            {
                // JSON parsing error
                ResponseText->SetText(FText::FromString("JSON parsing error."));
            }
        }
        else
        {
            // HTTP status error
            ResponseText->SetText(FText::FromString("HTTP status error: " + FString::FromInt((int32)Response->GetResponseCode())));
        }
    }
    else
    {
        // HTTP request error
        ResponseText->SetText(FText::FromString("HTTP request error."));
    }
}
