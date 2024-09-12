#ifndef AKGR_WORLD_DATA_H
#define AKGR_WORLD_DATA_H

#include <filesystem>

#include <gf2/core/TypeTraits.h>
#include <gf2/core/TiledMap.h>

#include "DataLexicon.h"
#include "DialogData.h"
#include "NotificationData.h"

namespace akgr {

  struct WorldData {
    gf::TiledMap map;

    DataLexicon<NotificationData> notifications;
    DataLexicon<DialogData> dialogs;

    void load_from_file(const std::filesystem::path& filename);
    void save_to_file(const std::filesystem::path& filename) const;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<WorldData, Archive>& data)
  {
    return ar | data.map | data.notifications | data.dialogs;
  }

}

#endif // AKGR_WORLD_DATA_H
