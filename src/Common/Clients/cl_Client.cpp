#include "Common/Clients/cl_Client.h" 

#include <stdexcept>

ACNet::CL_Client& ACNet::CL_ClientsContainer::AddClient(std::string username, bool isLocalPlayer)
{
	CL_Client& client = m_clients.emplace_back();
	client.username = username;
	client.isLocalPlayer = isLocalPlayer;
	return client;
}

ACNet::CL_Client& ACNet::CL_ClientsContainer::AddClient(std::uint8_t clientId, std::string username, bool isLocalPlayer)
{
	CL_Client& client = AddClient(std::move(username), isLocalPlayer);
	client.clientId = clientId;
	return client;
}

void ACNet::CL_ClientsContainer::RemoveClient(std::uint8_t clientId)
{
	auto it = std::find_if(m_clients.begin(), m_clients.end(), [clientId](const CL_Client& client) { return client.clientId == clientId; });
	if(it == m_clients.end())
		return;
	m_clients.erase(it);
}

const std::vector<ACNet::CL_Client>& ACNet::CL_ClientsContainer::GetClients() const
{
	return m_clients;
}

std::vector<ACNet::CL_Client>& ACNet::CL_ClientsContainer::GetMutableClients()
{
	return m_clients;
}

const ACNet::CL_Client* ACNet::CL_ClientsContainer::GetClient(std::uint8_t clientId) const
{
	auto it = std::find_if(m_clients.begin(), m_clients.end(), [clientId](const CL_Client& client) { return client.clientId == clientId; });
	if(it == m_clients.end())
		return nullptr;
	return &*it;
}

ACNet::CL_Client* ACNet::CL_ClientsContainer::GetClient(std::uint8_t clientId)
{
	auto it = std::find_if(m_clients.begin(), m_clients.end(), [clientId](const CL_Client& client) { return client.clientId == clientId; });
	if(it == m_clients.end())
		return nullptr;
	return &*it;
}

const ACNet::CL_Client* ACNet::CL_ClientsContainer::GetLocalClient() const
{
	auto it = std::find_if(m_clients.begin(), m_clients.end(), [](const CL_Client& client) { return client.isLocalPlayer; });
	if(it == m_clients.end())
		throw std::runtime_error("[CLIENT] No LocalPlayer found in Client's ClientsContainer");
	return &*it;
}

ACNet::CL_Client* ACNet::CL_ClientsContainer::GetLocalClient()
{
	auto it = std::find_if(m_clients.begin(), m_clients.end(), [](const CL_Client& client) { return client.isLocalPlayer; });
	if(it == m_clients.end())
		throw std::runtime_error("[CLIENT] No LocalPlayer found in Client's ClientsContainer");
	return &*it;
}
