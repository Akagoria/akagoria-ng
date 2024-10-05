#ifndef AKGR_CHARACTER_DATA_H
#define AKGR_CHARACTER_DATA_H

#include <filesystem>

#include <gf2/core/TypeTraits.h>

#include "DataLabel.h"

namespace akgr {

  struct CharacterData {
    DataLabel label;
    std::filesystem::path animations;
  };

  inline
  std::filesystem::path character_animation_path(const CharacterData& data)
  {
    std::filesystem::path path = data.animations;
    path += ".png";
    return "sprites" / path;
  }

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<CharacterData, Archive>& data) {
    return ar | data.label | data.animations;
  }

}

#endif // AKGR_CHARACTER_DATA_H
