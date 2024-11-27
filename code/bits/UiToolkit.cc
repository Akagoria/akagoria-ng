#include "UiToolkit.h"

#include <gf2/core/Log.h>

namespace akgr {

  gf::ActionGroupSettings compute_ui_settings()
  {
    using namespace gf::literals;
    gf::ActionGroupSettings settings;

    settings.actions.emplace("up"_id, gf::instantaneous_action().add_keycode_control(gf::Keycode::Up));
    settings.actions.emplace("down"_id, gf::instantaneous_action().add_keycode_control(gf::Keycode::Down));
    settings.actions.emplace("left"_id, gf::instantaneous_action().add_keycode_control(gf::Keycode::Left));
    settings.actions.emplace("right"_id, gf::instantaneous_action().add_keycode_control(gf::Keycode::Right));
    settings.actions.emplace("page_up"_id, gf::instantaneous_action().add_keycode_control(gf::Keycode::PageUp));
    settings.actions.emplace("page_down"_id, gf::instantaneous_action().add_keycode_control(gf::Keycode::PageDown));
    settings.actions.emplace("use"_id, gf::instantaneous_action().add_scancode_control(gf::Scancode::Return));

    return settings;
  }

  /*
   * UiElement
   */

  UiElement::~UiElement() = default;

  void UiElement::on_down([[maybe_unused]] UiToolkit& toolkit)
  {
  }

  void UiElement::on_up([[maybe_unused]] UiToolkit& toolkit)
  {
  }

  void UiElement::on_left([[maybe_unused]] UiToolkit& toolkit)
  {
  }

  void UiElement::on_right([[maybe_unused]] UiToolkit& toolkit)
  {
  }

  void UiElement::on_page_down([[maybe_unused]] UiToolkit& toolkit)
  {
  }

  void UiElement::on_page_up([[maybe_unused]] UiToolkit& toolkit)
  {
  }

  void UiElement::on_use([[maybe_unused]] UiToolkit& toolkit)
  {
  }

  void UiElement::on_visibility_change([[maybe_unused]] bool visible)
  {
  }

  /*
   * UiToolkit
   */

  void UiToolkit::add_element(gf::Id id, UiElement* element)
  {
    if (element != nullptr) {
      m_elements.emplace(id, element);
    }
  }

  void UiToolkit::handle_actions(const gf::ActionGroup& actions)
  {
    if (m_current_element == nullptr) {
      return;
    }

    using namespace gf::literals;

    if (actions.active("down"_id)) {
      m_current_element->on_down(*this);
    }

    if (actions.active("up"_id)) {
      m_current_element->on_up(*this);
    }

    if (actions.active("left"_id)) {
      m_current_element->on_left(*this);
    }

    if (actions.active("right"_id)) {
      m_current_element->on_right(*this);
    }

    if (actions.active("page_down"_id)) {
      m_current_element->on_page_down(*this);
    }

    if (actions.active("page_up"_id)) {
      m_current_element->on_page_up(*this);
    }

    if (actions.active("use"_id)) {
      m_current_element->on_use(*this);
    }
  }

  void UiToolkit::change_ui_element(gf::Id id)
  {
    if (m_current_element != nullptr) {
      m_current_element->on_visibility_change(false);
      m_current_element->entity()->set_visible(false);
    }

    auto iterator = m_elements.find(id);

    if (iterator == m_elements.end()) {
      gf::Log::fatal("No element found with id '{:X}'", uint64_t(id));
    }

    m_current_element = iterator->second;

    if (m_current_element != nullptr) {
      m_current_element->on_visibility_change(true);
      m_current_element->entity()->set_visible(true);
    }
  }

}
