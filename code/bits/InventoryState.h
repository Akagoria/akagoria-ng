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

  struct BasicInventoryState {
    std::vector<InventoryItemState> items;

    void add_item(const DataReference<ItemData>& data, int32_t count = 1);
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<BasicInventoryState, Archive>& state) {
    return ar | state.items;
  }

  struct InventoryState {
    BasicInventoryState regular;
    BasicInventoryState quest;

    void add_regular_item(const DataReference<ItemData>& data, int32_t count = 1);
    void add_quest_item(const DataReference<ItemData>& data, int32_t count = 1);
    void transfer_to_quest_items(const DataReference<ItemData>& data, int32_t count = 1);
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<InventoryState, Archive>& state) {
    return ar | state.regular | state.quest;
  }

}

#endif // AKGR_INVENTORY_STATE_H
