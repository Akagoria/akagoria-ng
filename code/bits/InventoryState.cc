#include "InventoryState.h"

#include <algorithm>

#include <gf2/core/Log.h>

namespace akgr {

  /*
   * BasicInventoryState
   */

  void BasicInventoryState::add_item(const DataReference<ItemData>& data, int32_t count)
  {
    if (auto iterator = std::find_if(items.begin(), items.end(), [&data](const InventoryItemState& state) { return data.id == state.data.id; }); iterator != items.end()) {
      iterator->count += count;
    } else {
      items.push_back({ data, count });
    }
  }

  /*
   * InventoryState
   */

  void InventoryState::add_regular_item(const DataReference<ItemData>& data, int32_t count)
  {
    regular.add_item(data, count);
  }

  void InventoryState::add_quest_item(const DataReference<ItemData>& data, int32_t count)
  {
    quest.add_item(data, count);
  }

  void InventoryState::transfer_to_quest_items(const DataReference<ItemData>& data, int32_t count)
  {
    if (auto iterator = std::find_if(quest.items.begin(), quest.items.end(), [&data](const InventoryItemState& state) { return data.id == state.data.id; }); iterator != quest.items.end()) {
      assert(iterator->count >= count);
      iterator->count -= count;
      add_quest_item(data, count);
    } else {
      gf::Log::warning("Item not found in inventory: '{}'", data.tag);
    }
  }

}
