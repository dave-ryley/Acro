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
#pragma warning ( push )
#pragma warning(disable:4265)


#include "Acro.capnp.h"
#include "Main.capnp.h"
#include <capnp/ez-rpc.h>
#include <kj/exception.h>

#pragma warning ( pop )
#pragma warning ( pop )
#pragma warning ( pop )
#pragma warning ( pop )
#pragma warning ( pop )
#pragma warning ( pop )

const kj::StringPtr ADDRESS = "localhost";
const uint16_t		PORT	= 9926;
class AcroGorbClient::AcroGorbClientImpl
{
public:
	AcroGorbClientImpl() :
		bIsConnected(false),
		Client(ADDRESS, PORT),
		MainCapability(Client.getMain<Main>()),
		AcroCapability()
	{
		UE_LOG(LogTemp, Warning, TEXT("in AcroGorbClientImpl constructor"));
	}

	bool Connect()
	{
		UE_LOG(LogTemp, Warning, TEXT("in AcroGorbClientImpl connect"));

		if (bIsConnected) return true;
		bool ok = MainCapability.healthCheckRequest().send().then(
			[](auto&& response)
			{
				return response.getResult();
			},
			[](kj::Exception&& ex)
			{
				UE_LOG(LogTemp, Error, TEXT("%s"), UTF8_TO_TCHAR(ex.getDescription().cStr()));
				return false;
			}
		).wait(Client.getWaitScope());

		if (!ok) 
		{
			UE_LOG(LogTemp, Warning, TEXT("main cap healthcheck failed"));

			return false;
		}

		AcroCapability = kj::heap(
			MainCapability.getAcroRequest().send().then(
				[](auto&& response)
				{
					return kj::mv(response);
				}
				,
				[](kj::Exception&& ex)
				{
					UE_LOG(LogTemp, Error, TEXT("%s"), UTF8_TO_TCHAR(ex.getDescription().cStr()));
					return kj::mv(ex);
				}
			).wait(Client.getWaitScope()).getAcro()
		);

		bIsConnected = ok;
		return ok;
	}

	bool IsConnected()
	{
		return bIsConnected;
	}

	bool CreateNewGame(FLevelData & Data)
	{
		check(bIsConnected && "Did you forget to call Init()?");
		bool ok = false;
		auto request = AcroCapability->createGameRequest();
		Acro::LevelData::Builder level = request.getLevel();
		level.setLevelName(Data.LevelName.GetPlainANSIString());
		level.setUuid(TCHAR_TO_UTF8(*Data.UUID));
		
		auto response = request.send().then(
			[&ok, this](auto&& response)
			{
				ok = true;
				if (response.getStatus() != Acro::Status::OK)
				{
					UE_LOG(LogTemp, Warning, TEXT("CreateNewGame Request failed with status: %s"), StatusText(response.getStatus()));
					ok = false;
				}
				return kj::mv(response);
			},
			[&ok](kj::Exception&& ex)
			{
				ok = false;
				UE_LOG(LogTemp, Error, TEXT("%s"), UTF8_TO_TCHAR(ex.getDescription().cStr()));
				return kj::mv(ex);
			}
		).wait(Client.getWaitScope());

		return ok;
	}

	bool LoadGames(TArray<FLevelData>& Levels)
	{
		check(bIsConnected && "Did you forget to call Init()?");
		bool ok = false;

		auto response = AcroCapability->loadGamesRequest().send().then(
			[&ok, this](auto&& response)
			{
				ok = true;
				if (response.getStatus() != Acro::Status::OK)
				{
					UE_LOG(LogTemp, Warning, TEXT("LoadGames Request failed with status: %s"), StatusText(response.getStatus()));
					ok = false;
				}
				return MoveTemp(response);
			},
			[&ok](kj::Exception&& ex)
			{
				ok = false;
				UE_LOG(LogTemp, Error, TEXT("%s"), UTF8_TO_TCHAR(ex.getDescription().cStr()));
				return kj::mv(ex);
			}
		).wait(Client.getWaitScope());

		if (!ok) return false;

		auto retrievedLevels = response.getLevels();

		Levels.Reserve(retrievedLevels.size());
		for (auto level : retrievedLevels)
		{
			FLevelData thisLevel;
			thisLevel.LevelName = level.getLevelName().cStr();
			thisLevel.UUID = UTF8_TO_TCHAR(level.getUuid().cStr());
			Levels.Add(MoveTemp(thisLevel));
		}

		return true;
	}

	bool SaveAcroMesh(FString & Uuid, UAcroMesh * Mesh)
	{
		check(bIsConnected && "Did you forget to call Init()?");
		bool ok = false;
		
		auto saveMeshRequest = AcroCapability->saveAcroMeshRequest();
		saveMeshRequest.setLevelUuid(TCHAR_TO_UTF8(*Uuid));
		auto centerPoints = saveMeshRequest.getMesh().initCenterPositions(Mesh->MeshActor->CenterPositionsCount);
		for (int32_t i = 0; i < Mesh->MeshActor->CenterPositionsCount; i++)
		{
			centerPoints[i].setX(Mesh->MeshActor->CenterPositions[i].X);
			centerPoints[i].setY(Mesh->MeshActor->CenterPositions[i].Y);
			centerPoints[i].setZ(Mesh->MeshActor->CenterPositions[i].Z);
		}
	
		auto response = saveMeshRequest.send().then(
			[&ok, this](auto&& response)
			{
				ok = true;
				if (response.getStatus() != Acro::Status::OK)
				{
					UE_LOG(LogTemp, Warning, TEXT("SaveAcroMesh Request failed with status: %s"), StatusText(response.getStatus()));
					ok = false;
				}
				return kj::mv(response);
			},
				[&ok](kj::Exception&& ex)
			{
				ok = false;
				UE_LOG(LogTemp, Error, TEXT("%s"), UTF8_TO_TCHAR(ex.getDescription().cStr()));
				return kj::mv(ex);
			}
		).wait(Client.getWaitScope());

		return ok;
	}

	bool LoadAcroMeshes(FString & Uuid, TArray<UAcroMesh*>* Meshes)
	{
		check(bIsConnected && "Did you forget to call Init()?");
		bool ok = false;

		auto loadMeshesRequest = AcroCapability->loadAcroMeshesRequest();
		loadMeshesRequest.setLevelUuid(TCHAR_TO_UTF8(*Uuid));

		auto response = loadMeshesRequest.send().then(
			[&ok, this](auto&& response)
			{
				ok = true;
				if (response.getStatus() != Acro::Status::OK)
				{
					UE_LOG(LogTemp, Warning, TEXT("LoadAcroMeshes Request failed with status: %s"), StatusText(response.getStatus()));
					ok = false;
				}
				return kj::mv(response);
			},
			[&ok](kj::Exception&& ex)
			{
				ok = false;
				UE_LOG(LogTemp, Error, TEXT("%s"), UTF8_TO_TCHAR(ex.getDescription().cStr()));
				return kj::mv(ex);
			}
		).wait(Client.getWaitScope());

		if (!ok) return false;

		auto retrieveMeshes = response.getMeshes();
		uint32_t meshCount = retrieveMeshes.size();

		Meshes->Reset();
		Meshes->Reserve(meshCount);

		for (uint32_t i = 0; i < meshCount; i++)
		{
			UAcroMesh * mesh = NewObject<UAcroMesh>();
			auto centerPoints = retrieveMeshes[i].getCenterPositions();
			uint32_t numPoints = centerPoints.size();
			mesh->CenterPositions.Reserve(numPoints);
			for (uint32_t j = 0; j < numPoints; j++)
			{
				mesh->CenterPositions.Add(FVector::ZeroVector);
				mesh->CenterPositions[j].X = centerPoints[j].getX();
				mesh->CenterPositions[j].Y = centerPoints[j].getY();
				mesh->CenterPositions[j].Z = centerPoints[j].getZ();
			}
			Meshes->Add(mesh);
		}

		return true;
	}

private:
	TMap<Acro::Status, FString> StatusMap = { 
		TPairInitializer<const Acro::Status&, const FString&>(Acro::Status::OK,						TEXT("OK")),
		TPairInitializer<const Acro::Status&, const FString&>(Acro::Status::INTERNAL_ERROR,			TEXT("INTERNAL_ERROR")),
		TPairInitializer<const Acro::Status&, const FString&>(Acro::Status::INVALID_UUID,			TEXT("INVALID_UUID")),
		TPairInitializer<const Acro::Status&, const FString&>(Acro::Status::LEVEL_ALREADY_EXISTS,	TEXT("LEVEL_ALREADY_EXISTS")),
		TPairInitializer<const Acro::Status&, const FString&>(Acro::Status::UNIMPLEMENTED,			TEXT("UNIMPLEMENTED")),
		TPairInitializer<const Acro::Status&, const FString&>(Acro::Status::UNKNOWN_ERROR,			TEXT("UNKNOWN_ERROR"))
	};
		
	const FString * StatusText(Acro::Status status) const
	{
		return StatusMap.Find(status);
	}

	bool bIsConnected;
	capnp::EzRpcClient Client;
	Main::Client MainCapability;
	kj::Own<Acro::Client> AcroCapability;
};

AcroGorbClient::AcroGorbClient() :
	GorbClient(MakeUnique<AcroGorbClientImpl>())
{

}

AcroGorbClient::~AcroGorbClient()
{

}

bool AcroGorbClient::Connect()
{
	return GorbClient->Connect();
}

bool AcroGorbClient::IsConnected()
{
	return GorbClient->IsConnected();
}


bool AcroGorbClient::CreateNewGame(FLevelData & Data)
{
	return GorbClient->CreateNewGame(Data);
}

bool AcroGorbClient::LoadGames(TArray<FLevelData>& Levels)
{
	return GorbClient->LoadGames(Levels);
}

bool AcroGorbClient::SaveAcroMesh(FString & Uuid, UAcroMesh * Mesh)
{
	return GorbClient->SaveAcroMesh(Uuid, Mesh);
}

bool AcroGorbClient::LoadAcroMeshes(FString & Uuid, TArray<UAcroMesh*>* Meshes)
{
	return GorbClient->LoadAcroMeshes(Uuid, Meshes);
}

