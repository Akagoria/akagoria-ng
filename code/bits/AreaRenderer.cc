#include "AreaRenderer.h"

#include <gf2/core/Log.h>

#include "Akagoria.h"

namespace akgr {

  namespace {

    constexpr gf::Time AreaUpdatePeriod = gf::seconds(1);

  }

  AreaRenderer::AreaRenderer(Akagoria* game, const WorldResources& resources)
  : m_game(game)
  , m_text_data(resources.area_text.data)
  , m_atlas({ 1024, 1024 }, game->render_manager())
  , m_text(&m_atlas, resources.area_text, game->render_manager(), game->resource_manager())
  {
    m_text.set_location({ 1230.0f, 250.0f });
  }

  void AreaRenderer::update(gf::Time time)
  {
    m_duration += time;

    if (!m_game->world_data()->areas.empty() && (m_current_area == nullptr || m_duration > AreaUpdatePeriod)) {
      m_duration -= AreaUpdatePeriod;
      const AreaData* current_area = m_game->world_data()->compute_closest_area(m_game->world_state()->hero.spot.location);

      if (current_area != m_current_area) {
        gf::TextData data = m_text_data;
        data.content = current_area->label.tag;
        m_text.text().update(data, m_game->render_manager());
        m_current_area = current_area;
      }
    }
  }

  void AreaRenderer::render(gf::RenderRecorder& recorder)
  {
    m_text.render(recorder);
  }


}
