#ifndef AKGR_START_MENU_RENDERER_H
#define AKGR_START_MENU_RENDERER_H

#include <cstdint>

#include <gf2/graphics/Entity.h>
#include <gf2/graphics/FontAtlas.h>
#include <gf2/graphics/Text.h>

#include "ui/State.h"
#include "ui/Theme.h"
#include "ui/Widgets.h"

#include "KickoffResources.h"

namespace akgr {
  class Akagoria;

  enum class StartMenuChoice : uint8_t {
    StartAdventure = 0,
    LoadAdventure = 1,
    Quit = 2,
  };

  class StartMenuRenderer : public gf::Entity {
  public:
    StartMenuRenderer(Akagoria* game, const KickoffResources& resources, gf::FontAtlas* atlas);

    void set_active(bool active) { m_active = active; }

    void compute_next_choice();
    void compute_prev_choice();
    StartMenuChoice choice() const;

    void update(gf::Time time) override;
    void render(gf::RenderRecorder& recorder) override;

  private:
    bool m_active = false;
    ui::IndexState m_menu_index;

    gf::Text m_icon_arrow_text;

    gf::Text m_start_text;
    gf::Text m_load_text;
    gf::Text m_quit_text;

    ui::FrameTheme m_frame_theme;
    ui::MenuTheme m_menu_theme;
    ui::LabelTheme m_label_theme;

    ui::FrameWidget m_frame_widget;
  };

}

#endif // AKGR_START_MENU_RENDERER_H
