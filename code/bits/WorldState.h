#ifndef AKGR_WORLD_STATE_H
#define AKGR_WORLD_STATE_H

#include <filesystem>

#include <gf2/core/TypeTraits.h>

namespace akgr {
  struct WorldData;

  struct WorldState {

    void load_from_file(const std::filesystem::path& filename);
    void save_to_file(const std::filesystem::path& filename) const;
    void bind(const WorldData& data);
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<WorldState, Archive>& state)
  {
    return ar;
  }

}

#endif // AKGR_WORLD_STATE_H
