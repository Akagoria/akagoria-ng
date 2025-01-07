#include "InventoryState.h"

#include <algorithm>

#include <gf2/core/Log.h>

namespace akgr {

  namespace {

    void add_item_to(const DataReference<ItemData>& data, std::vector<InventoryItemState>& items, int32_t count)
    {
      if (auto iterator = std::find_if(items.begin(), items.end(), [&data](const InventoryItemState& state) { return data.id == state.data.id; }); iterator != items.end()) {
        iterator->count += count;
      } else {
        items.push_back({ data, count });
      }
    }

  }

  void InventoryState::add_item(const DataReference<ItemData>& data, int32_t count)
  {
    add_item_to(data, items, count);
  }

  void InventoryState::add_quest_item(const DataReference<ItemData>& data, int32_t count)
  {
    add_item_to(data, quest_items, count);
  }

  void InventoryState::transfer_to_quest_items(const DataReference<ItemData>& data, int32_t count)
  {
    if (auto iterator = std::find_if(items.begin(), items.end(), [&data](const InventoryItemState& state) { return data.id == state.data.id; }); iterator != items.end()) {
      assert(iterator->count >= count);
      iterator->count -= count;
      add_quest_item(data, count);
    } else {
      gf::Log::warning("Item not found in inventory: '{}'", data.tag);
    }
  }

}
