#include "WorldMenuRenderer.h"

#include "Akagoria.h"
#include "Colors.h"
#include "ui/Widgets.h"

namespace akgr {

  /*
   * WorldMenuRenderer
   */

  WorldMenuRenderer::WorldMenuRenderer(Akagoria* game, const WorldResources& resources, gf::FontAtlas* atlas)
  : m_icon_arrow_text(atlas, resources.icon_arrow_text, game->render_manager(), game->resource_manager())
  , m_inventory_text(atlas, resources.inventory_text, game->render_manager(), game->resource_manager())
  , m_back_to_adventure_text(atlas, resources.back_to_adventure_text, game->render_manager(), game->resource_manager())
  , m_back_to_real_life_text(atlas, resources.back_to_real_life_text, game->render_manager(), game->resource_manager())
  , m_frame_widget(nullptr, &m_frame_theme, game->render_manager())
  {
    m_frame_theme.color = RpgBlue;

    m_menu_theme.padding = 15.0f;
    m_menu_theme.spacing = 10.0f;

    m_menu_index.count = 3;

    auto arrow = std::make_unique<ui::LabelWidget>(nullptr, &m_label_theme, &m_icon_arrow_text);

    auto* menu = m_frame_widget.add<ui::MenuWidget>(std::move(arrow), &m_menu_index, &m_menu_theme);
    menu->add<ui::LabelWidget>(&m_label_theme, &m_inventory_text);
    menu->add<ui::LabelWidget>(&m_label_theme, &m_back_to_adventure_text);
    menu->add<ui::LabelWidget>(&m_label_theme, &m_back_to_real_life_text);

    m_frame_widget.compute_layout();
  }

  void WorldMenuRenderer::compute_next_choice()
  {
    m_menu_index.compute_next_choice();
    m_frame_widget.compute_layout();
  }

  void WorldMenuRenderer::compute_prev_choice()
  {
    m_menu_index.compute_prev_choice();
    m_frame_widget.compute_layout();
  }

  WorldMenuChoice WorldMenuRenderer::choice() const
  {
    return static_cast<WorldMenuChoice>(m_menu_index.choice);
  }

  void WorldMenuRenderer::update(gf::Time time)
  {
    if (!visible()) {
      return;
    }

    m_frame_widget.update(time);
  }

  void WorldMenuRenderer::render(gf::RenderRecorder& recorder)
  {
    if (!visible()) {
      return;
    }

    m_frame_widget.render(recorder);
  }

  /*
   * WorldMenuElement
   */

  WorldMenuElement::WorldMenuElement(Akagoria* game, WorldMenuRenderer* entity)
  : m_game(game)
  , m_entity(entity)
  {
  }

  void WorldMenuElement::on_down([[maybe_unused]] UiToolkit& toolkit)
  {
    m_entity->compute_next_choice();
  }

  void WorldMenuElement::on_up([[maybe_unused]] UiToolkit& toolkit)
  {
    m_entity->compute_prev_choice();
  }

  void WorldMenuElement::on_use(UiToolkit& toolkit)
  {
    using namespace gf::literals;

    switch (m_entity->choice()) {
      case WorldMenuChoice::Inventory:
        toolkit.change_ui_element("inventory"_id);
        break;
      case WorldMenuChoice::BackToAdventure:
        m_game->replace_scene(&m_game->world_act()->travel_scene);
        break;
      case WorldMenuChoice::BackToRealLife:
        m_game->pop_all_scenes();
        break;
    }
  }

  UiEntity* WorldMenuElement::entity()
  {
    return m_entity;
  }

}
