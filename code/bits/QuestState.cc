#include "QuestState.h"

#include <cassert>

namespace akgr {

  void QuestState::reset_features()
  {
    assert(current_step < data->steps.size());
    const QuestStepData& step = data->steps[current_step];

    switch (step.type()) {
      case QuestType::None:
        {
          features = {};
        }
        break;
      case QuestType::Hunt:
        {
          HuntQuestState state = {};
          state.amount = 0;
          features = state;
        }
        break;
      case QuestType::Talk:
        {
          TalkQuestState state = {};
          features = state;
        }
        break;
      case QuestType::Farm:
        {
          FarmQuestState state = {};
          state.amount = 0;
          features = state;
        }
        break;
      case QuestType::Explore:
        {
          ExploreQuestState state = {};
          features = state;
        }
        break;
    }

  }

}
