// Copyright © 2018-2019 David Ryley and David Ryan. All rights reserved.

#include "AcroGorbClient.h"

#pragma warning ( push )
#pragma warning(disable:4583)
#pragma warning ( push )
#pragma warning(disable:4068)
#pragma warning ( push )
#pragma warning(disable:4668)
#pragma warning ( push )
#pragma warning(disable:4521)
#pragma warning ( push )
#pragma warning(disable:4541)

#include "GorbClient.h"
#include "Acro.capnp.h"

#pragma warning ( pop )
#pragma warning ( pop )
#pragma warning ( pop )
#pragma warning ( pop )
#pragma warning ( pop )


const kj::StringPtr GORB_ADDRESS = "localhost:8080";

class AcroGorbClient::AcroGorbClientImpl
{
public:
	AcroGorbClientImpl() :
		Client(GORB_ADDRESS)
	{

	}

	bool IsConnected()
	{
		return Client.HealthCheck();
	}

	bool InsertLevelData(FLevelData & Data)
	{
		capnp::MallocMessageBuilder message;
		auto insertParams = message.initRoot<Gorb::Object>();

		const kj::StringPtr name = Data.LevelName.GetPlainANSIString();
		const kj::StringPtr uuid = TCHAR_TO_UTF8(*Data.UUID);

		insertParams.setName(name);
		insertParams.setType(Gorb::Type::LEVEL_DATA);
		insertParams.setUuid(uuid);

		auto root = insertParams.getData().initAs<Acro::LevelData>();
		root.setLevelName(name);
		root.setLevelSegments(0);
		root.setUuid(uuid);

		capnp::MallocMessageBuilder resultMessage;

		auto response = Client.insert(insertParams.asReader(), resultMessage);

		UE_LOG(LogTemp, Warning, TEXT("Gorb response: %d"), (int)response.getStatus());

		return response.getStatus() == Gorb::Status::OK;
	}

	bool RetrieveAllLevels(TArray<FLevelData>& Levels)
	{
		kj::StringPtr noUuid = "";
		capnp::MallocMessageBuilder message;
		auto retrieveRoot = message.initRoot<Gorb::ObjectBroker::RetrieveResults>();
		retrieveRoot.getObject().setType(Gorb::Type::LEVEL_DATA_ALL);
		auto retrieveBuilder = retrieveRoot;
		auto response = Client.retrieve(noUuid, retrieveBuilder);

		if (response.getStatus() != Gorb::Status::OK)
		{
			UE_LOG(LogTemp, Warning, TEXT("Retrieve Levels failed. Status %d returned."), (int)response.getStatus());
			return false;
		}

		auto object = response.getObject();
		auto levels = object.getData().getAs<capnp::AnyList>().as<capnp::List<Acro::LevelData>>();

		Levels.Reserve(levels.size());

		for (const auto & level : levels)
		{
			FLevelData current;
			current.LevelName = FName(level.getLevelName().cStr());
			current.UUID = FString(level.getUuid().cStr());
			current.LevelSegments = level.getLevelSegments();
			Levels.Add(current);
		}
		
		return true;
	}


private:
	::GorbClient Client;
};

AcroGorbClient::AcroGorbClient() :
	GorbClient(MakeUnique<AcroGorbClientImpl>())
{

}

AcroGorbClient::~AcroGorbClient()
{
}

bool AcroGorbClient::IsConnected()
{
	return GorbClient->IsConnected();
}


bool AcroGorbClient::InsertLevelData(FLevelData & Data)
{
	return GorbClient->InsertLevelData(Data);
}

bool AcroGorbClient::RetrieveAllLevels(TArray<FLevelData>& Levels)
{
	return GorbClient->RetrieveAllLevels(Levels);
}
