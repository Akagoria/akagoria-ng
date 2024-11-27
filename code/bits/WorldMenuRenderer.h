#ifndef AKGR_WORLD_MENU_RENDERER_H
#define AKGR_WORLD_MENU_RENDERER_H

#include <cstdint>

#include <gf2/graphics/FontAtlas.h>
#include <gf2/graphics/Text.h>

#include "UiToolkit.h"
#include "WorldResources.h"

#include "ui/State.h"
#include "ui/Theme.h"
#include "ui/Widgets.h"

namespace akgr {
  class Akagoria;

  enum class WorldMenuChoice : uint8_t {
    Inventory,
    BackToAdventure,
    BackToRealLife,
  };

  class WorldMenuRenderer : public UiEntity {
  public:
    WorldMenuRenderer(Akagoria* game, const WorldResources& resources, gf::FontAtlas* atlas);

    void compute_next_choice();
    void compute_prev_choice();
    WorldMenuChoice choice() const;

    void update(gf::Time time) override;
    void render(gf::RenderRecorder& recorder) override;

  private:
    ui::IndexState m_menu_index;

    gf::Text m_icon_arrow_text;

    gf::Text m_inventory_text;
    gf::Text m_back_to_adventure_text;
    gf::Text m_back_to_real_life_text;

    ui::FrameTheme m_frame_theme;
    ui::MenuTheme m_menu_theme;
    ui::LabelTheme m_label_theme;

    ui::FrameWidget m_frame_widget;
  };

  class WorldMenuElement : public UiElement {
  public:
    WorldMenuElement(Akagoria* game, WorldMenuRenderer* entity);

    void on_down(UiToolkit& toolkit) override;
    void on_up(UiToolkit& toolkit) override;
    void on_use(UiToolkit& toolkit) override;

    UiEntity* entity() override;

  private:
    Akagoria* m_game = nullptr;
    WorldMenuRenderer* m_entity = nullptr;
  };



}

#endif // AKGR_WORLD_MENU_RENDERER_H
