#include "Widgets.h"
#include <utility>

#include <gf2/core/Color.h>
#include <gf2/core/Positioning.h>
#include <gf2/core/Transform.h>
#include <gf2/core/Log.h>

#include <gf2/graphics/RenderRecorder.h>
#include <gf2/graphics/Texture.h>
#include "gf2/core/ShapeBuffer.h"

namespace akgr::ui {

  namespace {

    constexpr gf::Color RpgBlue(0.015625, 0.03125, 0.515625, 0.9f);
    constexpr float Padding = 0.01f;
    constexpr float Thickness = 0.002f;

    constexpr float ArrowSize = 64.0f;

  } // namespace

  /*
   * ContainerWidget
   */

  ContainerWidget::ContainerWidget(Widget* parent)
  : Widget(parent)
  {
  }

  ContainerWidget::ContainerWidget(ContainerWidget&& other) noexcept
  : Widget(std::move(other))
  , m_children(std::move(other.m_children))
  {
  }

  ContainerWidget::~ContainerWidget() {
    for (auto* child : m_children) {
      delete child;
    }
  }

  ContainerWidget& ContainerWidget::operator=(ContainerWidget&& other) noexcept
  {
    Widget::operator=(std::move(other));
    std::swap(m_children, other.m_children);
    return *this;
  }

  void ContainerWidget::render_children(gf::RenderRecorder& recorder) {
    for (auto* child : m_children) {
      child->render(recorder);
    }
  }

  /*
   * BoxWidget
   */

  BoxWidget::BoxWidget(Widget* parent, Aspect aspect, BoxTheme* theme)
  : ContainerWidget(parent)
  , m_aspect(aspect)
  , m_theme(theme)
  {
  }

  void BoxWidget::layout_request() {
    gf::Log::debug("BoxWidget::do_layout_request()");

    gf::Vec2F container = m_theme->padding.ne + m_theme->padding.sw;

    float gf::Vec2F::* axis0 = m_aspect == Aspect::Horizontal ? &gf::Vec2F::w : &gf::Vec2F::h;
    float gf::Vec2F::* axis1 = m_aspect == Aspect::Vertical   ? &gf::Vec2F::w : &gf::Vec2F::h;

    for (auto* child : *this) {
      child->layout_request();
      const gf::Vec2F size = child->size();
      container.*axis0 += size.*axis0;
      container.*axis1 = std::max(container.*axis1, size.*axis1 + m_theme->padding.ne.*axis1 + m_theme->padding.sw.*axis1);
    }

    if (children_count() > 1) {
      container.*axis0 += static_cast<float>(children_count() - 1) * m_theme->spacing;
    }

    container += m_offset;
    set_size(gf::max(container, size()));

    gf::Log::debug("container: {} x {}", container.w, container.h);
  }

  void BoxWidget::layout_allocation() {
    gf::Log::debug("BoxWidget::do_layout_allocation()");

    gf::Vec2F container = size();
    gf::Log::debug("container: {} x {}", container.w, container.h);

    float gf::Vec2F::* axis0 = m_aspect == Aspect::Horizontal ? &gf::Vec2F::w : &gf::Vec2F::h;
    float gf::Vec2F::* axis1 = m_aspect == Aspect::Vertical   ? &gf::Vec2F::w : &gf::Vec2F::h;

    gf::Vec2F position = m_theme->padding.ne;

    for (auto* child : *this) {
      gf::Vec2F size = child->size();

      switch (m_positioning) {
        case Positioning::Minimum:
          position.*axis1 = m_theme->padding.ne.*axis1;
          break;
        case Positioning::Middle:
          position.*axis1 = m_theme->padding.ne.*axis1 + (container.*axis1 - m_theme->padding.ne.*axis1 - m_theme->padding.sw.*axis1 - size.*axis1) / 2;
          break;
        case Positioning::Maximum:
          position.*axis1 = container.*axis1 - size.*axis1 - m_theme->padding.sw.*axis1;
          break;
        case Positioning::Fill:
          position.*axis1 = m_theme->padding.ne.*axis1;
          size.*axis1 = container.*axis1 - m_theme->padding.ne.*axis1 - m_theme->padding.sw.*axis1;
          break;
      }

      gf::Log::debug("child: {}x{}, {}x{}", position.x, position.y, size.w, size.h);

      child->set_position(m_offset + position);
      child->set_size(size);
      child->layout_allocation();

      position.*axis0 += size.*axis0;
      position.*axis0 += m_theme->spacing;
    }

    on_layout_finished();
  }

  void BoxWidget::on_layout_finished()
  {
  }

  /*
   * StackWidget
   */

  // StackWidget::StackWidget(Widget* parent)
  // : ContainerWidget(parent)
  // , m_positioning(Positioning::Middle)
  // {
  // }
  //
  // void StackWidget::do_layout_request() {
  //   gf::Vec2F container = size();
  //
  //   for (auto* child : *this) {
  //     child->request_layout();
  //     container = gf::max(container, child->size() + m_theme->padding.ne.*axis1);
  //   }
  //
  //   set_size(container);
  // }
  //
  // void StackWidget::do_layout_allocation() {
  //   gf::Vec2F container = size();
  //
  //   for (auto* child : *this) {
  //     gf::Vec2F size = child->size();
  //     gf::Vec2F position = m_margin;
  //
  //     switch (m_positioning) {
  //       case Positioning::Minimum:
  //         position = m_margin;
  //         break;
  //       case Positioning::Middle:
  //         position = (container - size) / 2;
  //         break;
  //       case Positioning::Maximum:
  //         position = container - size - m_margin;
  //         break;
  //       case Positioning::Fill:
  //         size = container - 2 * m_margin;
  //         break;
  //     }
  //
  //     child->set_position(position);
  //     child->set_size(size);
  //     child->allocate_layout();
  //   }
  // }

  /*
    * ListWidget
    */

  ListWidget::ListWidget(Widget* parent, ListModel* model)
  : Widget(parent)
  , m_model(model)
  , m_positioning(Positioning::Middle)
  , m_margin({ 0.0f, 0.0f })
  , m_offset({ 0.0f, 0.0f })
  , m_spacing(0.0f)
  {
  }

  void ListWidget::layout_request() {
    gf::Vec2F container = 2 * m_margin;

    m_model->update(this);
    std::size_t count = m_model->widget_count();

    for (std::size_t i = 0; i < count; ++i) {
      auto* widget = m_model->widget(i);
      widget->layout_request();
      gf::Vec2F size = widget->size();
      container.h += size.h;
      container.w = std::max(container.w, size.w + 2 * m_margin.w);
    }

    if (count > 0) {
      container.h += static_cast<float>(count - 1) * m_spacing;
    }

    set_size(m_offset + container);
  }

  void ListWidget::layout_allocation() {
    gf::Vec2F container = size();

    gf::Vec2F position = m_margin;

    std::size_t count = m_model->widget_count();

    for (std::size_t i = 0; i < count; ++i) {
      auto* widget = m_model->widget(i);
      gf::Vec2F size = widget->size();

      switch (m_positioning) {
        case Positioning::Minimum:
          position.x = m_margin.w;
          break;
        case Positioning::Middle:
          position.x = (container.w - size.w) / 2;
          break;
        case Positioning::Maximum:
          position.x = container.w - size.w - m_margin.w;
          break;
        case Positioning::Fill:
          position.x = m_margin.w;
          size.w = container.w - 2 * m_margin.w;
          break;
      }

      widget->set_position(m_offset + position);
      widget->set_size(size);
      widget->layout_allocation();

      position.y += size.h;
      position.y += m_spacing;
    }
  }

  /*
   * FrameWidget
   */

  FrameWidget::FrameWidget(Widget* parent, FrameTheme* theme, gf::RenderManager* render_manager)
  : BoxWidget(parent, Aspect::Vertical, theme)
  , m_theme(theme)
  , m_render_manager(render_manager)
  {
  }

  void FrameWidget::render(gf::RenderRecorder& recorder) {
    auto geometry = m_shape.geometry();

    if (geometry.size == 0) {
      return;
    }

    gf::RenderObject object = {};
    object.priority = priority();
    object.geometry = m_shape.geometry();
    recorder.record(object);

    render_children(recorder);
  }

  void FrameWidget::on_layout_finished()
  {
    const gf::Vec2F widget_position = absolute_position();
    const gf::Vec2F widget_size = size();

    gf::ShapeBuffer buffer = gf::ShapeBuffer::make_rounded_rectangle(gf::RectF::from_position_size(widget_position, widget_size), m_theme->radius);
    buffer.color = m_theme->color;
    buffer.outline_thickness = m_theme->outline_thickness;
    buffer.outline_color = m_theme->outline_color;

    gf::ShapeGroupBuffer group_buffer;
    group_buffer.shapes.push_back(std::move(buffer));

    m_shape.update(group_buffer, m_render_manager);

    gf::Log::debug("FrameWidget: {} x {}.  {} x {}", widget_position.x, widget_position.y, widget_size.x, widget_size.y);
  }

  /*
   * MenuWidget
   */

  MenuWidget::MenuWidget(Widget* parent, Widget* arrow, const IndexState* index, MenuTheme* theme)
  : BoxWidget(parent, Aspect::Vertical, theme)
  , m_arrow(arrow)
  , m_index(index)
  , m_theme(theme)
  {
    set_positioning(Positioning::Minimum);
    m_arrow->set_parent(this);
  }

  MenuWidget::MenuWidget(MenuWidget&& other) noexcept
  : BoxWidget(std::move(other))
  , m_arrow(std::exchange(other.m_arrow, nullptr))
  , m_index(std::exchange(other.m_index, nullptr))
  , m_theme(std::exchange(other.m_theme, nullptr))
  {
  }

  MenuWidget::~MenuWidget()
  {
    delete m_arrow;
  }

  MenuWidget& MenuWidget::operator=(MenuWidget&& other) noexcept
  {
    BoxWidget::operator=(std::move(other));
    std::swap(m_arrow, other.m_arrow);
    std::swap(m_index, other.m_index);
    std::swap(m_theme, other.m_theme);
    return *this;
  }

  void MenuWidget::render(gf::RenderRecorder& recorder)
  {
    render_children(recorder);
    m_arrow->render(recorder);
  }

  void MenuWidget::layout_request()
  {
    m_arrow->layout_request();
    auto size = m_arrow->size();
    set_offset({ size.w, 0.0f });
    BoxWidget::layout_request();
  }

  void MenuWidget::layout_allocation()
  {
    BoxWidget::layout_allocation();

    const std::size_t chosen = m_index->choice;
    assert(chosen < children_count());
    auto *widget = child(chosen);
    assert(widget != nullptr);

    const gf::Vec2F chosen_size = widget->size();
    const gf::Vec2F arrow_size = m_arrow->size();
    gf::Vec2F position = widget->position();

    position.x -= arrow_size.w;
    position.y += (chosen_size.h - arrow_size.h) / 2.0f;

    m_arrow->set_position(position);
  }


#if 0
  /*
   * ChoiceWidget
   */

  ChoiceWidget::ChoiceWidget(Widget* parent, const WidgetIndexScenery& scenery)
  : StackWidget(parent)
  , m_scenery(scenery)
  {

  }

  void ChoiceWidget::render(gf::RenderTarget& target, const gf::RenderStates& states, Theme& theme) {
    std::size_t i = m_scenery.choice;
    assert(i < getChildrenCount());
    auto widget = getChild(i);
    assert(widget);
    widget->render(target, states, theme);

    gf::Coordinates coords(target);

    gf::Vec2F position = coords.getRelativePoint(getAbsolutePosition());
    gf::Vec2F size = coords.getRelativeSize(getSize());

    float square = size.height;

    gf::CompoundCurve back({ square / 2, square / 4 });
    back.lineTo({ square / 4, square / 2 });
    back.lineTo({ square / 2, 3 * square / 4 });
    back.setPosition(position);
    back.setType(gf::Curve::Outlined);
    back.setWidth(size.height / 10);
    back.setColor(gf::Color::White);
    back.setOutlineColor(gf::Color::Black);
    back.setOutlineThickness(1);
    target.draw(back, states);

    gf::CompoundCurve forth({ - square / 2, square / 4 });
    forth.lineTo({ - square / 4, square / 2 });
    forth.lineTo({ - square / 2, 3 * square / 4 });
    forth.setPosition({ position.x + size.width, position.y });
    forth.setType(gf::Curve::Outlined);
    forth.setWidth(size.height / 10);
    forth.setColor(gf::Color::White);
    forth.setOutlineColor(gf::Color::Black);
    forth.setOutlineThickness(1);
    target.draw(forth, states);
  }

  /*
    * CatalogueWidget
    */

  CatalogueWidget::CatalogueWidget(Widget* parent, ListModel& model, const WidgetListScenery& scenery)
  : ListWidget(parent, model)
  , m_scenery(scenery)
  {
    set_positioning(Positioning::Minimum);
    set_offset(gf::dirx(0.02f));
    set_spacing(0.01f);
    set_margin({ 0.01f, 0.01f });
  }

  void CatalogueWidget::render(gf::RenderTarget& target, const gf::RenderStates& states, Theme& theme) {
    std::size_t count = getModel().getWidgetCount();

    assert(count <= m_scenery.itemsPerPage);

    if (count == 0) {
      return;
    }

    for (std::size_t i = 0; i < count; ++i) {
      getModel().getWidget(i)->render(target, states, theme);
    }

    std::size_t i = m_scenery.choice % m_scenery.itemsPerPage;
    assert(i < count);
    auto widget = getModel().getWidget(i);
    assert(widget);
    auto widgetPosition = widget->getAbsolutePosition();
    auto widgetSize = widget->getSize();

    gf::Vec2F position = widgetPosition + gf::projy(widgetSize) / 2 - gf::dirx(0.015f);

    gf::Coordinates coords(target);

    float height = coords.getRelativeSize({ 0.0f, 0.03f }).height;

    gf::Sprite sprite(theme.getArrowTexture(), gf::RectF::fromPositionSize({ 0.25f, 0.0f }, { 0.125f, 0.125f }));
    sprite.setPosition(coords.getRelativePoint(position));
    sprite.setScale(height / ArrowSize);
    sprite.setAnchor(gf::Anchor::Center);
    target.draw(sprite, states);
  }
#endif

  /*
   * LabelWidget
   */

  LabelWidget::LabelWidget(Widget* parent, LabelTheme* theme, const gf::Text* text)
  : Widget(parent)
  , m_theme(theme)
  , m_text(text)
  {
  }

  void LabelWidget::render(gf::RenderRecorder& recorder)
  {
    auto geometry = m_text->geometry();

    if (geometry.size == 0) {
      return;
    }

    gf::Transform transform;
    transform.location = absolute_position() + m_theme->padding.ne;

    gf::RenderObject object = {};
    object.priority = priority();
    object.geometry = geometry;
    object.transform = transform.compute_matrix(m_text->bounds());
    recorder.record(object);
  }

  void LabelWidget::layout_request()
  {
    const gf::RectF bounds = m_text->bounds();
    set_size(bounds.size() + m_theme->padding.ne + m_theme->padding.sw);
  }

  void LabelWidget::layout_allocation()
  {
  }


#if 0
  /*
   * TextWidget
   */

  TextWidget::TextWidget(Widget* parent, std::string caption, float characterSize, gf::Alignment alignment)
  : Widget(parent)
  , m_caption(std::move(caption))
  , m_characterSize(characterSize)
  , m_alignment(alignment)
  {

  }

  void TextWidget::render(gf::RenderTarget& target, const gf::RenderStates& states, Theme& theme) {
    gf::Coordinates coords(target);

    gf::Vec2F position = coords.getRelativePoint(getAbsolutePosition());
    gf::Vec2F size = coords.getRelativeSize(getSize());
    unsigned characterSize = coords.getRelativeCharacterSize(m_characterSize);

    gf::Text text(m_caption, theme.getFont(), characterSize);
    text.setParagraphWidth(size.width);
    text.setAlignment(m_alignment);
    text.setColor(gf::Color::White);
    text.setOutlineColor(gf::Color::Black);
    text.setOutlineThickness(1);
    text.setAnchor(gf::Anchor::TopLeft);

    gf::Vec2F bounds = text.getLocalBounds().getSize();
    text.setPosition(position);
    target.draw(text, states);

    if (bounds.width > size.width || bounds.height > size.height) {
      gf::Log::debug("TextWidget::render(): text can not fit in area, area is (%g, %g), bounds is (%g, %g), text is: \"%s\"\n", size.width, size.height, bounds.width, bounds.height, gf::escapeString(m_caption).c_str());
    }
  }

  /*
   * DoubleTextWidget
   */

  DoubleTextWidget::DoubleTextWidget(Widget* parent, std::string leftCaption, std::string rightCaption, float characterSize)
  : Widget(parent)
  , m_leftCaption(leftCaption)
  , m_rightCaption(rightCaption)
  , m_characterSize(characterSize)
  {

  }

  void DoubleTextWidget::render(gf::RenderTarget& target, const gf::RenderStates& states, Theme& theme) {
    gf::Coordinates coords(target);

    gf::Vec2F position = coords.getRelativePoint(getAbsolutePosition());
    gf::Vec2F size = coords.getRelativeSize(getSize());
    unsigned characterSize = coords.getRelativeCharacterSize(m_characterSize);

    auto displayText = [&](const std::string& caption, gf::Alignment alignment) {
      gf::Text text(caption, theme.getFont(), characterSize);
      text.setParagraphWidth(size.width);
      text.setAlignment(alignment);
      text.setColor(gf::Color::White);
      text.setOutlineColor(gf::Color::Black);
      text.setOutlineThickness(1);
      text.setAnchor(gf::Anchor::CenterLeft);

      gf::Vec2F bounds = text.getLocalBounds().getSize();
      text.setPosition(position + gf::diry(size.height / 2));
      target.draw(text, states);

      if (bounds.width > size.width || bounds.height > size.height) {
        gf::Log::debug("DoubleTextWidget::render(): text can not fit in area, area is (%g, %g), bounds is (%g, %g), text is: \"%s\"\n", size.width, size.height, bounds.width, bounds.height, gf::escapeString(caption).c_str());
      }
    };

    displayText(m_rightCaption, gf::Alignment::Right);
    displayText(m_leftCaption, gf::Alignment::Left);
  }
#endif

}
