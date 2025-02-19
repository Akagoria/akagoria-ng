#ifndef AKGR_ITEM_RENDERER_H
#define AKGR_ITEM_RENDERER_H

#include <gf2/graphics/SpriteGroup.h>

#include "FloorRenderer.h"

namespace akgr {

  class Akagoria;
  struct WorldResources;

  class ItemRenderer : public FloorRenderer {
  public:
    ItemRenderer(Akagoria* game, const WorldResources& resources);

    void update(gf::Time time, int32_t floor) override;
    void render_floor(gf::RenderRecorder& recorder, int32_t floor) override;

  private:
    Akagoria* m_game = nullptr;

    gf::SpriteGroupResource m_sprite_group_resource;
    gf::SpriteGroup m_sprite_group;
  };


}

#endif // AKGR_ITEM_RENDERER_H
