#pragma once

#include <endstone/endstone.hpp>
#include <boost/unordered/concurrent_flat_map.hpp>
#include <vector>
#include <filesystem>
#include <optional>
#include <string>
#include "log.hpp"

class CombatManager {
private:
    boost::concurrent_flat_map<endstone::UUID, endstone::UUID> playersInPvP;
    boost::concurrent_flat_map<endstone::UUID, int> pvpTime;
    std::vector<endstone::UUID> playersToKill;
    endstone::Plugin &plugin;
    std::filesystem::path dataFolder;
public:
    CombatManager(endstone::Plugin &plugin) : plugin(plugin), dataFolder(plugin.getDataFolder() / "latest.log") {
        if (!std::filesystem::exists(plugin.getDataFolder())) {
            std::filesystem::create_directories(plugin.getDataFolder());
        }
        plugin.getServer().getScheduler().runTaskAsync(plugin, [path = dataFolder]() {
            log("CombatManager was successfuly loaded!", path);
        });
    }

	std::optional<std::string> registerCombat(endstone::UUID player, endstone::UUID opponent) {
		try {
			if (playersInPvP.contains(player) || playersInPvP.contains(opponent) || pvpTime.contains(player) || pvpTime.contains(opponent)) {
				return std::string(player.str() + " or " + opponent.str() + " is already in PvP!");
			}
			playersInPvP.try_emplace(player, opponent);
			playersInPvP.try_emplace(opponent, player);
			pvpTime.try_emplace(player, 30);
			pvpTime.try_emplace(opponent, 30);
			plugin.getLogger().info(std::string(player.str() + " has hitted by " + opponent.str()));
			plugin.getServer().getScheduler().runTaskAsync(plugin, [dataFolder = dataFolder, player = player, opponent = opponent]() {
				log(std::string(player.str() + " has hitted by " + opponent.str()), dataFolder);
			});
			return std::nullopt;
		} catch (const std::exception &e) {
			plugin.getLogger().error(e.what());
			plugin.getServer().getScheduler().runTaskAsync(plugin, [e = e, dataFolder = dataFolder]() {
				log(e.what(), dataFolder);
			});
			return e.what();
		}
	}

	std::optional<std::string> changeOpponent(endstone::UUID player, endstone::UUID opponent) {
		try {
			if (!playersInPvP.contains(player)) {
				return std::string(player.str() + " isn't in PvP!");
			}
			bool expired = pvpTime.erase_if(player, [](const auto &pair) {
				return pair.second <= 0;
			});
			if (expired) {
				playersInPvP.erase(player);
				return std::string(player.str() + " isn't in PvP!");
			}
			playersInPvP.insert_or_assign(player, opponent);
			playersInPvP.insert_or_assign(opponent, player);
			pvpTime.insert_or_assign(player, 30);
			pvpTime.insert_or_assign(opponent, 30);
			plugin.getLogger().info(std::string(player.str() + " has hitted by " + opponent.str()));
			plugin.getServer().getScheduler().runTaskAsync(plugin, [dataFolder = dataFolder, player = player, opponent = opponent]() {
				log(std::string(player.str() + " has hitted by " + opponent.str()), dataFolder);
			});
			return std::nullopt;
		} catch (const std::exception &e) {
			plugin.getLogger().error(e.what());
			plugin.getServer().getScheduler().runTaskAsync(plugin, [e = e, dataFolder = dataFolder]() {
				log(e.what(), dataFolder);
			});
			return e.what();
		}
	}

	std::optional<std::string> deleteCombat(endstone::UUID player) {
		try {
			std::optional<endstone::UUID> opponent = std::nullopt;
			playersInPvP.cvisit(player, [&opponent](const auto &pair) {
				opponent = pair.second;
			});
			if (!opponent.has_value()) {
				pvpTime.erase(player);
				return std::string(player.str() + " isn't in PvP!");
			}
			bool expired = pvpTime.erase_if(player, [](const auto &pair) {
				return pair.second <= 0;
			});
			playersInPvP.erase(player);
			pvpTime.erase(player);
			if (expired) {
				return std::string(player.str() + " isn't in PvP!");
			}
			plugin.getServer().getScheduler().runTaskAsync(plugin, [dataFolder = dataFolder, player = player]() {
				log(std::string(player.str() + " has leaved from PvP"), dataFolder);
			});
			return std::nullopt;
		} catch (const std::exception &e) {
			plugin.getLogger().error(e.what());
			plugin.getServer().getScheduler().runTaskAsync(plugin, [e = e, dataFolder = dataFolder]() {
				log(e.what(), dataFolder);
			});
			return e.what();
		}
	}

	std::optional<int> getCombatTime(endstone::UUID player) {
		try {
			if (!playersInPvP.contains(player)) return std::nullopt;
			std::optional<int> result = std::nullopt;
			bool erased = pvpTime.erase_if(player, [&result](const auto &pair) {
				if (pair.second <= 0) {
					return true;
				}
				result = pair.second;
				return false;
			});
			if (erased) {
				playersInPvP.erase(player);
			}
			return result;
		} catch (const std::exception &e) {
			plugin.getLogger().error(e.what());
			plugin.getServer().getScheduler().runTaskAsync(plugin, [e = e, dataFolder = dataFolder]() {
				log(e.what(), dataFolder);
			});
			return std::nullopt;
		}
	}
};