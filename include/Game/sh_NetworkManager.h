#pragma once 
 
 
namespace ACNet
{
	class NetworkManager
	{
	public:
		NetworkManager();
		~NetworkManager();
		NetworkManager(NetworkManager& other) = delete;
		NetworkManager(NetworkManager&& other) = delete;
		NetworkManager& operator=(NetworkManager& other) = delete;
		NetworkManager& operator=(NetworkManager&& other) = delete;
	};
}