// Fill out your copyright notice in the Description page of Project Settings.

#include "Acro.h"
#include "GorbSceneComponent.h"
#include "GorbClient.h"

#include "Runtime/Core/Public/Misc/Guid.h"
#include "Runtime/Core/Public/Containers/UnrealString.h"
#include "Runtime/Core/Public/HAL/UnrealMemory.h"

// Sets default values for this component's properties
UGorbSceneComponent::UGorbSceneComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGorbSceneComponent::BeginPlay()
{
	Super::BeginPlay();

	GorbClient client("localhost:8080");

	const FGuid guid = FGuid::NewGuid();


	char uuidBuf[37];
	memcpy(uuidBuf, TCHAR_TO_UTF8(*guid.ToString().ToLower()), 33);

	kj::StringPtr uuid(uuidBuf, 32);
	kj::StringPtr content = "test content";
	

	if (client.Connect())
	{
		UE_LOG(LogTemp, Warning, TEXT("Health Check successful!"));

		::capnp::MallocMessageBuilder message;
		auto insertParams = message.initRoot<FileManager::File>();
		
		insertParams.setName(uuid);
		insertParams.setUuid(uuid);
		insertParams.initContent(content.size());
		insertParams.setContent(kj::ArrayPtr<const kj::byte>(reinterpret_cast<const unsigned char *>(content.cStr()), content.size()));

		::capnp::MallocMessageBuilder message2;
		auto insertResults = message2.initRoot<FileManager::InsertResults>();

		auto insertSuccess = client.insert(insertParams.asReader(), insertResults); //change to status
		auto nameOrphan = insertParams.disownName();
		auto uuidOrphan = insertParams.disownUuid();


		if (insertSuccess && insertResults.getStatus() == FileManager::Status::OK)
		{
			UE_LOG(LogTemp, Warning, TEXT("Successfully inserted: %s"), UTF8_TO_TCHAR(uuid.cStr()));
			::capnp::MallocMessageBuilder message3;
			auto retrieveResult = message3.initRoot<FileManager::RetrieveResults>();
			auto retrieveSuccess = client.retrieve(uuid, retrieveResult);

			if (retrieveSuccess && retrieveResult.getStatus() == FileManager::Status::OK)
			{
				UE_LOG(LogTemp, Warning, TEXT("Successfully retrieved: %s"), UTF8_TO_TCHAR(retrieveResult.getFile().getUuid().cStr()));
				UE_LOG(LogTemp, Warning, TEXT("Has File?: %s"), (retrieveResult.hasFile() ? TEXT("True") : TEXT("False")));
				UE_LOG(LogTemp, Warning, TEXT("Content size in component: %d"), retrieveResult.getFile().getContent().size());

				//UE_LOG(LogTemp, Warning, TEXT("Successfully retrieved:"));
				auto fileChars = retrieveResult.getFile().getContent().asChars();
				kj::StringPtr str(fileChars.begin(), fileChars.end());
				UE_LOG(LogTemp, Warning, TEXT("Retrieved data: %s"), UTF8_TO_TCHAR(str.cStr()));
				//UE_LOG(LogTemp, Warning, TEXT("Retrieved data size: %d"), resp.getFile().getContent().size());
				//UE_LOG(LogTemp, Warning, TEXT("Retrieved data:"));


				::capnp::MallocMessageBuilder message4;
				auto deleteResults = message4.initRoot<FileManager::RemoveResults>();
				auto deleteSuccess = client.remove(uuid, deleteResults);
				if (deleteSuccess && deleteResults.getStatus() == FileManager::Status::OK)
				{
					UE_LOG(LogTemp, Warning, TEXT("Deleted %s successfully"), UTF8_TO_TCHAR(uuid.cStr()));
				}
			}
		}
	}
		/*

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Health Check failed!"));
	}
		*/
	// ...
	
}


// Called every frame
void UGorbSceneComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



