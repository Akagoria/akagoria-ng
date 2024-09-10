#include "HeroRuntime.h"

#include "PhysicsUtils.h"

namespace akgr {

  void HeroRuntime::set_spot(Spot spot)
  {
    controller.set_location(spot.location);
    controller.set_velocity({ 0.0f, 0.0f });
    body.set_location(spot.location);
    body.set_velocity({ 0.0f, 0.0f });
    shape.set_shape_filter(filter_from_floor(spot.floor));
  }

}
