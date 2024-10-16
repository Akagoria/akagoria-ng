#ifndef AKGR_QUEST_DATA_H
#define AKGR_QUEST_DATA_H

#include <cstdint>

#include <string>
#include <variant>
#include <vector>

#include <gf2/core/Id.h>
#include <gf2/core/TypeTraits.h>

#include "DataLabel.h"
#include "DataReference.h"
#include "LocationData.h"

namespace akgr {
  struct CreatureData;
  struct DialogData;
  struct ItemData;

  enum class QuestScope : uint8_t {
    History,
    Long,
    Medium,
    Short,
  };

  enum class QuestType : uint8_t {
    None,
    Hunt,
    Talk,
    Farm,
    Explore,
  };

  struct HuntQuestData {
    DataReference<CreatureData> creature;
    int32_t count;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<HuntQuestData, Archive>& data) {
    return ar | data.creature | data.count;
  }

  struct TalkQuestData {
    DataReference<DialogData> dialog;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<TalkQuestData, Archive>& data) {
    return ar | data.dialog;
  }

  struct FarmQuestData {
    DataReference<ItemData> item;
    int32_t count;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<FarmQuestData, Archive>& data) {
    return ar | data.item | data.count;
  }

  struct ExploreQuestData {
    DataReference<LocationData> location;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<ExploreQuestData, Archive>& data) {
    return ar | data.location;
  }

  struct QuestStepData {
    std::string description;
    std::variant<std::monostate, HuntQuestData, TalkQuestData, FarmQuestData, ExploreQuestData> features;

    QuestType type() const { return static_cast<QuestType>(features.index()); }
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<QuestStepData, Archive>& data) {
    return ar | data.description | data.features;
  }

  struct QuestData {
    DataLabel label;
    std::string title;
    std::string description;
    QuestScope scope;
    std::vector<QuestStepData> steps;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<QuestData, Archive>& data) {
    return ar | data.label | data.title | data.description | data.scope | data.steps;
  }

}

#endif // AKGR_QUEST_DATA_H
