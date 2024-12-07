#ifndef AKGR_ITEM_STATE_H
#define AKGR_ITEM_STATE_H

#include <cstdint>

#include <gf2/core/Flags.h>
#include <gf2/core/TypeTraits.h>

#include "DataReference.h"
#include "ItemData.h"
#include "Spot.h"

namespace akgr {

  enum class ItemStatus : uint8_t {
    Picked = 0x01,
  };

  struct ItemState {
    DataReference<ItemData> data;
    Spot spot;
    float rotation = 0.0f;
    gf::Flags<ItemStatus> status = gf::None;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<ItemState, Archive>& state) {
    return ar | state.data | state.spot | state.rotation | state.status;
  }

}

#endif // AKGR_ITEM_STATE_H
