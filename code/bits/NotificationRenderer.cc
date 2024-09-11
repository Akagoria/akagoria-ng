#include "NotificationRenderer.h"

#include <gf2/core/FontFace.h>

#include "Akagoria.h"

namespace akgr {

  namespace {

    gf::RichTextStyle compute_notification_style()
    {
      gf::RichTextStyle style;
      style.set_style("akgr", { 1.0f, gf::Gray, gf::FontStyle::Bold });
      return style;
    }

  }

  NotificationRenderer::NotificationRenderer(Akagoria* game, const WorldResources& resources)
  : m_game(game)
  , m_atlas({ 1024, 1024 }, game->render_manager())
  , m_style(compute_notification_style())
  , m_default_text_data(resources.notification_text.data)
  , m_text_entity(&m_atlas, &m_style, resources.notification_text, game->render_manager(), game->resource_manager())
  , m_shape(resources.notification_background, game->render_manager(), game->resource_manager())
  {
    static constexpr gf::Vec2F NotificationLocation = { 800.0f, 200.0f };
    static constexpr gf::Vec2F NotificationOrigin = { 0.5f, 0.5f };

    m_text_entity.set_location(NotificationLocation);
    m_text_entity.set_origin(NotificationOrigin);

    m_shape.set_location(NotificationLocation);
    m_shape.set_origin(NotificationOrigin);
  }

  void NotificationRenderer::update([[maybe_unused]] gf::Time time)
  {
    auto& notifications = m_game->world_state()->notifications;

    if (notifications.empty()) {
      return;
    }

    auto& current = notifications.front();

    if (current.data->label.id != m_last_notification) {
      gf::TextData data = m_default_text_data;
      data.content = current.data->message;
      m_text_entity.text().update(data, m_game->render_manager());
      m_last_notification = current.data->label.id;
    }
  }

  void NotificationRenderer::render(gf::RenderRecorder& recorder)
  {
    auto& notifications = m_game->world_state()->notifications;

    if (notifications.empty()) {
      return;
    }

    m_shape.render(recorder);
    m_text_entity.render(recorder);
  }

}
