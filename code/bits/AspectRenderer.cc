#include "AspectRenderer.h"

#include <fmt/core.h>

#include "Akagoria.h"
#include "fmt/format.h"
#include "gf2/core/Color.h"
#include "gf2/core/TextData.h"

namespace akgr {

  AspectRenderer::AspectRenderer(Akagoria* game, const WorldResources& resources)
  : m_game(game)
  , m_atlas({ 512, 512 }, game->render_manager())
  , m_default_text_data(resources.aspect_text.data)
  , m_hp_entity(&m_atlas, resources.aspect_text, game->render_manager(), game->resource_manager())
  , m_mp_entity(&m_atlas, resources.aspect_text, game->render_manager(), game->resource_manager())
  , m_vp_entity(&m_atlas, resources.aspect_text, game->render_manager(), game->resource_manager())
  , m_background_shape(compute_shape_buffer(), game->render_manager())
  {
    m_background_shape.set_location({ 10.0f, 10.0f });
    m_hp_entity.set_location({ 30.0f, 20.0f });
    m_mp_entity.set_location({ 30.0f, 45.0f });
    m_vp_entity.set_location({ 30.0f, 70.0f });
  }

  void AspectRenderer::update([[maybe_unused]] gf::Time time)
  {
    const auto& aspects = m_game->world_state()->hero.aspects;

    m_background_shape.shape_group().update(compute_shape_buffer(), m_game->render_manager());

    gf::TextData hp_text = m_default_text_data;
    hp_text.content = fmt::format("HP: {}/{}", aspects.hp.value.as_int(), aspects.hp.max.as_int());
    m_hp_entity.text().update(hp_text, m_game->render_manager());

    gf::TextData mp_text = m_default_text_data;
    mp_text.content = fmt::format("MP: {}/{}", aspects.mp.value.as_int(), aspects.mp.max.as_int());
    m_mp_entity.text().update(mp_text, m_game->render_manager());

    gf::TextData vp_text = m_default_text_data;
    vp_text.content = fmt::format("VP: {}/{}", aspects.vp.value.as_int(), aspects.vp.max.as_int());
    m_vp_entity.text().update(vp_text, m_game->render_manager());
  }

  void AspectRenderer::render(gf::RenderRecorder& recorder)
  {
    m_background_shape.render(recorder);
    m_hp_entity.render(recorder);
    m_mp_entity.render(recorder);
    m_vp_entity.render(recorder);
  }

  gf::ShapeGroupBuffer AspectRenderer::compute_shape_buffer()
  {
    const auto& aspects = m_game->world_state()->hero.aspects;

    gf::ShapeGroupBuffer buffer;

    gf::ShapeBuffer frame = gf::ShapeBuffer::make_rectangle(gf::RectF::from_size({ 300.0f, 90.0f }));
    frame.color = gf::Gray * gf::opaque(0.8f);
    buffer.shapes.push_back(std::move(frame));

    // hp

    const float hp = 280.0f * aspects.hp.value.as_float() / aspects.hp.max.as_float();

    gf::ShapeBuffer hp_background = gf::ShapeBuffer::make_rectangle(gf::RectF::from_position_size({ 10.0f, 10.0f }, { 280.0f, 15.0f }));
    hp_background.color = gf::Transparent;
    hp_background.outline_color = aspect_color(Aspect::Health);
    hp_background.outline_thickness = 1.0f;
    buffer.shapes.push_back(std::move(hp_background));

    gf::ShapeBuffer hp_foreground = gf::ShapeBuffer::make_rectangle(gf::RectF::from_position_size({ 10.0f, 10.0f }, { hp, 15.0f }));
    hp_foreground.color = aspect_color(Aspect::Health);
    buffer.shapes.push_back(std::move(hp_foreground));

    // mp

    const float mp = 280.0f * aspects.mp.value.as_float() / aspects.mp.max.as_float();

    gf::ShapeBuffer mp_background = gf::ShapeBuffer::make_rectangle(gf::RectF::from_position_size({ 10.0f, 35.0f }, { 280.0f, 15.0f }));
    mp_background.color = gf::Transparent;
    mp_background.outline_color = aspect_color(Aspect::Magic);
    mp_background.outline_thickness = 1.0f;
    buffer.shapes.push_back(std::move(mp_background));

    gf::ShapeBuffer mp_foreground = gf::ShapeBuffer::make_rectangle(gf::RectF::from_position_size({ 10.0f, 35.0f }, { mp, 15.0f }));
    mp_foreground.color = aspect_color(Aspect::Magic);
    buffer.shapes.push_back(std::move(mp_foreground));

    // vp

    const float vp = 280.0f * aspects.vp.value.as_float() / aspects.vp.max.as_float();

    gf::ShapeBuffer vp_background = gf::ShapeBuffer::make_rectangle(gf::RectF::from_position_size({ 10.0f, 60.0f }, { 280.0f, 15.0f }));
    vp_background.color = gf::Transparent;
    vp_background.outline_color = aspect_color(Aspect::Vitality);
    vp_background.outline_thickness = 1.0f;
    buffer.shapes.push_back(std::move(vp_background));

    gf::ShapeBuffer vp_foreground = gf::ShapeBuffer::make_rectangle(gf::RectF::from_position_size({ 10.0f, 60.0f }, { vp, 15.0f }));
    vp_foreground.color = aspect_color(Aspect::Vitality);
    buffer.shapes.push_back(std::move(vp_foreground));

    return buffer;
  }

}
