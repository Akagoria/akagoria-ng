#ifndef AKGR_INVENTORY_STATE_H
#define AKGR_INVENTORY_STATE_H

#include <gf2/core/TypeTraits.h>

#include "DataReference.h"
#include "ItemData.h"

namespace akgr {

  struct InventoryItemState {
    DataReference<ItemData> data;
    int32_t count = 0;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<InventoryItemState, Archive>& state) {
    return ar | state.data | state.count;
  }

  struct InventoryState {
    std::vector<InventoryItemState> items;

    void add_item(DataReference<ItemData> data);
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<InventoryState, Archive>& state) {
    return ar | state.items;
  }

}

#endif // AKGR_INVENTORY_STATE_H
