#include "SlotSelectorRenderer.h"

#include <cassert>

#include "Akagoria.h"
#include "Colors.h"
#include "ui/Properties.h"

namespace akgr {
  /*
   * SlotSelectorRenderer
   */

  SlotSelectorRenderer::SlotSelectorRenderer(Akagoria* game, const KickoffResources& resources, gf::FontAtlas* atlas)
  : m_render_manager(game->render_manager())
  , m_slot_manager(game->slot_manager())
  , m_icon_arrow_text(atlas, resources.icon_arrow_text, game->render_manager(), game->resource_manager())
  , m_slot_default_text(resources.slot_default_text)
  , m_slot_texts{
    gf::Text(atlas, m_slot_default_text, game->render_manager(), game->resource_manager()),
    gf::Text(atlas, m_slot_default_text, game->render_manager(), game->resource_manager()),
    gf::Text(atlas, m_slot_default_text, game->render_manager(), game->resource_manager()),
    gf::Text(atlas, m_slot_default_text, game->render_manager(), game->resource_manager()),
    gf::Text(atlas, m_slot_default_text, game->render_manager(), game->resource_manager()),
  }
  , m_back_text(atlas, resources.back_text, game->render_manager(), game->resource_manager())
  , m_frame_widget(nullptr, &m_frame_theme, game->render_manager())
  {
    m_frame_theme.color = RpgBlue;

    m_inner_frame_theme.color = gf::darker(RpgBlue, 0.2);
    m_inner_frame_theme.outline_thickness = 1.0f;
    m_inner_frame_theme.outline_color = gf::White;
    m_inner_frame_theme.radius = 10.0f;
    m_inner_frame_theme.padding = 15.0f;
    m_inner_frame_theme.spacing = 10.0f;

    m_menu_theme.padding = 15.0f;
    m_menu_theme.spacing = 10.0f;

    m_menu_index.count = SlotCount + 1; // +1 for Back

    auto arrow = std::make_unique<ui::LabelWidget>(nullptr, &m_label_theme, &m_icon_arrow_text);

    auto* menu = m_frame_widget.add<ui::MenuWidget>(std::move(arrow), &m_menu_index, &m_menu_theme);

    for (auto& text : m_slot_texts) {
      auto* inner_frame = menu->add<ui::FrameWidget>(&m_inner_frame_theme, game->render_manager());
      inner_frame->add<ui::LabelWidget>(&m_label_theme, &text);
      inner_frame->set_positioning(ui::Positioning::Fill);
      inner_frame->set_size({ 400.0f, 100.0f });
      inner_frame->set_position( { 20.0f, 0.0f });
    }

    menu->add<ui::LabelWidget>(&m_label_theme, &m_back_text);

    synchronize_with_slots();

    m_frame_widget.compute_layout();
  }

  void SlotSelectorRenderer::synchronize_with_slots()
  {
    gf::TextData data = m_slot_default_text.data;

    for (auto&& [i, text] : gf::enumerate(m_slot_texts)) {
      data.content = m_slot_manager->slot_description(i);
      text.update(data, m_render_manager);
    }
  }

  void SlotSelectorRenderer::compute_next_choice()
  {
    m_menu_index.compute_next_choice();
    m_frame_widget.compute_layout();
  }

  void SlotSelectorRenderer::compute_prev_choice()
  {
    m_menu_index.compute_prev_choice();
    m_frame_widget.compute_layout();
  }

  SlotSelectorChoice SlotSelectorRenderer::choice() const
  {
    return m_menu_index.choice < SlotCount ? SlotSelectorChoice::Slot : SlotSelectorChoice::Back;
  }

  std::size_t SlotSelectorRenderer::selected_slot() const
  {
    assert(m_menu_index.choice < SlotCount);
    return m_menu_index.choice;
  }

  void SlotSelectorRenderer::update(gf::Time time)
  {
    if (!visible()) {
      return;
    }

    m_frame_widget.update(time);
  }

  void SlotSelectorRenderer::render(gf::RenderRecorder& recorder)
  {
    if (!visible()) {
      return;
    }

    m_frame_widget.render(recorder);
  }

  /*
   * SlotSelectorElement
   */

  SlotSelectorElement::SlotSelectorElement(Akagoria* game, SlotSelectorRenderer* entity)
  : m_game(game)
  , m_entity(entity)
  {
  }

  void SlotSelectorElement::on_down([[maybe_unused]] UiToolkit& toolkit)
  {
    m_entity->compute_next_choice();
  }

  void SlotSelectorElement::on_up([[maybe_unused]] UiToolkit& toolkit)
  {
    m_entity->compute_prev_choice();
  }

  void SlotSelectorElement::on_use(UiToolkit& toolkit)
  {
    using namespace gf::literals;

    switch (m_entity->choice()) {
      case SlotSelectorChoice::Slot:
        {
          auto index = m_entity->selected_slot();

          if (m_game->slot_manager()->slot(index).active) {
            m_game->load_world(AdventureChoice::Saved, index);
            m_game->replace_scene(&m_game->kickoff_act()->loading_scene);
          }
        }
        break;
      case SlotSelectorChoice::Back:
        toolkit.change_ui_element("start_menu"_id);
        break;
    }
  }

  void SlotSelectorElement::on_visibility_change(bool visible)
  {
    if (visible) {
      m_entity->synchronize_with_slots();
    }
  }

  UiEntity* SlotSelectorElement::entity()
  {
    return m_entity;
  }

}
