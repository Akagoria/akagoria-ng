#ifndef AKGR_WORLD_STATE_H
#define AKGR_WORLD_STATE_H

#include <filesystem>
#include <vector>

#include <gf2/core/TypeTraits.h>

#include "HeroState.h"
#include "NotificationState.h"

namespace akgr {
  struct WorldData;
  struct WorldRuntime;

  struct WorldState {
    HeroState hero;

    std::vector<NotificationState> notifications;

    void load_from_file(const std::filesystem::path& filename);
    void save_to_file(const std::filesystem::path& filename) const;
    void bind(const WorldData& data);
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<WorldState, Archive>& state)
  {
    return ar | state.hero | state.notifications;
  }

}

#endif // AKGR_WORLD_STATE_H
