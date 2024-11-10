#ifndef AKGR_ITEM_STATE_H
#define AKGR_ITEM_STATE_H

#include <string>

#include <gf2/core/TypeTraits.h>

#include "DataReference.h"
#include "ItemData.h"
#include "Spot.h"

namespace akgr {

  struct ItemState {
    std::string name;
    DataReference<ItemData> data;
    Spot spot;
    float rotation = 0.0f;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<ItemState, Archive>& state) {
    return ar | state.name | state.data | state.spot | state.rotation;
  }

}

#endif // AKGR_ITEM_STATE_H
