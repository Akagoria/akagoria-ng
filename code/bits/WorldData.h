#ifndef AKGR_WORLD_DATA_H
#define AKGR_WORLD_DATA_H

#include <filesystem>

#include <gf2/core/TypeTraits.h>
#include <gf2/core/TiledMap.h>

#include "AreaData.h"
#include "CharacterData.h"
#include "CreatureData.h"
#include "DataLexicon.h"
#include "DialogData.h"
#include "ItemData.h"
#include "LocationData.h"
#include "NotificationData.h"
#include "QuestData.h"

namespace akgr {

  struct WorldData {
    gf::TiledMap map;

    DataLexicon<NotificationData> notifications;
    DataLexicon<DialogData> dialogs;
    DataLexicon<ItemData> items;
    DataLexicon<CharacterData> characters;
    DataLexicon<CreatureData> creatures;
    DataLexicon<QuestData> quests;

    // from map (so no serialization)
    DataLexicon<AreaData> areas;
    DataLexicon<LocationData> locations;

    void load_from_file(const std::filesystem::path& filename);
    void save_to_file(const std::filesystem::path& filename) const;
    void bind();

  private:
    void bind_quests();
    void bind_map();
    void bind_map_object_layer(const gf::MapLayerStructure& layer, int32_t floor, const gf::TiledMap& map);
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<WorldData, Archive>& data)
  {
    return ar | data.map | data.notifications | data.dialogs | data.items | data.characters | data.quests;
  }

}

#endif // AKGR_WORLD_DATA_H
