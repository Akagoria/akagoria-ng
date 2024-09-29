#ifndef AKGR_WORLD_STATE_H
#define AKGR_WORLD_STATE_H

#include <filesystem>
#include <vector>

#include <gf2/core/TypeTraits.h>

#include "CharacterState.h"
#include "HeroState.h"
#include "ItemState.h"
#include "NotificationState.h"

namespace akgr {
  struct WorldData;
  struct WorldRuntime;

  struct WorldState {
    HeroState hero;

    std::vector<NotificationState> notifications;
    std::vector<ItemState> items;
    std::vector<CharacterState> characters;

    void load_from_file(const std::filesystem::path& filename);
    void save_to_file(const std::filesystem::path& filename) const;
    void bind(const WorldData& data);
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<WorldState, Archive>& state)
  {
    return ar | state.hero | state.notifications | state.items | state.characters;
  }

}

#endif // AKGR_WORLD_STATE_H
