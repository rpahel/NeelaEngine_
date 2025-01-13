#include "Common/Clients/sv_Client.h" 

#include <iostream>
#include <fmt/color.h>

#include "Common/sh_logger.h"

ACNet::SV_Client& ACNet::SV_ClientsContainer::CreateNewClient(ENetPeer* clientPeer)
{
	SV_Client& client = m_clients.emplace_back();
	client.clientId = GenerateClientId();
	client.clientPeer = clientPeer;
	client.hasJoinedGame = false;
	return client;
}

void ACNet::SV_ClientsContainer::RemoveClient(ENetPeer* clientPeer)
{
	auto it = std::find_if(m_clients.begin(), m_clients.end(), [&clientPeer](const SV_Client& client) { return client.clientPeer == clientPeer; });
	if(it == m_clients.end())
	{
		ACLog(LogSource::Server, LogType::Warning, "Could not find Client from Peer.Should not happen.\n");
		return;
	}
	m_clients.erase(it);
}

void ACNet::SV_ClientsContainer::RemoveClient(std::uint8_t clientId)
{
	auto it = std::find_if(m_clients.begin(), m_clients.end(), [clientId](const SV_Client& client) { return client.clientId == clientId; });
	if(it == m_clients.end())
		return;
	m_clients.erase(it);
}

const std::vector<ACNet::SV_Client>& ACNet::SV_ClientsContainer::GetClients() const
{
	return m_clients;
}

std::vector<ACNet::SV_Client>& ACNet::SV_ClientsContainer::GetMutableClients()
{
	return m_clients;
}

std::uint8_t ACNet::SV_ClientsContainer::GetInGameClientsCount() const
{
	std::uint8_t result = 0;
	for (const SV_Client& client : m_clients)
		if(client.hasJoinedGame)
			result++;
	return result;
}

const ACNet::SV_Client* ACNet::SV_ClientsContainer::GetClient(std::uint8_t clientId) const
{
	auto it = std::find_if(m_clients.begin(), m_clients.end(), [clientId](const SV_Client& client) { return client.clientId == clientId; });
	if(it == m_clients.end())
		return nullptr;
	return &*it;
}

ACNet::SV_Client* ACNet::SV_ClientsContainer::GetClient(std::uint8_t clientId)
{
	auto it = std::find_if(m_clients.begin(), m_clients.end(), [clientId](const SV_Client& client) { return client.clientId == clientId; });
	if(it == m_clients.end())
		return nullptr;
	return &*it;
}

const ACNet::SV_Client* ACNet::SV_ClientsContainer::GetClient(ENetPeer* clientPeer) const
{
	auto it = std::find_if(m_clients.begin(), m_clients.end(), [&clientPeer](const SV_Client& client) { return client.clientPeer == clientPeer; });
	if(it == m_clients.end())
		return nullptr;
	return &*it;
}

ACNet::SV_Client* ACNet::SV_ClientsContainer::GetClient(ENetPeer* clientPeer)
{
	auto it = std::find_if(m_clients.begin(), m_clients.end(), [&clientPeer](const SV_Client& client) { return client.clientPeer == clientPeer; });
	if(it == m_clients.end())
		return nullptr;
	return &*it;
}

std::uint8_t ACNet::SV_ClientsContainer::GenerateClientId()
{
	std::uint8_t result = 0;
	for(const SV_Client& client : m_clients)
		result = max(result, client.clientId);
	return result + 1;
}
