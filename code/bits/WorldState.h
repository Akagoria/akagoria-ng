#ifndef AKGR_WORLD_STATE_H
#define AKGR_WORLD_STATE_H

#include <filesystem>

#include <gf2/core/TypeTraits.h>

#include "HeroState.h"

namespace akgr {
  struct WorldData;
  struct WorldRuntime;

  struct WorldState {
    HeroState hero;

    void load_from_file(const std::filesystem::path& filename);
    void save_to_file(const std::filesystem::path& filename) const;
    void bind(const WorldData& data);
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<WorldState, Archive>& state)
  {
    return ar | state.hero;
  }

}

#endif // AKGR_WORLD_STATE_H
