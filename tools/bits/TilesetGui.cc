#include "TilesetGui.h"

#include <cassert>
#include <algorithm>
#include <fstream>

#include <imgui.h>

#include <gf2/core/Log.h>
#include <gf2/graphics/RenderManager.h>

#include "TilesetProcess.h"

namespace akgr {

  namespace {

    constexpr int BottomMargin = 200; // TODO: find something better?
    constexpr float EmptySize = 38.0f;

    constexpr const char* PigmentStyleList[] = { "Plain", "Randomize", "Striped", "Paved" }; // see PigmentStyle
    constexpr const char* BorderEffectList[] = { "None", "Fade", "Outline", "Sharpen", "Lighten", "Blur", "Blend" }; // see BorderEffect


    bool atom_combo(const TilesetData& data, const char *label, gf::Id *current, std::initializer_list<gf::Id> forbidden) {
      auto current_atom = data.get_atom(*current);

      bool res = ImGui::BeginCombo(label, current_atom.id.name.c_str());

      if (res) {
        res = false;

        for (const auto& atom : data.atoms) {
          ImGuiSelectableFlags flags = ImGuiSelectableFlags_None;

          if (std::find(forbidden.begin(), forbidden.end(), atom.id.hash) != forbidden.end()) {
            flags |= ImGuiSelectableFlags_Disabled;
          };

          ImGui::ColorButton("##Color", ImVec4(atom.color.r, atom.color.g, atom.color.b, atom.color.a));
          ImGui::SameLine();

          if (ImGui::Selectable(atom.id.name.c_str(), atom.id.hash == *current, flags)) {
            *current = atom.id.hash;
            res = true;
          }
        }

        ImGui::EndCombo();
      }

      ImGui::SameLine();
      ImGui::ColorButton("##Color", ImVec4(current_atom.color.r, current_atom.color.g, current_atom.color.b, current_atom.color.a));

      return res;
    }
  }

  TilesetGui::TilesetGui(std::filesystem::path datafile, gf::RenderManager* render_manager)
  : m_render_manager(render_manager)
  , m_datafile(std::move(datafile))
  , m_data(TilesetData::load(m_datafile))
  , m_size(m_data.settings.image_size())
  , m_pigment_preview(generate_empty_atom(m_data.settings.tile), m_render_manager)
  , m_wang2_preview(generate_empty_wang2(m_data.settings.tile), m_render_manager)
  , m_wang3_preview(generate_empty_wang3(m_data.settings.tile), m_render_manager)
  {
  }

  void TilesetGui::update([[maybe_unused]] gf::Time time)
  {
    auto size = m_render_manager->surface_size();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(float(size.w - 10), float(size.h - 10)));

    if (ImGui::Begin("Tileset", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings)) {

      if (ImGui::BeginTabBar("##Tabs")) {
        if (ImGui::BeginTabItem("Settings")) {

          if (ImGui::Checkbox("Locked", &m_data.settings.locked)) {
            m_modified = true;
          }

          ImGui::Separator();

          static constexpr int InputSlowStep = 16;
          static constexpr int InputFastStep = 64;

          if (ImGui::InputInt("TileSize", &m_data.settings.tile.size, InputSlowStep, InputFastStep)) {
            m_size = m_data.settings.image_size();
            m_modified = true;
          }

          if (ImGui::InputInt("TileSpacing", &m_data.settings.tile.spacing, 1, 2)) {
            m_size = m_data.settings.image_size();
            m_modified = true;
          }

          if (!m_data.settings.locked) {
            ImGui::Separator();

            if (ImGui::InputInt("Max Atom Count", &m_data.settings.max_atom_count, InputSlowStep, InputFastStep)) {
              m_size = m_data.settings.image_size();
              m_modified = true;
            }

            if (ImGui::InputInt("Max Wang2 Count", &m_data.settings.max_wang2_count, InputSlowStep, InputFastStep)) {
              m_size = m_data.settings.image_size();
              m_modified = true;
            }

            if (ImGui::InputInt("Max Wang3 Count", &m_data.settings.max_wang3_count, InputSlowStep, InputFastStep)) {
              m_size = m_data.settings.image_size();
              m_modified = true;
            }
          }

          ImGui::Text("Image size: %ix%i", m_size.w, m_size.h);

          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Atoms")) {
          ImGui::Text("Atom count: %zu/%i", m_data.atoms.size(), m_data.settings.max_atom_count);
          ImGui::Spacing();

          if (ImGui::BeginChild("##AtomChild", ImVec2(0.0f, float(size.h - BottomMargin)))) {

            if (ImGui::BeginTable("##AtomTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
              ImGui::TableSetupColumn("Atom");
              ImGui::TableSetupColumn("Operations", ImGuiTableColumnFlags_WidthFixed);


              ImGui::TableHeadersRow();

              std::size_t index = 0;
              gf::Id atom_to_delete = gf::InvalidId;

              for (auto& atom : m_data.atoms) {
                ImGui::TableNextColumn();

                ImGui::PushID(int(index));
                ImGui::ColorButton("##Color", ImVec4(atom.color.r, atom.color.g, atom.color.b, atom.color.a));
                ImGui::SameLine();
                ImGui::Text("%s", atom.id.name.c_str());

                ImGui::TableNextColumn();

                if (!m_data.settings.locked && index + 1 < m_data.atoms.size()) {
                  if (ImGui::ArrowButton("Down", ImGuiDir_Down)) {
                    std::swap(m_data.atoms[index], m_data.atoms[index + 1]);
                    m_modified = true;
                  }
                } else {
                  ImGui::Dummy(ImVec2(EmptySize, EmptySize));
                }

                ImGui::SameLine();

                if (!m_data.settings.locked && index > 0) {
                  if (ImGui::ArrowButton("Up", ImGuiDir_Up)) {
                    std::swap(m_data.atoms[index], m_data.atoms[index - 1]);
                    m_modified = true;
                  }
                } else {
                  ImGui::Dummy(ImVec2(EmptySize, EmptySize));
                }

                ImGui::SameLine();

                auto prepare_edited_atom = [&]() {
                  m_edited_atom = atom;
                  assert(m_edited_atom.id.name.size() < NameBufferSize - 1);
                  std::copy(m_edited_atom.id.name.begin(), m_edited_atom.id.name.end(), std::begin(m_name_buffer));
                  m_name_buffer[m_edited_atom.id.name.size()] = '\0';
                  m_pigment_choice = static_cast<int>(m_edited_atom.pigment.style);
                };

                auto generate_preview = [this]() {
                  m_data.temporary.atom = m_edited_atom;
                  m_pigment_preview.update(generate_atom_preview(m_edited_atom, m_random, m_data.settings.tile), m_render_manager);
                  m_data.temporary.atom = Atom();
                };

                if (ImGui::Button("Edit")) {
                  ImGui::OpenPopup("Edit");
                  prepare_edited_atom();
                  generate_preview();
                }

                if (m_new_atom && index + 1 == m_data.atoms.size()) {
                  ImGui::SetScrollHereY(1.0f);
                  ImGui::OpenPopup("Edit");
                  prepare_edited_atom();
                  generate_preview();
                  m_new_atom = false;
                }

                if (ImGui::BeginPopupModal("Edit", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                  bool changed = false;

                  ImGui::InputText("Name##Atom", std::data(m_name_buffer), NameBufferSize);
                  changed = ImGui::ColorEdit4("Color##Atom", &m_edited_atom.color.r) || changed;

                  if (ImGui::Combo("Pigment##PigmentStyle", &m_pigment_choice, std::data(PigmentStyleList), std::size(PigmentStyleList))) {
                    m_edited_atom.pigment.style = static_cast<PigmentStyle>(m_pigment_choice);

                    switch (m_edited_atom.pigment.style) {
                      case PigmentStyle::Plain:
                        break;
                      case PigmentStyle::Randomize:
                        m_edited_atom.pigment.randomize.ratio = 0.1f;
                        m_edited_atom.pigment.randomize.deviation = 0.1f;
                        m_edited_atom.pigment.randomize.size = 1;
                        break;
                      case PigmentStyle::Striped:
                        m_edited_atom.pigment.striped.width = 3;
                        m_edited_atom.pigment.striped.stride = 8;
                        break;
                      case PigmentStyle::Paved:
                        m_edited_atom.pigment.paved.width = 8;
                        m_edited_atom.pigment.paved.length = 16;
                        m_edited_atom.pigment.paved.modulation = 0.5f;
                        break;
                    }

                    changed = true;
                  }

                  ImGui::Indent();

                  switch (m_edited_atom.pigment.style) {
                    case PigmentStyle::Plain:
                      break;
                    case PigmentStyle::Randomize:
                      changed = ImGui::SliderFloat("Ratio##AtomRandomize", &m_edited_atom.pigment.randomize.ratio, 0.0f, 1.0f, "%.2f") || changed;
                      changed = ImGui::SliderFloat("Deviation##AtomRandomize", &m_edited_atom.pigment.randomize.deviation, 0.0f, 0.5f, "%.2f") || changed;
                      changed = ImGui::SliderInt("Size##AtomRandomize", &m_edited_atom.pigment.randomize.size, 1, 5) || changed;
                      break;
                    case PigmentStyle::Striped:
                      changed = ImGui::SliderInt("Width##AtomStriped", &m_edited_atom.pigment.striped.width, 1, 8) || changed;
                      changed = ImGui::SliderInt("Stride##AtomStriped", &m_edited_atom.pigment.striped.stride, 1, 16) || changed;
                      break;
                    case PigmentStyle::Paved:
                      changed = ImGui::SliderInt("Width##AtomPaved", &m_edited_atom.pigment.paved.width, 4, 16) || changed;
                      changed = ImGui::SliderInt("Length##AtomPaved", &m_edited_atom.pigment.paved.length, 4, 32) || changed;
                      changed = ImGui::SliderFloat("Modulation##AtomPaved", &m_edited_atom.pigment.paved.modulation, -0.8f, 0.8f, "%.2f") || changed;
                      break;
                  }

                  ImGui::Unindent();

                  ImGui::Spacing();

                  static constexpr float PreviewSize = 128.0f;

                  if (changed) {
                    generate_preview();
                  }

                  ImGui::SetCursorPosX((ImGui::GetWindowWidth() - PreviewSize) / 2);
                  ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(&m_pigment_preview)), ImVec2(PreviewSize, PreviewSize));

                  ImGui::Spacing();


                  if (ImGui::Button("Save")) {
                    m_edited_atom.id.name = std::data(m_name_buffer);
                    m_edited_atom.id.hash = gf::hash_string(m_edited_atom.id.name);
                    m_data.update_atom(atom, m_edited_atom);
                    ImGui::CloseCurrentPopup();
                    m_modified = true;
                  }

                  ImGui::SameLine();

                  if (ImGui::Button("Cancel")) {
                    ImGui::CloseCurrentPopup();
                  }

                  ImGui::SameLine();

                  if (ImGui::Button("Preview")) {
                    generate_preview();
                  }

                  ImGui::EndPopup();
                }

                ImGui::SameLine();

                if (m_data.settings.locked) {
                  ImGui::TextDisabled("Delete");
                } else {
                  if (ImGui::Button("Delete")) {
                    ImGui::OpenPopup("Delete");
                  }
                }

                if (ImGui::BeginPopupModal("Delete", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                  ImGui::Text("Are you sure you want to delete '%s'?", m_data.atoms[index].id.name.c_str());

                  if (ImGui::Button("No, do not delete!")) {
                    ImGui::CloseCurrentPopup();
                  }

                  ImGui::SameLine();

                  if (ImGui::Button("Yes, I want to delete")) {
                    m_data.atoms.erase(std::next(m_data.atoms.begin(), std::ptrdiff_t(index)));
                    ImGui::CloseCurrentPopup();
                    m_modified = true;
                  }

                  ImGui::EndPopup();
                }

                ImGui::PopID();
                ++index;
              }

              ImGui::EndTable();

              if (atom_to_delete != gf::InvalidId) {
                m_data.delete_atom(atom_to_delete);
              }
            }

          }

          ImGui::EndChild();

          if (ImGui::Button("New")) {
            Atom atom;
            atom.id.name = "NewAtom"; atom.id.hash = gf::hash_string(atom.id.name);
            atom.color = gf::White;
            atom.pigment.style = PigmentStyle::Plain;
            m_data.atoms.emplace_back(std::move(atom));
            m_new_atom = true;
            m_modified = true;
          }


          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Wang2")) {
          ImGui::Text("Wang2 count: %zu/%i", m_data.wang2.size(), m_data.settings.max_wang2_count);
          ImGui::Spacing();

          if (ImGui::BeginChild("##Wang2", ImVec2(0, float(size.h - BottomMargin)))) {

            if (ImGui::BeginTable("##AtomTable", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
              ImGui::TableSetupColumn("Atom #1");
              ImGui::TableSetupColumn("Atom #2");
              ImGui::TableSetupColumn("Operations", ImGuiTableColumnFlags_WidthFixed);

              ImGui::TableHeadersRow();

              std::size_t index = 0;

              for (auto& wang : m_data.wang2) {
                ImGui::TableNextColumn();

                ImGui::PushID(int(index));

                for (const auto& border : wang.borders) {
                  if (border.id.hash == Void) {
                    ImGui::ColorButton("##Color", ImVec4(0, 0, 0, 0), ImGuiColorEditFlags_AlphaPreview);
                    ImGui::SameLine();
                    ImGui::Text("-");
                    ImGui::TableNextColumn();
                  } else {
                    auto atom = m_data.get_atom(border.id.hash);
                    ImGui::ColorButton("##Color", ImVec4(atom.color.r, atom.color.g, atom.color.b, atom.color.a));
                    ImGui::SameLine();
                    ImGui::Text("%s", atom.id.name.c_str());
                    ImGui::TableNextColumn();
                  }
                }

                if (!m_data.settings.locked && index + 1 < m_data.wang2.size()) {
                  if (ImGui::ArrowButton("Down", ImGuiDir_Down)) {
                    std::swap(m_data.wang2[index], m_data.wang2[index + 1]);
                    m_modified = true;
                  }
                } else {
                  ImGui::Dummy(ImVec2(EmptySize, EmptySize));
                }

                ImGui::SameLine();

                if (!m_data.settings.locked && index > 0) {
                  if (ImGui::ArrowButton("Up", ImGuiDir_Up)) {
                    std::swap(m_data.wang2[index], m_data.wang2[index - 1]);
                    m_modified = true;
                  }
                } else {
                  ImGui::Dummy(ImVec2(EmptySize, EmptySize));
                }

                ImGui::SameLine();

                auto prepare_edited_wang2 = [&]() {
                  m_edited_wang2 = wang;
                  m_border_effect_choices[0] = static_cast<int>(m_edited_wang2.borders[0].effect);
                  m_border_effect_choices[1] = static_cast<int>(m_edited_wang2.borders[1].effect);
                };

                auto generate_preview = [this]() {
                  m_data.temporary.wang2 = m_edited_wang2;
                  m_wang2_preview.update(generate_wang2_preview(m_edited_wang2, m_random, m_data), m_render_manager);
                  m_data.temporary.wang2 = Wang2();
                };

                if (ImGui::Button("Edit")) {
                  ImGui::OpenPopup("Edit");
                  prepare_edited_wang2();
                  generate_preview();
                }

                if (m_new_wang2 && index + 1 == m_data.wang2.size()) {
                  ImGui::SetScrollHereY(1.0f);
                  ImGui::OpenPopup("Edit");
                  prepare_edited_wang2();
                  generate_preview();
                  m_new_wang2 = false;
                }

                if (ImGui::BeginPopupModal("Edit", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                  bool changed = false;
                  int j = 0;

                  for (auto& border : m_edited_wang2.borders) {
                    ImGui::PushID(j);

                    ImGui::Text("Atom #%i\n", j+1);

                    if (j == 1) {

                      if (ImGui::RadioButton("Overlay##Wang2", border.id.hash == Void)) {
                        changed = true;

                        if (border.id.hash == Void) {
                          border.id = m_data.atoms.front().id;
                          border.effect = BorderEffect::None;
                        } else {
                          border.id.name = "Void";
                          border.id.hash = Void;
                          border.effect = BorderEffect::None;
                        }
                      }
                    }

                    if (border.id.hash != Void) {
                      if (atom_combo(m_data, "##Wang2Atom", &border.id.hash, { m_edited_wang2.borders[1 - j].id.hash })) {
                        changed = true;
                        auto atom = m_data.get_atom(border.id.hash);
                        border.id = atom.id;
                      }

                      if (ImGui::Combo("Border##BorderEffect", &m_border_effect_choices[j], std::data(BorderEffectList), std::size(BorderEffectList))) {
                        changed = true;

                        border.effect = static_cast<BorderEffect>(m_border_effect_choices[j]);

                        switch (border.effect) {
                          case BorderEffect::None:
                            break;
                          case BorderEffect::Fade:
                            border.fade.distance = 11;
                            break;
                          case BorderEffect::Outline:
                            border.outline.distance = 6;
                            border.outline.factor = 0.2f;
                            break;
                          case BorderEffect::Sharpen:
                            border.sharpen.distance = 6;
                            break;
                          case BorderEffect::Lighten:
                            border.lighten.distance = 6;
                            break;
                          case BorderEffect::Blur:
                            break;
                          case BorderEffect::Blend:
                            border.blend.distance = 5;
                            break;
                        }
                      }

                      ImGui::Indent();

                      switch (border.effect) {
                        case BorderEffect::None:
                          break;
                        case BorderEffect::Fade:
                          changed = ImGui::SliderInt("Distance##Wang2Fade", &border.fade.distance, 1, m_data.settings.tile.size / 2) || changed;
                          break;
                        case BorderEffect::Outline:
                          changed = ImGui::SliderInt("Distance##Wang2Outline", &border.outline.distance, 1, m_data.settings.tile.size / 2) || changed;
                          changed = ImGui::SliderFloat("Factor##Wang2Outline", &border.outline.factor, 0.0f, 1.0f, "%.2f") || changed;
                          break;
                        case BorderEffect::Sharpen:
                          changed = ImGui::SliderInt("Distance##Wang2Sharpen", &border.sharpen.distance, 1, m_data.settings.tile.size / 2) || changed;
                          break;
                        case BorderEffect::Lighten:
                          changed = ImGui::SliderInt("Distance##Wang2Lighten", &border.lighten.distance, 1, m_data.settings.tile.size / 2) || changed;
                          break;
                        case BorderEffect::Blur:
                          break;
                        case BorderEffect::Blend:
                          changed = ImGui::SliderInt("Distance##Wang2Blend", &border.blend.distance, 1, m_data.settings.tile.size / 2) || changed;
                          break;
                      }

                      ImGui::Unindent();

                    }

                    ImGui::Separator();

                    ImGui::PopID();
                    ++j;
                  }

                  ImGui::Spacing();

                  changed = ImGui::SliderInt("Offset##Wang2Offset", &m_edited_wang2.edge.offset, -m_data.settings.tile.size / 4, m_data.settings.tile.size / 4) || changed;

                  if (ImGui::RadioButton("Limit##Wang2Limit", m_edited_wang2.edge.limit)) {
                    m_edited_wang2.edge.limit = !m_edited_wang2.edge.limit;
                  }

                  ImGui::Spacing();

                  changed = ImGui::SliderInt("Iterations##Wang2Iterations", &m_edited_wang2.edge.displacement.iterations, 0, 5) || changed;
                  changed = ImGui::SliderFloat("Initial factor##Wang2Initial", &m_edited_wang2.edge.displacement.initial, 0.1f, 1.0f, "%.2f") || changed;
                  changed = ImGui::SliderFloat("Reduction factor##Wang2Reduction", &m_edited_wang2.edge.displacement.reduction, 0.1f, 1.0f, "%.2f") || changed;

                  ImGui::Spacing();

                  static constexpr float PreviewSize = (128.0f + 3.0f) * 2;

                  if (changed) {
                    generate_preview();
                  }

                  ImGui::SetCursorPosX((ImGui::GetWindowWidth() - PreviewSize) / 2);
                  ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(&m_wang2_preview)), ImVec2(PreviewSize, PreviewSize));

                  ImGui::Spacing();

                  if (ImGui::Button("Save")) {
                    wang = m_edited_wang2;
                    ImGui::CloseCurrentPopup();
                    m_modified = true;
                  }

                  ImGui::SameLine();

                  if (ImGui::Button("Cancel")) {
                    ImGui::CloseCurrentPopup();
                  }

                  ImGui::SameLine();

                  if (ImGui::Button("Preview")) {
                    generate_preview();
                  }

                  ImGui::EndPopup();
                }

                ImGui::SameLine();

                if (m_data.settings.locked) {
                  ImGui::TextDisabled("Delete");
                } else {
                  if (ImGui::Button("Delete")) {
                    ImGui::OpenPopup("Delete");
                  }
                }

                if (ImGui::BeginPopupModal("Delete", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                  ImGui::Text("Are you sure you want to delete this?");

                  if (ImGui::Button("No, do not delete!")) {
                    ImGui::CloseCurrentPopup();
                  }

                  ImGui::SameLine();

                  if (ImGui::Button("Yes, I want to delete")) {
                    m_data.wang2.erase(std::next(m_data.wang2.begin(), std::ptrdiff_t(index)));
                    ImGui::CloseCurrentPopup();
                    m_modified = true;
                  }

                  ImGui::EndPopup();
                }

                ImGui::PopID();
                ++index;
              }

              ImGui::EndTable();
            }

          }

          ImGui::EndChild();

          if (ImGui::Button("New")) {
            assert(m_data.atoms.size() >= 2);
            Wang2 wang;
            wang.borders[0].id = m_data.atoms[0].id;
            wang.borders[0].effect = BorderEffect::None;
            wang.borders[1].id = m_data.atoms[1].id;
            wang.borders[1].effect = BorderEffect::None;
            m_data.wang2.emplace_back(std::move(wang));
            m_new_wang2 = true;
            m_modified = true;
          }

          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Wang3")) {
          ImGui::Text("Wang3 count: %zu/%i", m_data.wang3.size(), m_data.settings.max_wang3_count);
          ImGui::Spacing();

          if (ImGui::BeginChild("##Wang3", ImVec2(0, float(size.h - BottomMargin)))) {

            if (ImGui::BeginTable("##AtomTable", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
              ImGui::TableSetupColumn("Atom #1");
              ImGui::TableSetupColumn("Atom #2");
              ImGui::TableSetupColumn("Atom #3");
              ImGui::TableSetupColumn("Operations", ImGuiTableColumnFlags_WidthFixed);

              ImGui::TableHeadersRow();

              std::size_t index = 0;

              for (auto& wang : m_data.wang3) {
                ImGui::TableNextColumn();

                ImGui::PushID(int(index));

                for (const auto& id : wang.ids) {
                  if (id.hash == Void) {
                    ImGui::ColorButton("##Color", ImVec4(0, 0, 0, 0), ImGuiColorEditFlags_AlphaPreview);
                    ImGui::SameLine();
                    ImGui::Text("-");
                    ImGui::TableNextColumn();
                  } else {
                    auto atom = m_data.get_atom(id.hash);
                    ImGui::ColorButton("##Color", ImVec4(atom.color.r, atom.color.g, atom.color.b, atom.color.a));
                    ImGui::SameLine();
                    ImGui::Text("%s", atom.id.name.c_str());
                    ImGui::TableNextColumn();
                  }
                }

                if (!m_data.settings.locked && index + 1 < m_data.wang3.size()) {
                  if (ImGui::ArrowButton("Down", ImGuiDir_Down)) {
                    std::swap(m_data.wang3[index], m_data.wang3[index + 1]);
                    m_modified = true;
                  }
                } else {
                  ImGui::Dummy(ImVec2(EmptySize, EmptySize));
                }

                ImGui::SameLine();

                if (!m_data.settings.locked && index > 0) {
                  if (ImGui::ArrowButton("Up", ImGuiDir_Up)) {
                    std::swap(m_data.wang3[index], m_data.wang3[index - 1]);
                    m_modified = true;
                  }
                } else {
                  ImGui::Dummy(ImVec2(EmptySize, EmptySize));
                }

                ImGui::SameLine();

                auto prepare_edited_wang3 = [&]() {
                  m_edited_wang3 = wang;
                };

                auto generate_preview = [this]() {
                  m_wang3_preview.update(generate_wang3_preview(m_edited_wang3, m_random, m_data), m_render_manager);
                };

                if (ImGui::Button("Edit")) {
                  ImGui::OpenPopup("Edit");
                  prepare_edited_wang3();
                  generate_preview();
                }

                if (m_new_wang3 && index + 1 == m_data.wang3.size()) {
                  ImGui::SetScrollHereY(1.0f);
                  ImGui::OpenPopup("Edit");
                  prepare_edited_wang3();
                  generate_preview();
                  m_new_wang3 = false;
                }

                if (ImGui::BeginPopupModal("Edit", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                  bool changed = false;
                  int j = 0;

                  for (auto& id : m_edited_wang3.ids) {
                    ImGui::PushID(j);

                    ImGui::Text("Atom #%i\n", j+1);

                    if (j == 2) {
                      if (ImGui::RadioButton("Overlay##Wang3", id.hash == Void)) {
                        changed = true;

                        if (id.hash == Void) {
                          id = m_data.atoms.front().id;
                        } else {
                          id.name = "Void";
                          id.hash = Void;
                        }
                      }
                    }

                    if (id.hash != Void) {
                      if (atom_combo(m_data, "##Wang3Atom", &id.hash, { m_edited_wang3.ids[(j + 1) % 3].hash, m_edited_wang3.ids[(j + 2) % 3].hash })) {
                        changed = true;
                        auto atom = m_data.get_atom(id.hash);
                        id = atom.id;
                      }
                    }

                    ImGui::Separator();

                    ImGui::PopID();
                    ++j;
                  }

                  ImGui::Spacing();

                  static constexpr float PreviewSize = (192.0f + 5.0f) * 2;

                  if (changed) {
                    generate_preview();
                  }

                  ImGui::SetCursorPosX((ImGui::GetWindowWidth() - PreviewSize) / 2);
                  ImGui::Image(static_cast<ImTextureID>(reinterpret_cast<uintptr_t>(&m_wang3_preview)), ImVec2(PreviewSize, PreviewSize));

                  ImGui::Spacing();

                  if (ImGui::Button("Save")) {
                    // TODO: add missing wang2
                    wang = m_edited_wang3;
                    ImGui::CloseCurrentPopup();
                    m_modified = true;
                  }

                  ImGui::SameLine();

                  if (ImGui::Button("Cancel")) {
                    ImGui::CloseCurrentPopup();
                  }

                  ImGui::SameLine();

                  if (ImGui::Button("Preview")) {
                    generate_preview();
                  }

                  ImGui::EndPopup();
                }

                ImGui::SameLine();

                if (m_data.settings.locked) {
                  ImGui::TextDisabled("Delete");
                } else {
                  if (ImGui::Button("Delete")) {
                    ImGui::OpenPopup("Delete");
                  }
                }

                if (ImGui::BeginPopupModal("Delete", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
                  ImGui::Text("Are you sure you want to delete this?");

                  if (ImGui::Button("No, do not delete!")) {
                    ImGui::CloseCurrentPopup();
                  }

                  ImGui::SameLine();

                  if (ImGui::Button("Yes, I want to delete")) {
                    m_data.wang3.erase(std::next(m_data.wang3.begin(), std::ptrdiff_t(index)));
                    ImGui::CloseCurrentPopup();
                    m_modified = true;
                  }

                  ImGui::EndPopup();
                }

                ImGui::PopID();
                ++index;
              }

              ImGui::EndTable();

            }
          }

          ImGui::EndChild();

          if (ImGui::Button("New")) {
            assert(m_data.atoms.size() >= 3);
            Wang3 wang;
            wang.ids[0] = m_data.atoms[0].id;
            wang.ids[1] = m_data.atoms[1].id;
            wang.ids[2] = m_data.atoms[2].id;
            m_data.wang3.emplace_back(std::move(wang));
            m_new_wang3 = true;
            m_modified = true;
          }

          ImGui::SameLine();

          if (m_data.settings.locked) {
            ImGui::TextDisabled("Generate");
          } else {
            if (ImGui::Button("Generate")) {
              m_data.generate_all_wang3();
              m_modified = true;
            }
          }

          ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
      }

      ImGui::Separator();

      if (m_modified) {
        if (ImGui::Button("Save the current project")) {
          TilesetData::save(m_datafile, m_data);
          m_modified = false;
        }
      } else {
        ImGui::TextDisabled("Save the current project");
      }

      ImGui::SameLine();

      if (ImGui::Button("Export the tileset to TMX")) {
        auto tilesets = generate_tilesets(m_random, m_data);
        auto image = generate_tileset_image(m_random, m_data, tilesets);
        auto image_path = m_datafile.replace_extension(".png");
        image.save_to_file(image_path);
        auto xml = generate_tileset_xml(image_path.filename(), m_data, tilesets);
        auto xml_path = m_datafile.replace_extension(".tsx");
        std::ofstream(xml_path.string()) << xml;
      }

    }

    ImGui::End();
  }


}
