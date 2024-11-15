#include "InventoryState.h"

#include <algorithm>

namespace akgr {

  void InventoryState::add_item(DataReference<ItemData> data)
  {
    if (auto iterator = std::find_if(items.begin(), items.end(), [&data](const InventoryItemState& state) { return data.id == state.data.id; }); iterator != items.end()) {
      ++iterator->count;
    } else {
      items.push_back({ data, 1 });
    }
  }

}
