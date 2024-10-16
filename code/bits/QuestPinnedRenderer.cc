#include "QuestPinnedRenderer.h"

#include "Akagoria.h"
#include "QuestData.h"

namespace akgr {

  namespace {

    gf::RichTextStyle compute_quest_style()
    {
      gf::RichTextStyle style;
      style.set_style("title", { 1.2f, gf::White, gf::FontStyle::Bold });
      style.set_style("history", { 1.0f, gf::Yellow, gf::FontStyle::Bold });
      style.set_style("current", { 1.0f, gf::White, gf::FontStyle::Italic });
      style.set_style("progress", { 1.0f, gf::White, gf::None });
      style.set_style("character", { 1.0f, gf::Chartreuse, gf::None });
      style.set_style("location", { 1.0f, gf::Orange, gf::None });
      return style;
    }

  }

  QuestPinnedRenderer::QuestPinnedRenderer(Akagoria* game, const WorldResources& resources)
  : m_game(game)
  , m_atlas({ 1024, 1024 }, game->render_manager())
  , m_style(compute_quest_style())
  , m_default_text_data(resources.pinned_quest_text.data)
  , m_quest_texts({ initial_text(resources), initial_text(resources), initial_text(resources), initial_text(resources), initial_text(resources) })
  , m_background_shape(compute_shape_buffer(), game->render_manager())
  {
    m_background_shape.set_location({ 1230.0f, 280.0f });
  }

  void QuestPinnedRenderer::update(gf::Time time)
  {
    std::vector<QuestState*> visible_quests;

    for (auto& quest : m_game->world_state()->hero.quests) {
      if (quest.status != QuestStatus::Visible) {
        continue;
      }

      visible_quests.push_back(&quest);
    }

    if (visible_quests.empty()) {
      m_quest_count = 0;
      return;
    }

    std::sort(visible_quests.begin(), visible_quests.end(), [](const QuestState* lhs, const QuestState* rhs) {
      if (lhs->data->scope == QuestScope::History) {
        return true;
      }

      if (rhs->data->scope == QuestScope::History) {
        return false;
      }

      return lhs->last_update > rhs->last_update;
    });

    if (visible_quests.size() > PinnedQuestCount) {
      visible_quests.resize(PinnedQuestCount);
    }

    m_quest_count = visible_quests.size();

    for (std::size_t i = 0; i < m_quest_count; ++i) {
      const auto* quest = visible_quests[i];
      const std::size_t step_index = quest->current_step;

      gf::TextData data = m_default_text_data;

      if (quest->data->scope == QuestScope::History) {
        data.content = fmt::format("<style=title><style=history>{}</></>", quest->data->title);
      } else {
        data.content = fmt::format("<style=title>{}</>", quest->data->title);
      }

      switch (quest->type()) {
        case QuestType::None:
          break;
        case QuestType::Hunt:
          {
            data.content += '\n';

            const auto& quest_state = std::get<HuntQuestState>(quest->features);
            const auto& quest_data = std::get<HuntQuestData>(quest->data->steps[step_index].features);

            data.content += fmt::format("→ <style=current>{}</> <style=progress>{}/{}</>", quest->data->steps[step_index].description, quest_state.amount, quest_data.count);
          }
          break;
        case QuestType::Talk:
          {
            data.content += '\n';
            data.content += fmt::format("→ <style=current>{}</>", quest->data->steps[step_index].description);
          }
          break;
        case QuestType::Farm:
          {
            data.content += '\n';

            const auto& quest_state = std::get<FarmQuestState>(quest->features);
            const auto& quest_data = std::get<FarmQuestData>(quest->data->steps[step_index].features);

            data.content += fmt::format("→ <style=current>{}</> <style=progress>{}/{}</>", quest->data->steps[step_index].description, quest_state.amount, quest_data.count);
          }
          break;
        case QuestType::Explore:
          {
            data.content += '\n';
            data.content += fmt::format("→ {}", quest->data->steps[step_index].description);
          }
          break;
      }

      m_quest_texts[i].text().update(data, m_game->render_manager()); // NOLINT
      m_quest_texts[i].set_location({ 1250.0f, 300.0f + 50.0f * float(i) }); // NOLINT
    }

  }

  void QuestPinnedRenderer::render(gf::RenderRecorder& recorder)
  {
    m_background_shape.render(recorder);

    for (std::size_t i = 0; i < m_quest_count; ++i) {
      m_quest_texts[i].render(recorder); // NOLINT
    }
  }

  gf::RichTextEntity QuestPinnedRenderer::initial_text(const WorldResources& resources)
  {
    return { &m_atlas, &m_style, resources.pinned_quest_text, m_game->render_manager(), m_game->resource_manager() };
  }

  gf::ShapeGroupBuffer QuestPinnedRenderer::compute_shape_buffer()
  {
    gf::ShapeGroupBuffer buffer;

    gf::ShapeBuffer frame = gf::ShapeBuffer::make_rectangle({ 340.0f, 500.0f });
    frame.color = gf::Gray * gf::opaque(0.8f);
    buffer.shapes.push_back(std::move(frame));

    return buffer;
  }

}
