#ifndef AKGR_HERO_RUNTIME_H
#define AKGR_HERO_RUNTIME_H

#include <gf2/physics/PhysicsBody.h>

namespace akgr {

  struct HeroRuntime {
    gf::PhysicsBody controller;
    gf::PhysicsBody body;
  };

}

#endif // AKGR_HERO_RUNTIME_H
