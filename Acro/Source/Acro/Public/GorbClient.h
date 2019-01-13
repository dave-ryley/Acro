//#pragma comment(lib,"Ws2_32.lib")
#include "Acro.h"

#include <kj/string.h>

#include "FileManager.capnp.h"
#include "capnp/ez-rpc.h"

using InsertResults = FileManager::InsertResults::Builder;
using RetrieveResults = FileManager::RetrieveResults::Builder;
using RemoveResults = FileManager::RemoveResults::Builder;

class GorbClient
{
public:
	GorbClient();
	GorbClient(const kj::StringPtr address);
	bool HealthCheck();
	bool Connect();

	bool insert(const FileManager::File::Reader & file, InsertResults & out);
	bool retrieve(const kj::StringPtr UUID, RetrieveResults & out);
	bool remove(const kj::StringPtr UUID, RemoveResults & out);
private:
	kj::StringPtr m_address;
	bool m_isConnected;
	capnp::EzRpcClient m_client;
	FileManager::Client m_fileManagerCap;
};
