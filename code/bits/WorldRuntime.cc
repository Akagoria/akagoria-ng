#include "WorldRuntime.h"

#include <gf2/core/Log.h>

#include "Akagoria.h"

namespace akgr {

  WorldRuntime::WorldRuntime(Akagoria* game)
  : script(game)
  {
    physics.zone_handler.on_message.connect([this, game](const std::string& message, const std::set<gf::Id>& requirements) {
      auto& hero = game->world_state()->hero;

      if (std::includes(hero.requirements.begin(), hero.requirements.end(), requirements.begin(), requirements.end())) {
        script.on_message_deferred(message);
      }
    });
  }

  void WorldRuntime::bind(const WorldData& data, const WorldState& state)
  {
    script.bind();
    physics.bind(data, state);
  }

}
