#ifndef AKGR_UI_TOOLKIT_H
#define AKGR_UI_TOOLKIT_H

#include <gf2/core/ActionSettings.h>

#include <gf2/core/ActionGroup.h>
#include <gf2/graphics/Entity.h>

namespace akgr {

  gf::ActionGroupSettings compute_ui_settings();

  class UiEntity : public gf::Entity {
  public:

    bool visible() const { return m_visible; }
    void set_visible(bool visible) { m_visible = visible; }

  private:
    bool m_visible = false;
  };

  class UiToolkit;

  class UiElement {
  public:
    UiElement() = default;
    UiElement(const UiElement&) = default;
    UiElement(UiElement&&) noexcept = default;
    virtual ~UiElement();

    UiElement& operator=(const UiElement&) = default;
    UiElement& operator=(UiElement&&) noexcept = default;

    virtual void on_down(UiToolkit& toolkit);
    virtual void on_up(UiToolkit& toolkit);
    virtual void on_left(UiToolkit& toolkit);
    virtual void on_right(UiToolkit& toolkit);
    virtual void on_page_down(UiToolkit& toolkit);
    virtual void on_page_up(UiToolkit& toolkit);
    virtual void on_use(UiToolkit& toolkit);

    virtual void on_visibility_change(bool visible);

    virtual UiEntity* entity() = 0;
  };

  class UiToolkit {
  public:

    void add_element(gf::Id id, UiElement* element);

    void handle_actions(const gf::ActionGroup& actions);

    void change_ui_element(gf::Id);

  private:
    UiElement* m_current_element = nullptr;
    std::map<gf::Id, UiElement*> m_elements;
  };

}

#endif // AKGR_UI_TOOLKIT_H
