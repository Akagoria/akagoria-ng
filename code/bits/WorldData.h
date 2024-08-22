#ifndef AKGR_WORLD_DATA_H
#define AKGR_WORLD_DATA_H

#include <filesystem>

#include <gf2/core/TypeTraits.h>
#include <gf2/core/TiledMap.h>

namespace akgr {

  struct WorldData {
    gf::TiledMap map;


    void load_from_file(const std::filesystem::path& filename);
    void save_to_file(const std::filesystem::path& filename) const;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<WorldData, Archive>& data)
  {
    return ar | data.map;
  }

}

#endif // AKGR_WORLD_DATA_H
