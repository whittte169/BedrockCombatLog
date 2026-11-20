#pragma once

#include <endstone/endstone.hpp>
#include <exception>
#include "combat.hpp"

class Main : public endstone::Plugin {
private:
    std::unique_ptr<CombatManager> api;
public:
    void onEnable() {
        try {
            api = std::make_unique<CombatManager>(*this);
            getServer().getScheduler().runTaskTimer(*this, [this]() {
                for (endstone::Player *player : getServer().getOnlinePlayers()) {
                    endstone::UUID uuid = player->getUniqueId();
                }
            }, 0, 20);
            getLogger().info("BedrockCombatLog was successfuly loaded!");
        } catch (const std::exception &e) {
            getLogger().error(e.what());
        }
    }
};