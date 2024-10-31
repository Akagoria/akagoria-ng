#ifndef AKGR_QUEST_STATE_H
#define AKGR_QUEST_STATE_H

#include <variant>

#include "DataReference.h"
#include "QuestData.h"

namespace akgr {

  struct HuntQuestState {
    int32_t amount;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<HuntQuestState, Archive>& state) {
    return ar | state.amount;
  }

  struct TalkQuestState {

  };

  template<typename Archive>
  Archive& operator|(Archive& ar, [[maybe_unused]] gf::MaybeConst<TalkQuestState, Archive>& state) {
    return ar;
  }

  struct FarmQuestState {
    int32_t amount;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<FarmQuestState, Archive>& state) {
    return ar | state.amount;
  }

  struct ExploreQuestState {

  };

  template<typename Archive>
  Archive& operator|(Archive& ar, [[maybe_unused]] gf::MaybeConst<ExploreQuestState, Archive>& state) {
    return ar;
  }

  enum class QuestStatus : uint8_t {
    Started,
    Visible,
    Finished,
  };

  struct QuestState {
    DataReference<QuestData> data;
    QuestStatus status = QuestStatus::Started;
    int64_t last_update = 0;
    uint32_t current_step = 0;
    std::variant<std::monostate, HuntQuestState, TalkQuestState, FarmQuestState, ExploreQuestState> features;

    QuestType type() const { return static_cast<QuestType>(features.index()); }

    void reset_features();
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<QuestState, Archive>& state) {
    return ar | state.data | state.status | state.last_update | state.current_step | state.features;
  }

}

#endif // AKGR_QUEST_STATE_H
