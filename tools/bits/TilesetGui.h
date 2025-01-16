#ifndef AKGR_TILESET_GUI_H
#define AKGR_TILESET_GUI_H

#include <filesystem>

#include <gf2/core/Random.h>
#include <gf2/graphics/Entity.h>
#include <gf2/graphics/Texture.h>

#include "TilesetData.h"

namespace akgr {

  class TilesetGui : public gf::HudEntity {
  public:
    TilesetGui(std::filesystem::path datafile, gf::RenderManager* render_manager);

    void update(gf::Time time) override;

  private:
    gf::RenderManager* m_render_manager = nullptr;

    std::filesystem::path m_datafile;
    TilesetData m_data;
    gf::Random m_random;

    bool m_modified = false;

    // for settings
    gf::Vec2I m_size;

    // edit for atoms
    Atom m_edited_atom;
    static constexpr std::size_t NameBufferSize = 256;
    char m_name_buffer[NameBufferSize] = { '\0' };
    int m_pigment_choice = 0;
    gf::Texture m_pigment_preview;
    bool m_new_atom = false;

    // edit for wang2
    Wang2 m_edited_wang2;
    int m_border_effect_choices[2] = { 0, 0 };
    gf::Texture m_wang2_preview;
    bool m_new_wang2 = false;

    // edit for wang3
    Wang3 m_edited_wang3;
    gf::Id m_ids_choice[3] = { gf::id(0), gf::id(0), gf::id(0) };
    gf::Texture m_wang3_preview;
    bool m_new_wang3 = false;
  };

}

#endif // AKGR_TILESET_GUI_H
