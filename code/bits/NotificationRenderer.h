#ifndef AKGR_NOTIFICATION_RENDERER_H
#define AKGR_NOTIFICATION_RENDERER_H

#include <gf2/core/Id.h>
#include <gf2/core/TextStyle.h>

#include <gf2/graphics/Entity.h>
#include <gf2/graphics/ShapeEntity.h>
#include <gf2/graphics/TextEntity.h>

#include "WorldResources.h"

namespace akgr {
  class Akagoria;

  class NotificationRenderer : public gf::Entity {
  public:
    NotificationRenderer(Akagoria* game, const WorldResources& resources);

    void update(gf::Time time) override;
    void render(gf::RenderRecorder& recorder) override;

  private:
    Akagoria* m_game = nullptr;
    gf::FontAtlas m_atlas;
    gf::RichTextStyle m_style;
    gf::TextData m_default_text_data;
    gf::RichTextEntity m_text_entity;
    gf::ShapeEntity m_background_shape;
    gf::Id m_last_notification = gf::InvalidId;
  };

}

#endif // AKGR_NOTIFICATION_RENDERER_H
