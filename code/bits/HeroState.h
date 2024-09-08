#ifndef AKGR_HERO_STATE_H
#define AKGR_HERO_STATE_H

#include <gf2/core/Move.h>
#include <gf2/core/TypeTraits.h>

#include "PhysicsState.h"

namespace akgr {

  enum class HeroMoveMethod {
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
    PhysicsState physics;


    gf::Vec2F location() const { return physics.spot.location; }
    float rotation() const { return physics.rotation; }
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<HeroState, Archive>& state)
  {
    return ar | state.move | state.physics;
  }

}

#endif // AKGR_HERO_STATE_H
