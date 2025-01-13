#pragma once 

#include "Common/Network/sh_NetworkSerializer.h"

#include <enet6/enet.h>
#include <vector>

namespace ACNet
{
	class INetworkSerializeable
	{
	public:
		virtual void NetSerialize(ByteArray& byteArray) const = 0;
		virtual void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) = 0;
	};
}