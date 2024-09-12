#include "DialogRenderer.h"

#include "Akagoria.h"

namespace akgr {

  namespace {

    gf::RichTextStyle compute_dialog_style()
    {
      gf::RichTextStyle style;
      return style;
    }

  }

  DialogRenderer::DialogRenderer(Akagoria* game, const WorldResources& resources)
  : m_game(game)
  , m_atlas({ 1024, 1024 }, game->render_manager())
  , m_style(compute_dialog_style())
  , m_default_speaker_text_data(resources.dialog_speaker_text.data)
  , m_speaker_text_entity(&m_atlas, resources.dialog_speaker_text, game->render_manager(), game->resource_manager())
  , m_speaker_background_shape(resources.dialog_speaker_background, game->render_manager(), game->resource_manager())
  , m_default_words_text_data(resources.dialog_words_text.data)
  , m_words_text_entity(&m_atlas, &m_style, resources.dialog_words_text, game->render_manager(), game->resource_manager())
  , m_words_background_shape(resources.dialog_words_background, game->render_manager(), game->resource_manager())
  {
    static constexpr gf::Vec2F SpeakerBackgroundLocation = { 420.0f, 690.0f };
    static constexpr gf::Vec2F SpeakerBackgroundOrigin = { 0.0f, 0.5f };
    static constexpr gf::Vec2F SpeakerLocation = { 440.0f, 690.0f };
    static constexpr gf::Vec2F SpeakerOrigin = { 0.0f, 0.5f };

    static constexpr gf::Vec2F WordsBackgroundLocation = { 800.0f, 700.0f };
    static constexpr gf::Vec2F WordsBackgroundOrigin = { 0.5f, 0.0f };
    static constexpr gf::Vec2F WordsLocation = { 800.0f, 725.0f };
    static constexpr gf::Vec2F WordsOrigin = { 0.5f, 0.0f };

    m_speaker_background_shape.set_location(SpeakerBackgroundLocation);
    m_speaker_background_shape.set_origin(SpeakerBackgroundOrigin);

    m_speaker_text_entity.set_location(SpeakerLocation);
    m_speaker_text_entity.set_origin(SpeakerOrigin);

    m_words_background_shape.set_location(WordsBackgroundLocation);
    m_words_background_shape.set_origin(WordsBackgroundOrigin);

    m_words_text_entity.set_location(WordsLocation);
    m_words_text_entity.set_origin(WordsOrigin);
  }

  void DialogRenderer::update(gf::Time time)
  {
    const auto& dialog = m_game->world_state()->hero.dialog;

    if (!dialog.data) {
      return;
    }

    if (dialog.data->label.id != m_last_dialog_id || dialog.current_line != m_last_line) {
      assert(dialog.current_line < dialog.data->content.size());
      const DialogLine& line = dialog.data->content[dialog.current_line];

      gf::TextData speaker_data = m_default_speaker_text_data;
      speaker_data.content = line.speaker;
      m_speaker_text_entity.text().update(speaker_data, m_game->render_manager());

      gf::TextData words_data = m_default_words_text_data;
      words_data.content = line.words;
      m_words_text_entity.text().update(words_data, m_game->render_manager());

      m_last_dialog_id = dialog.data->label.id;
      m_last_line = dialog.current_line;
    }

  }

  void DialogRenderer::render(gf::RenderRecorder& recorder)
  {
    const auto& dialog = m_game->world_state()->hero.dialog;

    if (!dialog.data) {
      return;
    }

    m_words_background_shape.render(recorder);
    m_words_text_entity.render(recorder);
    m_speaker_background_shape.render(recorder);
    m_speaker_text_entity.render(recorder);
  }

}
