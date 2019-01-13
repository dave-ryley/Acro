#include "Acro.h"

#include "GorbClient.h"
#include "Runtime/Core/Public/Containers/UnrealString.h"
#include <kj/memory.h>

GorbClient::GorbClient() :
	m_address(kj::mv("localhost:8080")),
	m_isConnected(false),
	m_client(m_address),
	m_fileManagerCap(m_client.getMain<FileManager>())
{

}

GorbClient::GorbClient(const kj::StringPtr address) :
	m_address(address),
	m_isConnected(false),
	m_client(m_address),
	m_fileManagerCap(m_client.getMain<FileManager>())
{

}

bool GorbClient::Connect()
{
	if (m_isConnected)
	{
		return true;
	}

	m_isConnected = HealthCheck();

	return m_isConnected;
}

bool GorbClient::HealthCheck()
{
	m_isConnected = false;

	auto promise = m_fileManagerCap.healthCheckRequest().send().then([this](auto&& response)
	{
		return response.getResult();
	},
	[this](kj::Exception&& ex)
	{

		UE_LOG(LogTemp, Warning, TEXT("Client HealthCheck failed: %s"), UTF8_TO_TCHAR(ex.getDescription().cStr()));
		return false;
	});

	m_isConnected = promise.wait(m_client.getWaitScope());
	return m_isConnected;
}

bool GorbClient::insert(const FileManager::File::Reader & file, InsertResults & out)
{
	bool success = false;
	auto req = m_fileManagerCap.insertRequest();
	req.setInput(file);
	UE_LOG(LogTemp, Warning, TEXT("Client inserting: %s"), UTF8_TO_TCHAR(req.getInput().getUuid().cStr()));
	auto response = req.send().then([this, &success](auto&& response)
	{
		success = true;
		return kj::mv(response);
	},
	[this, &success](kj::Exception&& ex)
	{
		UE_LOG(LogTemp, Warning, TEXT("Client insert failed: %s"), UTF8_TO_TCHAR(ex.getDescription().cStr()));
		success = false;
		return kj::mv(ex);
	}).wait(m_client.getWaitScope());

	out.setStatus(response.getStatus());

	return success;
}

bool GorbClient::retrieve(kj::StringPtr UUID, RetrieveResults & out)
{
	bool success = false;
	auto req = m_fileManagerCap.retrieveRequest();
	UE_LOG(LogTemp, Warning, TEXT("Client retrieving: %s"), UTF8_TO_TCHAR(UUID.cStr()));

	req.setUuid(UUID);

	auto response = req.send().then([this, &success](auto&& response)
	{
		UE_LOG(LogTemp, Warning, TEXT("Content size in unreal: %d"), response.getFile().getContent().size());
		success = true;
		return kj::mv(response);
	},
	[this, &success](kj::Exception&& ex)
	{
		success = false;
		UE_LOG(LogTemp, Warning, TEXT("Client retrieve failed: %s"), UTF8_TO_TCHAR(ex.getDescription().cStr()));
		return kj::mv(ex);
	}).wait(m_client.getWaitScope());

	out.setStatus(response.getStatus());
	out.setFile(response.getFile());
	//kj::StringPtr str(fileChars.begin(), fileChars.end());
	//UE_LOG(LogTemp, Warning, TEXT("Retrieved data size: %d"), resp.getFile().getContent().size());
	//UE_LOG(LogTemp, Warning, TEXT("Retrieved data: %s"), UTF8_TO_TCHAR(str.cStr()));
	return success;
}

bool GorbClient::remove(kj::StringPtr UUID, RemoveResults & out)
{
	bool success = false;
	auto req = m_fileManagerCap.removeRequest();
	req.setUuid(UUID);

	auto response = req.send().then([this, &success](auto&& response)
	{
		success = true;
		return kj::mv(response);
	},
	[this, &success](kj::Exception&& ex)
	{
		success = false;
		KJ_LOG(ERROR, "Client remove failed: ", ex.getDescription().cStr());
		return kj::mv(ex);
	}).wait(m_client.getWaitScope());

	out.setStatus(response.getStatus());

	return success;
}
