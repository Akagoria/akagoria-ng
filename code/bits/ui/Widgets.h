#ifndef AKGR_UI_WIDGETS_H
#define AKGR_UI_WIDGETS_H

#include <memory>
#include <vector>

#include <gf2/graphics/RenderManager.h>
#include <gf2/graphics/Shape.h>
#include <gf2/graphics/Text.h>

#include "Models.h"
#include "Properties.h"
#include "State.h"
#include "Theme.h"
#include "Widget.h"

namespace akgr::ui {

  /*
   * Layout widgets
   */

  class ContainerWidget : public Widget {
  public:
    ContainerWidget(Widget* parent);
    ContainerWidget(const ContainerWidget&) = delete;
    ContainerWidget(ContainerWidget&& other) noexcept;
    ~ContainerWidget();

    ContainerWidget& operator=(const ContainerWidget&) = delete;
    ContainerWidget& operator=(ContainerWidget&& other) noexcept;

    std::size_t children_count() const noexcept { return m_children.size(); }

    Widget* child(std::size_t i) noexcept { return m_children[i]; }
    const Widget* child(std::size_t i) const noexcept { return m_children[i]; }

    Widget* operator[](std::size_t i) noexcept { return m_children[i]; }
    const Widget* operator[](std::size_t i) const noexcept { return m_children[i]; }

    void add_child(std::unique_ptr<Widget> widget) noexcept { m_children.push_back(widget.release()); }

    template<typename Class, typename... Args>
    Class* add(Args&& ... args)
    {
      auto child = new Class(this, std::forward<Args>(args)...);
      m_children.push_back(child);
      return child;
    }

    Widget** begin() noexcept { return m_children.data(); }
    Widget** end() noexcept { return m_children.data() + m_children.size(); }

    void render_children(gf::RenderRecorder& recorder);

  private:
    std::vector<Widget*> m_children;
  };

  class BoxWidget : public ContainerWidget {
  public:
    BoxWidget(Widget* parent, Aspect aspect, BoxTheme* theme);

    Aspect aspect() const noexcept { return m_aspect; }
    void set_aspect(Aspect aspect) noexcept { m_aspect = aspect; }

    Positioning positioning() const noexcept { return m_positioning; }
    void set_positioning(Positioning positioning) noexcept { m_positioning = positioning; }

    gf::Vec2F offset() const noexcept { return m_offset; }
    void set_offset(gf::Vec2F offset) noexcept { m_offset = offset; }

    void layout_request() override;
    void layout_allocation() override;
    virtual void on_layout_finished();

  private:
    Aspect m_aspect;
    Positioning m_positioning = Positioning::Minimum;
    gf::Vec2F m_offset = { 0.0f, 0.0f };
    BoxTheme* m_theme = nullptr;
  };

  class StackWidget : public ContainerWidget {
  public:
    StackWidget(Widget* parent, StackTheme* theme);

    Positioning positioning() const noexcept { return m_positioning; }
    void set_positioning(Positioning positioning) noexcept { m_positioning = positioning; }

    void layout_request() override;
    void layout_allocation() override;

  private:
    Positioning m_positioning = Positioning::Middle;
    StackTheme* m_theme = nullptr;
  };

  class ListWidget : public Widget {
  public:
    ListWidget(Widget* parent, ListModel* model);

    Positioning positioning() const noexcept { return m_positioning; }
    void set_positioning(Positioning positioning) noexcept { m_positioning = positioning; }

    gf::Vec2F margin() const noexcept { return m_margin; }
    void set_margin(gf::Vec2F margin) noexcept { m_margin = margin; }

    gf::Vec2F offset() const noexcept { return m_offset; }
    void set_offset(gf::Vec2F offset) noexcept { m_offset = offset; }

    float spacing() const noexcept { return m_spacing; }
    void set_spacing(float spacing) noexcept { m_spacing = spacing; }

    void layout_request() override;
    void layout_allocation() override;

  protected:
    ListModel* model() { return m_model; }

  private:
    ListModel* m_model;
    Positioning m_positioning;
    gf::Vec2F m_margin;
    gf::Vec2F m_offset;
    float m_spacing;
  };

  /*
   * Content widget
   */

  class FrameWidget : public BoxWidget {
  public:
    FrameWidget(Widget* parent, FrameTheme* theme, gf::RenderManager* render_manager);

    void render(gf::RenderRecorder& recorder) override;

    void on_layout_finished() override;

  private:
    FrameTheme* m_theme = nullptr;
    gf::RenderManager* m_render_manager = nullptr;
    gf::ShapeGroup m_shape;
  };

  class MenuWidget : public BoxWidget {
  public:
    MenuWidget(Widget* parent, std::unique_ptr<Widget> arrow, const IndexState* index, MenuTheme* theme);

    void render(gf::RenderRecorder& recorder) override;

    void layout_request() override;
    void layout_allocation() override;

  private:
    std::unique_ptr<Widget> m_arrow = nullptr;
    const IndexState* m_index = nullptr;
    MenuTheme* m_theme = nullptr;
  };

  class ChoiceWidget : public StackWidget {
  public:
    ChoiceWidget(Widget* parent, std::unique_ptr<Widget> left_arrow, std::unique_ptr<Widget> right_arrow, const IndexState* index, ChoiceTheme* theme);

    void render(gf::RenderRecorder& recorder) override;

    void layout_request() override;
    void layout_allocation() override;

  private:
    std::unique_ptr<Widget> m_left_arrow = nullptr;
    std::unique_ptr<Widget> m_right_arrow = nullptr;
    const IndexState* m_index = nullptr;
    ChoiceTheme* m_theme = nullptr;
  };

#if 0
  class CatalogueWidget : public ListWidget {
  public:
    CatalogueWidget(Widget* parent, ListModel& model, const WidgetListScenery& scenery);

  private:
    const WidgetListScenery& m_scenery;
  };
#endif

  class LabelWidget : public Widget {
  public:
    LabelWidget(Widget* parent, LabelTheme* theme, gf::Text* text);

    void render(gf::RenderRecorder& recorder) override;

    void layout_request() override;
    void layout_allocation() override;

  private:
    LabelTheme* m_theme = nullptr;
    gf::Text* m_text = nullptr;
  };

#if 0
  class TextWidget : public Widget {
  public:
    TextWidget(Widget* parent, std::string caption, float characterSize = 0.03f, gf::Alignment alignment = gf::Alignment::Left);

    void setCaption(std::string caption) { m_caption = std::move(caption); }
    std::string getCaption() const { return m_caption; }

  private:
    std::string m_caption;
    float m_characterSize;
    gf::Alignment m_alignment;
  };

  class DoubleTextWidget : public Widget {
  public:
    DoubleTextWidget(Widget* parent, std::string leftCaption, std::string rightCaption, float characterSize = 0.03f);

    void setLeftCaption(std::string caption) { m_leftCaption = std::move(caption); }
    std::string getLeftCaption() const { return m_leftCaption; }

    void setRightCaption(std::string caption) { m_rightCaption = std::move(caption); }
    std::string getRightCaption() const { return m_rightCaption; }

  private:
    std::string m_leftCaption;
    std::string m_rightCaption;
    float m_characterSize;
  };
#endif

}

#endif // AKGR_UI_WIDGETS_H
