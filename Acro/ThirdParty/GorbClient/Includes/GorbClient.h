#pragma once

#include "ObjectBroker.capnp.h"
#include <kj/string.h>
#include <capnp/ez-rpc.h>

using InsertResults_R 		= Gorb::ObjectBroker::InsertResults::Reader;
using RetrieveResults_R 	= Gorb::ObjectBroker::RetrieveResults::Reader;
using RetrieveResults_B 	= Gorb::ObjectBroker::RetrieveResults::Builder;
using RemoveResults_R		= Gorb::ObjectBroker::RemoveResults::Reader;
using Object_R 				= Gorb::Object::Reader;
using Object_B 				= Gorb::Object::Builder;
using ObjectBrokerClient 	= Gorb::ObjectBroker::Client;

class GorbClient
{
public:
	GorbClient() = delete;
	GorbClient(const kj::StringPtr address);
	bool HealthCheck();
	bool Connect();

	InsertResults_R 	insert(const Object_R & Object, capnp::MessageBuilder & out);
	RetrieveResults_R 	retrieve(const kj::StringPtr UUID, RetrieveResults_B & out);
	RemoveResults_R 	remove(const kj::StringPtr UUID, capnp::MessageBuilder & out);
private:
	kj::StringPtr m_address;
	bool m_isConnected;
	capnp::EzRpcClient m_client;
	ObjectBrokerClient m_objectBrokerCap;
};
