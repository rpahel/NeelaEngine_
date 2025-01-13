#pragma once
#include <cstdint>
#include "Common/sh_logger.h"

namespace ACGame
{
    struct GameData
    {
        std::uint32_t money = 0;
        std::uint8_t moneyChangedCount = 0;

        bool HasMoney(std::uint32_t cost)
        {
            return money >= cost;
        }

        void AddMoney(std::uint32_t gain)
        {
            money += gain;
            moneyChangedCount++;
            ACLog(ACNet::LogSource::Common, ACNet::LogType::InfoMoney, "Added {0} to the bank (new count: {1})\n", gain, money);
        }

        void RemoveMoney(std::uint32_t cost)
        {
            assert(cost <= money);
            money -= cost;
            moneyChangedCount++;
            ACLog(ACNet::LogSource::Common, ACNet::LogType::InfoMoney, "Removed {0} to the bank (new count: {1})\n", cost, money);
        }

        void SetMoney(std::uint32_t newMoney)
        {
            money = newMoney;
            moneyChangedCount++;
            ACLog(ACNet::LogSource::Common, ACNet::LogType::InfoMoney, "Bank new count: {0}\n", money);
        }
    };
}
