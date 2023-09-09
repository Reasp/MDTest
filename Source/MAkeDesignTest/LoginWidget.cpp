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
    const FString Username = UsernameInput->GetText().ToString();
    const FString Password = PasswordInput->GetText().ToString();

    TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
    Request->SetVerb("POST");
    Request->SetURL(ServerAddress);
    Request->SetHeader("Content-Type", "application/json");

    const FString Content = "email=" + Username + "&password=" + Password;
    Request->SetContentAsString(Content);

    Request->OnProcessRequestComplete().BindUObject(this, &ULoginWidget::OnHttpRequestComplete);
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
            const FString ResponseString = Response->GetContentAsString();
            TSharedPtr<FJsonObject> JsonObject;
            const TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseString);

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
