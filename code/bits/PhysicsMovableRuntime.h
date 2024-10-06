#ifndef AKGR_PHYSICS_MOVABLE_RUNTIME_H
#define AKGR_PHYSICS_MOVABLE_RUNTIME_H

#include <gf2/physics/PhysicsBody.h>
#include <gf2/physics/PhysicsShape.h>

#include "Spot.h"

namespace akgr {

  struct PhysicsMovableRuntime {
    gf::PhysicsBody controller;
    gf::PhysicsBody body;
    gf::PhysicsShape shape;

    void set_spot(Spot spot);
  };

}

#endif // AKGR_PHYSICS_MOVABLE_RUNTIME_H
