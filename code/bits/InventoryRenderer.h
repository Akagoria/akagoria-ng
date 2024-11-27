#ifndef AKGR_INVENTORY_RENDERER_H
#define AKGR_INVENTORY_RENDERER_H

#include <gf2/graphics/Text.h>

#include "UiToolkit.h"
#include "WorldResources.h"
#include "ui/Theme.h"
#include "ui/Widgets.h"

namespace akgr {
  class Akagoria;

  class InventoryRenderer : public UiEntity {
  public:
    InventoryRenderer(Akagoria* game, const WorldResources& resources, gf::FontAtlas* atlas);

    void compute_next_choice();
    void compute_prev_choice();

    void update(gf::Time time) override;
    void render(gf::RenderRecorder& recorder) override;

  private:
    Akagoria* m_game = nullptr;

    ui::IndexState m_choice_index;

    gf::Text m_icon_left_arrow_text;
    gf::Text m_icon_right_arrow_text;

    gf::Text m_common_items_text;
    gf::Text m_quest_items_text;
    gf::Text m_back_text;

    ui::FrameTheme m_frame_theme;
    ui::ChoiceTheme m_choice_theme;
    ui::LabelTheme m_label_theme;


    ui::FrameWidget m_frame_widget;
  };

  class InventoryElement : public UiElement {
  public:
    InventoryElement(Akagoria* game, InventoryRenderer* entity);

    void on_left(UiToolkit& toolkit) override;
    void on_right(UiToolkit& toolkit) override;

    UiEntity* entity() override;

  private:
    Akagoria* m_game = nullptr;
    InventoryRenderer* m_entity = nullptr;
  };

}

#endif // AKGR_INVENTORY_RENDERER_H
