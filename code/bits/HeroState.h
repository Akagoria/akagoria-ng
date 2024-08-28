#ifndef AKGR_HERO_STATE_H
#define AKGR_HERO_STATE_H

#include <gf2/core/TypeTraits.h>

#include "PhysicsState.h"

namespace akgr {

  struct HeroState {
    PhysicsState physics;


    gf::Vec2F location() const { return physics.spot.location; }
    float rotation() const { return physics.rotation; }
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<HeroState, Archive>& state)
  {
    return ar | state.physics;
  }

}

#endif // AKGR_HERO_STATE_H
