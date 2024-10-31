#ifndef AKGR_HERO_STATE_H
#define AKGR_HERO_STATE_H

#include <set>

#include <gf2/core/Id.h>
#include <gf2/core/Move.h>
#include <gf2/core/TypeTraits.h>

#include "AspectState.h"
#include "AttributeState.h"
#include "DialogState.h"
#include "QuestState.h"
#include "Spot.h"

namespace akgr {

  enum class HeroMoveMethod : uint8_t {
    Relative,
    Absolute,
  };

  struct HeroMove {
    HeroMoveMethod method = HeroMoveMethod::Relative;
    gf::LinearMove linear = gf::LinearMove::None;
    gf::AngularMove angular = gf::AngularMove::None;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<HeroMove, Archive>& move) {
    return ar | move.linear | move.angular;
  }

  struct HeroState {
    HeroMove move;
    Spot spot;
    float rotation = 0.0f;

    AspectState aspects;
    AttributeState attributes;

    std::set<gf::Id> requirements;

    DialogState dialog;
    std::vector<QuestState> quests;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<HeroState, Archive>& state)
  {
    return ar | state.move | state.spot | state.rotation | state.aspects | state.attributes | state.requirements | state.dialog | state.quests;
  }

}

#endif // AKGR_HERO_STATE_H
