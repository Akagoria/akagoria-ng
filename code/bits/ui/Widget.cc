#include "Widget.h"

#include <utility>

namespace akgr::ui {

  Widget::Widget(Widget* parent)
  : m_parent(parent)
  {
  }

  Widget::~Widget() = default;

  Widget::Widget(Widget&& other) noexcept
  : m_parent(std::exchange(other.m_parent, nullptr))
  , m_position(other.m_position)
  , m_size(other.m_size)
  {

  }

  Widget& Widget::operator=(Widget&& other) noexcept {
    std::swap(m_parent, other.m_parent);
    std::swap(m_position, other.m_position);
    std::swap(m_size, other.m_size);
    return *this;
  }

  gf::Vec2F Widget::absolute_position() const noexcept {
    if (m_parent != nullptr) {
      return m_parent->absolute_position() + m_position;
    }

    return m_position;
  }

  void Widget::layout_request() {
  }

  void Widget::layout_allocation() {
  }

}
