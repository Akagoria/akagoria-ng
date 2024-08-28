#ifndef AKGR_PHYSICS_STATE_H
#define AKGR_PHYSICS_STATE_H

#include <gf2/core/TypeTraits.h>
#include <gf2/physics/PhysicsBody.h>
#include <gf2/physics/PhysicsConstraint.h>

#include "Spot.h"

namespace akgr {

  struct PhysicsState {
    Spot spot;
    float rotation = 0.0f;
    gf::PhysicsBody controller;
    gf::PhysicsBody body;
  };

  template<typename Archive>
  Archive& operator|(Archive& ar, gf::MaybeConst<PhysicsState, Archive>& state)
  {
    return ar | state.spot | state.rotation;
  }

}

#endif // AKGR_PHYSICS_STATE_H
