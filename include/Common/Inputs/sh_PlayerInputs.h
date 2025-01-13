#pragma once

#include "Common/Network/sh_INetworkSerializeable.h"

namespace ACGame
{
	struct PlayerInputs : ACNet::INetworkSerializeable
	{
		bool m_Up = false;
		bool m_Down = false;
		bool m_Left = false;
		bool m_Right = false;

		std::optional<ACNet::NetworkEntityId> m_interactedNetworkEntityId;

		// Debug
		bool m_Die = false;

		//==== Methods

		void Reset()
		{
			m_Up	= false;
			m_Down	= false;
			m_Left	= false;
			m_Right = false;
			m_interactedNetworkEntityId.reset();
			m_Die	= false;
		}

		//==== INetworkSerializeable

		void NetSerialize(ByteArray& byteArray) const override
		{
			// Les 4 bits de droite seront ceux du deplacement.

			Byte moveInputs = 0;
			(moveInputs |= m_Up)	<<= 1;
			(moveInputs |= m_Down)	<<= 1;
			(moveInputs |= m_Left)	<<= 1;
			(moveInputs |= m_Right);

			// Les 4 bits de gauche seront les autres inputs.
			Byte otherInputs = 0;
			(otherInputs |= m_Die) <<= 7;

			ACNet::NetworkSerializer::Serialize_u8(byteArray, moveInputs | otherInputs);
			ACNet::NetworkSerializer::Serialize_u8(byteArray, m_interactedNetworkEntityId.has_value());
			if(m_interactedNetworkEntityId.has_value())
				ACNet::NetworkSerializer::Serialize_NetworkEntityId(byteArray, m_interactedNetworkEntityId.value());
		}

		void NetDeserialize(const ByteArray& byteArray, std::size_t& offset) override
		{
			Byte inputs = ACNet::NetworkSerializer::Deserialize_u8(byteArray, offset);

			m_Up	= inputs & 8;
			m_Down	= inputs & 4;
			m_Left	= inputs & 2;
			m_Right = inputs & 1;

			m_Die	= inputs & 128;

			if(ACNet::NetworkSerializer::Deserialize_u8(byteArray, offset))
				m_interactedNetworkEntityId.emplace(ACNet::NetworkSerializer::Deserialize_NetworkEntityId(byteArray, offset));
			else
				m_interactedNetworkEntityId.reset();
		}

		bool operator==(const PlayerInputs& other) const
		{
			if(m_Up != other.m_Up
				|| m_Down != other.m_Down
				|| m_Left != other.m_Left
				|| m_Right != other.m_Right
				|| m_Die != other.m_Die)
				return false;

			if(m_interactedNetworkEntityId.has_value() != other.m_interactedNetworkEntityId.has_value())
				return false;
			if(m_interactedNetworkEntityId.has_value())
			{
				return m_interactedNetworkEntityId.value() == other.m_interactedNetworkEntityId.value();
			}
			return true;
		}
	};
}