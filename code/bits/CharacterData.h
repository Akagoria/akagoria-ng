#ifndef AKGR_CHARACTER_DATA_H
#define AKGR_CHARACTER_DATA_H

#include <gf2/core/TypeTraits.h>

#include "DataLabel.h"

namespace akgr {

  struct CharacterData {
    DataLabel label;
    std::string name;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<CharacterData, Archive>& data) {
    return ar | data.label | data.name;
  }

}

#endif // AKGR_CHARACTER_DATA_H
