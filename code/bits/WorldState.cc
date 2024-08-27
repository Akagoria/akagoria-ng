#include "WorldState.h"

#include <gf2/core/Streams.h>
#include <gf2/core/SerializationOps.h>

#include <gf2/physics/PhysicsShape.h>

#include "Version.h"
#include "WorldRuntime.h"
#include "WorldData.h"

namespace akgr {

  namespace {

    constexpr float HeroMass = 1.0f;
    constexpr float HeroRadius = 20.0f;

    constexpr float HeroVelocity = 200.0f;

  }

  void WorldState::load_from_file(const std::filesystem::path& filename)
  {
    gf::FileInputStream file(filename);
    gf::CompressedInputStream compressed(&file);
    gf::Deserializer ar(&compressed);

    ar | *this;
  }

  void WorldState::save_to_file(const std::filesystem::path& filename) const
  {
    gf::FileOutputStream file(filename);
    gf::CompressedOutputStream compressed(&file);
    gf::Serializer ar(&compressed, StateVersion);

    ar | *this;
  }

  namespace {



  }

  void WorldState::bind(const WorldData& data, WorldRuntime& runtime)
  {

    // hero

    {
      hero.physics.controller = gf::PhysicsBody::make_kinematic();
      runtime.physics.world.add_body(hero.physics.controller);

      hero.physics.body = gf::PhysicsBody::make_dynamic(HeroMass, gf::compute_moment_for_circle(HeroMass, 0.0f, HeroRadius, { 0.0f, 0.0f }));
      hero.physics.body.set_location(hero.physics.spot.location);
      hero.physics.body.set_angle(hero.physics.angle);
      runtime.physics.world.add_body(hero.physics.body);

      auto pivot = gf::PhysicsConstraint::make_pivot_joint(&hero.physics.controller, &hero.physics.body, { 0.0f, 0.0f }, { 0.0f, 0.0f });
      pivot.set_max_bias(0.0f);
      pivot.set_max_force(1'000.0f);
      runtime.physics.world.add_constraint(pivot);

      auto gear = gf::PhysicsConstraint::make_gear_joint(&hero.physics.controller, &hero.physics.body, 0.0f, 1.0f);
      gear.set_error_bias(0.0f);
      gear.set_max_bias(1.2f);
      gear.set_max_force(5'000.0f);
      runtime.physics.world.add_constraint(gear);

      auto shape = gf::PhysicsShape::make_circle(&hero.physics.body, HeroRadius, { 0.0f, 0.0f });
      runtime.physics.world.add_shape(shape);
    }

  }

}
