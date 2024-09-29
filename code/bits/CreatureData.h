#ifndef AKGR_CREATURE_DATA_H
#define AKGR_CREATURE_DATA_H

#include <gf2/core/TypeTraits.h>

#include "DataLabel.h"

namespace akgr {

  struct CreatureData {
    DataLabel label;
    std::string name;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<CreatureData, Archive>& data) {
    return ar | data.label | data.name;
  }

}

#endif // AKGR_CREATURE_DATA_H
