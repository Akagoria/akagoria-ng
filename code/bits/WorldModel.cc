#include "WorldModel.h"

#include <gf2/core/Math.h>
#include <gf2/core/Log.h>
#include "gf2/core/Move.h"

namespace akgr {

  namespace {

    constexpr float LinearVelocity = 200.0f;
    constexpr float AngularVelocity = 3.0f;

  }

  WorldModel::WorldModel(Akagoria* game)
  : runtime(game)
  {
  }

  void WorldModel::update(gf::Time time)
  {
    const float dt = time.as_seconds();

    // hero

    float rotation = state.hero.rotation;
    float velocity = 0.0f;

    if (state.hero.move.method == HeroMoveMethod::Relative) {
      switch (state.hero.move.angular) {
        case gf::AngularMove::None:
          break;
        case gf::AngularMove::Left:
          rotation -= AngularVelocity * dt;
          break;
        case gf::AngularMove::Right:
          rotation += AngularVelocity * dt;
          break;
      }

      switch (state.hero.move.linear) {
        case gf::LinearMove::None:
          break;
        case gf::LinearMove::Forward:
          velocity = LinearVelocity;
          break;
        case gf::LinearMove::Backward:
          velocity = - 0.5f * LinearVelocity;
          break;
      }

    } else {
      if (state.hero.move.linear == gf::LinearMove::Forward) {
        velocity = LinearVelocity;
      }
    }

    runtime.physics.hero.body.set_rotation(rotation);
    runtime.physics.hero.controller.set_rotation(rotation);
    runtime.physics.hero.controller.set_velocity(velocity * gf::unit(rotation));

    // physics

    runtime.physics.world.update(time);

    state.hero.rotation = runtime.physics.hero.body.rotation();
    state.hero.spot.location = runtime.physics.hero.body.location();

    runtime.script.handle_deferred_messages();

  }

}
