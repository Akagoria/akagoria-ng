#ifndef AKGR_WORLD_DEBUG_H
#define AKGR_WORLD_DEBUG_H

#include <gf2/graphics/Entity.h>

namespace akgr {
  class Akagoria;

  class WorldDebug : public gf::Entity {
  public:
    WorldDebug(Akagoria* game);

    void update(gf::Time time) override;

  private:
    void items();
    void notifications();

    Akagoria* m_game = nullptr;
  };

}

#endif // AKGR_WORLD_DEBUG_H
