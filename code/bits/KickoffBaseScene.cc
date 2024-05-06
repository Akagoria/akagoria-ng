#include "KickoffBaseScene.h"

#include "Akagoria.h"

namespace akgr {

  KickoffBaseScene::KickoffBaseScene(Akagoria* game, const KickoffData& data)
  : m_game(game)
  , m_atlas({ 1024, 1024 }, game->render_manager())
  , m_title_sprite(data.title_sprite, game->render_manager(), game->resource_manager())
  , m_main_title_text(&m_atlas, data.main_title_text, game->render_manager(), game->resource_manager())
  , m_additional_title_text(&m_atlas, data.additional_title_text, game->render_manager(), game->resource_manager())
  , m_subtitle_text(&m_atlas, data.subtitle_text, game->render_manager(), game->resource_manager())
  , m_underline_shape(data.underline_shape, game->render_manager(), game->resource_manager())
  {
    set_clear_color(gf::White);
    set_world_size({ 1000.0f, 1000.0f });
    set_world_center({ 500.0f, 500.0f });

    m_title_sprite.set_location({ 500.0f, 500.0f });
    m_title_sprite.set_origin({ 0.5f, 0.5f });
    m_title_sprite.set_scale(500.0f / m_title_sprite.sprite().texture()->size().h);
    add_world_entity(&m_title_sprite);

    m_main_title_text.set_location({ 500.0f, 487.5f });
    m_main_title_text.set_origin({ 0.5f, 1.0f }); // bottom center
    add_world_entity(&m_main_title_text);

    const gf::RectF main_title_bounds = m_main_title_text.text().bounds();
    const gf::Vec2F additional_title_text_location = m_main_title_text.location() + gf::vec(main_title_bounds.extent.w / 2.0f, 25.0f);
    m_additional_title_text.set_location(additional_title_text_location);
    m_additional_title_text.set_origin({ 1.0f, 0.0f }); // top right
    add_world_entity(&m_additional_title_text);

    m_subtitle_text.set_location({ 500.0f, 650.0f });
    m_subtitle_text.set_origin({ 0.5f, 0.5f }); // center
    add_world_entity(&m_subtitle_text);

    m_underline_shape.set_location({ 500.0f, 500.0f });
    m_underline_shape.set_origin({ 0.5f, 0.5f });
    add_world_entity(&m_underline_shape);
  }

}
