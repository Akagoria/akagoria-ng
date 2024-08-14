#include "KickoffBaseScene.h"

#include "Akagoria.h"

namespace akgr {

  constexpr float PaddingAroundLine = 10.0f;

  KickoffBaseScene::KickoffBaseScene(Akagoria* game, const KickoffResources& resources)
  : m_game(game)
  , m_atlas({ 1024, 1024 }, game->render_manager())
  , m_action_group(compute_settings())
  , m_title_sprite(resources.title_sprite, game->render_manager(), game->resource_manager())
  , m_main_title_text(&m_atlas, resources.main_title_text, game->render_manager(), game->resource_manager())
  , m_additional_title_text(&m_atlas, resources.additional_title_text, game->render_manager(), game->resource_manager())
  , m_subtitle_text(&m_atlas, resources.subtitle_text, game->render_manager(), game->resource_manager())
  , m_underline_shape(resources.underline_shape, game->render_manager(), game->resource_manager())
  {
    set_clear_color(gf::White);
    set_world_size({ 1000.0f, 1000.0f });
    set_world_center({ 500.0f, 500.0f });

    m_title_sprite.set_location({ 500.0f, 500.0f });
    m_title_sprite.set_origin({ 0.5f, 0.5f }); // center
    m_title_sprite.set_scale(500.0f / static_cast<float>(m_title_sprite.sprite().texture()->size().h));
    add_world_entity(&m_title_sprite);

    m_main_title_text.set_location({ 500.0f, 500.0f - PaddingAroundLine });
    m_main_title_text.set_origin({ 0.5f, 1.0f }); // bottom center
    add_world_entity(&m_main_title_text);

    const gf::RectF main_title_bounds = m_main_title_text.text().bounds();
    const gf::Vec2F additional_title_text_location = m_main_title_text.location() + gf::vec(main_title_bounds.extent.w / 2.0f, 2.0f * PaddingAroundLine);
    m_additional_title_text.set_location(additional_title_text_location);
    m_additional_title_text.set_origin({ 1.0f, 0.0f }); // top right
    add_world_entity(&m_additional_title_text);

    m_subtitle_text.set_location({ 500.0f, 700.0f });
    m_subtitle_text.set_origin({ 0.5f, 0.5f }); // center
    add_world_entity(&m_subtitle_text);

    m_underline_shape.set_location({ 500.0f, 500.0f });
    m_underline_shape.set_origin({ 0.5f, 0.5f }); // center
    add_world_entity(&m_underline_shape);
  }

  gf::ActionGroupSettings KickoffBaseScene::compute_settings()
  {
    using namespace gf::literals;
    gf::ActionGroupSettings settings;

    settings.actions.emplace("fullscreen"_id, gf::instantaneous_action().add_keycode_control(gf::Keycode::F));
    settings.actions.emplace("quit"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::Escape));

    return settings;
  }

  void KickoffBaseScene::do_process_event(const gf::Event& event)
  {
    m_action_group.process_event(event);
  }

  void KickoffBaseScene::do_handle_actions()
  {
    using namespace gf::literals;

    if (m_action_group.active("fullscreen"_id)) {
      m_game->window()->toggle_fullscreen();
    }

    if (m_action_group.active("quit"_id)) {
      m_game->window()->close();
    }

    m_action_group.reset();
  }

}
