#ifndef AKGR_ITEM_STATE_H
#define AKGR_ITEM_STATE_H

#include <gf2/core/TypeTraits.h>

#include "DataReference.h"
#include "ItemData.h"
#include "Spot.h"

namespace akgr {

  struct ItemState {
    DataReference<ItemData> data;
    Spot spot;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<ItemState, Archive>& state) {
    return ar | state.data | state.spot;
  }

}

#endif // AKGR_ITEM_STATE_H
