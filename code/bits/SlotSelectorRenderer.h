#ifndef AKGR_SLOT_SELECTOR_RENDERER_H
#define AKGR_SLOT_SELECTOR_RENDERER_H

#include <gf2/graphics/Entity.h>

#include "KickoffResources.h"
#include "SlotManager.h"
#include "UiToolkit.h"
#include "ui/State.h"
#include "ui/Theme.h"
#include "ui/Widgets.h"

namespace akgr {
  class Akagoria;

  enum class SlotSelectorChoice : uint8_t {
    Slot,
    Back,
  };

  class SlotSelectorRenderer : public UiEntity {
  public:
    SlotSelectorRenderer(Akagoria* game, const KickoffResources& resources, gf::FontAtlas* atlas);

    void synchronize_with_slots();
    void compute_next_choice();
    void compute_prev_choice();
    SlotSelectorChoice choice() const;
    std::size_t selected_slot() const;

    void update(gf::Time time) override;
    void render(gf::RenderRecorder& recorder) override;

  private:
    gf::RenderManager* m_render_manager = nullptr;
    SlotManager* m_slot_manager = nullptr;

    ui::IndexState m_menu_index;

    gf::Text m_icon_arrow_text;

    gf::TextResource m_slot_default_text;

    std::array<gf::Text, SlotCount> m_slot_texts;
    gf::Text m_back_text;

    ui::FrameTheme m_frame_theme;
    ui::FrameTheme m_inner_frame_theme;
    ui::MenuTheme m_menu_theme;
    ui::LabelTheme m_label_theme;

    ui::FrameWidget m_frame_widget;
  };

  class SlotSelectorElement : public UiElement {
  public:
    SlotSelectorElement(Akagoria* game, SlotSelectorRenderer* entity);

    void on_down(UiToolkit& toolkit) override;
    void on_up(UiToolkit& toolkit) override;
    void on_use(UiToolkit& toolkit) override;
    void on_visibility_change(bool visible) override;

    UiEntity* entity() override;

  private:
    Akagoria* m_game = nullptr;
    SlotSelectorRenderer* m_entity = nullptr;
  };

}

#endif // AKGR_SLOT_SELECTOR_RENDERER_H
