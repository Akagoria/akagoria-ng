#include "InventoryRenderer.h"

#include "Akagoria.h"
#include "Colors.h"
#include "ui/Widgets.h"

namespace akgr {
  /*
   * InventoryRenderer
   */

  InventoryRenderer::InventoryRenderer(Akagoria* game, const WorldResources& resources, gf::FontAtlas* atlas)
  : m_game(game)
  , m_icon_left_arrow_text(atlas, resources.icon_left_arrow_text, game->render_manager(), game->resource_manager())
  , m_icon_right_arrow_text(atlas, resources.icon_right_arrow_text, game->render_manager(), game->resource_manager())
  , m_common_items_text(atlas, resources.common_items_text, game->render_manager(), game->resource_manager())
  , m_quest_items_text(atlas, resources.quest_items_text, game->render_manager(), game->resource_manager())
  , m_back_text(atlas, resources.back_text, game->render_manager(), game->resource_manager())
  , m_frame_widget(nullptr, &m_frame_theme, game->render_manager())
  {
    m_frame_theme.color = RpgBlue;

    m_choice_theme.padding = ui::Space(0.0f, 30.0f);

    auto left_arrow = std::make_unique<ui::LabelWidget>(nullptr, &m_label_theme, &m_icon_left_arrow_text);
    auto right_arrow = std::make_unique<ui::LabelWidget>(nullptr, &m_label_theme, &m_icon_right_arrow_text);

    auto* choice = m_frame_widget.add<ui::ChoiceWidget>(std::move(left_arrow), std::move(right_arrow), &m_choice_index, &m_choice_theme);
    choice->add<ui::LabelWidget>(&m_label_theme, &m_common_items_text);
    choice->add<ui::LabelWidget>(&m_label_theme, &m_quest_items_text);
    choice->add<ui::LabelWidget>(&m_label_theme, &m_back_text);

    m_choice_index.count = 3;

    m_frame_widget.set_position({ 200.0f, 200.0f });
    m_frame_widget.set_size({ 600.0f, 30.0f });
    m_frame_widget.set_positioning(ui::Positioning::Fill);

    m_frame_widget.compute_layout();
  }

  void InventoryRenderer::compute_next_choice()
  {
    m_choice_index.compute_next_choice();
    m_frame_widget.compute_layout();
  }

  void InventoryRenderer::compute_prev_choice()
  {
    m_choice_index.compute_prev_choice();
    m_frame_widget.compute_layout();
  }

  void InventoryRenderer::update(gf::Time time)
  {
    if (!visible()) {
      return;
    }

    m_frame_widget.update(time);
  }

  void InventoryRenderer::render(gf::RenderRecorder& recorder)
  {
    if (!visible()) {
      return;
    }

    m_frame_widget.render(recorder);
  }

  /*
   * InventoryElement
   */

  InventoryElement::InventoryElement(Akagoria* game, InventoryRenderer* entity)
  : m_game(game)
  , m_entity(entity)
  {
  }

  void InventoryElement::on_left([[maybe_unused]] UiToolkit& toolkit)
  {
    m_entity->compute_prev_choice();
  }

  void InventoryElement::on_right([[maybe_unused]] UiToolkit& toolkit)
  {
    m_entity->compute_next_choice();
  }

  UiEntity* InventoryElement::entity()
  {
    return m_entity;
  }

}
