#include "StartMenuRenderer.h"

#include "Akagoria.h"
#include "Colors.h"
#include "ui/Widgets.h"

namespace akgr {
  /*
   * StartMenuRenderer
   */

  StartMenuRenderer::StartMenuRenderer(Akagoria* game, const KickoffResources& resources, gf::FontAtlas* atlas)
  : m_icon_arrow_text(atlas, resources.icon_arrow_text, game->render_manager(), game->resource_manager())
  , m_start_text(atlas, resources.start_text, game->render_manager(), game->resource_manager())
  , m_load_text(atlas, resources.load_text, game->render_manager(), game->resource_manager())
  , m_quit_text(atlas, resources.quit_text, game->render_manager(), game->resource_manager())
  , m_frame_widget(nullptr, &m_frame_theme, game->render_manager())
  {
    m_frame_theme.color = RpgBlue;
    // m_frame_theme.margin = { 10.0f, 10.0f };
    // m_frame_theme.outline_color = gf::White;
    // m_frame_theme.outline_thickness = 2.0f;
    // m_frame_theme.radius = 5.0f;

    m_menu_theme.padding = 15.0f;
    m_menu_theme.spacing = 10.0f;

    m_menu_index.count = 3;

    auto arrow = std::make_unique<ui::LabelWidget>(nullptr, &m_label_theme, &m_icon_arrow_text);

    auto* menu = m_frame_widget.add<ui::MenuWidget>(arrow.release(), &m_menu_index, &m_menu_theme);
    menu->add<ui::LabelWidget>(&m_label_theme, &m_start_text);
    menu->add<ui::LabelWidget>(&m_label_theme, &m_load_text);
    menu->add<ui::LabelWidget>(&m_label_theme, &m_quit_text);

    m_frame_widget.compute_layout();
  }

  void StartMenuRenderer::compute_next_choice()
  {
    m_menu_index.compute_next_choice();
    m_frame_widget.compute_layout();
  }

  void StartMenuRenderer::compute_prev_choice()
  {
    m_menu_index.compute_prev_choice();
    m_frame_widget.compute_layout();
  }

  StartMenuChoice StartMenuRenderer::choice() const
  {
    return static_cast<StartMenuChoice>(m_menu_index.choice);
  }

  void StartMenuRenderer::update(gf::Time time)
  {
    if (!visible()) {
      return;
    }

    m_frame_widget.update(time);
  }

  void StartMenuRenderer::render(gf::RenderRecorder& recorder)
  {
    if (!visible()) {
      return;
    }

    m_frame_widget.render(recorder);
  }

  /*
   * StartMenuElement
   */

  StartMenuElement::StartMenuElement(Akagoria* game, StartMenuRenderer* entity)
  : m_game(game)
  , m_entity(entity)
  {
  }

  void StartMenuElement::on_down([[maybe_unused]] UiToolkit& toolkit)
  {
    m_entity->compute_next_choice();
  }

  void StartMenuElement::on_up([[maybe_unused]] UiToolkit& toolkit)
  {
    m_entity->compute_prev_choice();
  }

  void StartMenuElement::on_use(UiToolkit& toolkit)
  {
    using namespace gf::literals;

    switch (m_entity->choice()) {
      case StartMenuChoice::StartAdventure:
        m_game->load_world(AdventureChoice::New);
        m_game->replace_scene(&m_game->kickoff_act()->loading_scene);
        break;
      case StartMenuChoice::LoadAdventure:
        toolkit.change_ui_element("slot_selector"_id);
        break;
      case StartMenuChoice::Quit:
        m_game->pop_all_scenes();
        break;
    }
  }

  UiEntity* StartMenuElement::entity()
  {
    return m_entity;
  }

}
