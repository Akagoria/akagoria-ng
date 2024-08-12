#ifndef AKGR_UI_WIDGET_H
#define AKGR_UI_WIDGET_H

#include <gf2/core/Rect.h>
#include <gf2/core/Vec2.h>

#include <gf2/graphics/Entity.h>

namespace akgr::ui {

  class Widget : public gf::Entity {
  public:
    Widget(Widget* parent);
    virtual ~Widget();

    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;

    Widget(Widget&& other) noexcept;
    Widget& operator=(Widget&& other) noexcept;

    Widget* parent() noexcept { return m_parent; }
    const Widget* parent() const noexcept { return m_parent; }
    void set_parent(Widget* parent) noexcept { m_parent = parent; }

    gf::Vec2F position() const noexcept { return m_position; }
    void set_position(gf::Vec2F position) noexcept { m_position = position; }

    gf::Vec2F absolute_position() const noexcept;

    gf::Vec2F size() const noexcept { return m_size; }
    void set_size(gf::Vec2F size) noexcept { m_size = size; }

    gf::RectF bounds() const noexcept
    {
      return gf::RectF::from_position_size(m_position, m_size);
    }

    virtual void layout_request();
    virtual void layout_allocation();

    void compute_layout() {
      layout_request();
      layout_allocation();
    }

  private:
    Widget* m_parent = nullptr;
    gf::Vec2F m_position = { 0.0f, 0.0f };
    gf::Vec2F m_size = { 0.0f, 0.0f };
  };

}


#endif // AKGR_UI_WIDGET_H
