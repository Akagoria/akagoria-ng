#include "Script.h"

#include <cassert>
#include <ctime>

#include <fstream>

#include <gf2/core/Id.h>

#include "Akagoria.h"
#include "QuestState.h"
#include "agate.h"

namespace akgr {

  namespace {
    constexpr const char* Unit = "Adventure";

    std::string load_file(const std::filesystem::path& filename)
    {
       std::ifstream file(filename);

      if (!file) {
        gf::Log::fatal("File for script does not exist: '{}'", filename.string());
      }

      std::string content;

      for (std::string line; std::getline(file, line); ) {
        content.append(line);
        content.append(1, '\n');
      }

      return content;
    }

    const char *vm_unit_load(const char* name, void* user_data) {
      auto* script = static_cast<Script *>(user_data);
      return script->load_module(name);
    }

    AgateUnitHandler vm_unit_handler(AgateVM* vm, [[maybe_unused]] const char* name) {
      AgateUnitHandler handler;
      handler.load = vm_unit_load;
      handler.release = nullptr;
      handler.user_data = agateGetUserData(vm);
      return handler;
    }

    AgateForeignMethodFunc vm_foreign_method_handler([[maybe_unused]] AgateVM* vm, [[maybe_unused]] const char* unit_name, [[maybe_unused]] const char* class_name, [[maybe_unused]] AgateForeignMethodKind kind, const char* signature) {
      assert(std::strcmp(unit_name, "world") == 0);
      assert(std::strcmp(class_name, "World") == 0);
      assert(kind == AGATE_FOREIGN_METHOD_CLASS);
      using namespace gf::literals;

      switch (gf::hash_string(signature)) {
        case "move_hero(_)"_id:
          return &Script::move_hero;
        case "move_hero_down()"_id:
          return &Script::move_hero_down;
        case "move_hero_up()"_id:
          return &Script::move_hero_up;
        case "post_notification(_)"_id:
          return &Script::post_notification;
        case "add_requirement(_)"_id:
          return &Script::add_requirement;
        case "remove_requirement(_)"_id:
          return &Script::remove_requirement;
        case "add_item(_,_,_)"_id:
          return &Script::add_item;
        // case "add_item_to_inventory(_)"_id:
        //   return &Script::addItemToInventory;
        // case "set_character_mood(_,_)"_id:
        //   return &Script::setCharacterMood;
        case "start_dialog(_)"_id:
          return &Script::start_dialog;
        case "add_character(_,_,_,_)"_id:
          return &Script::add_character;
        case "add_dialog_to_character(_,_)"_id:
          return &Script::add_dialog_to_character;
        case "start_quest(_)"_id:
          return &Script::start_quest;
        default:
          break;
      }

      return &Script::not_implemented;
    }

    void vm_print([[maybe_unused]] AgateVM* vm, const char* text) {
      if (std::strcmp(text, "\n") == 0) {
        return;
      }

      gf::Log::info("{}", text);
    }

    const char* vm_error_to_string(AgateErrorKind kind) {
      switch (kind) {
        case AGATE_ERROR_COMPILE:
          return "Compile error";
        case AGATE_ERROR_RUNTIME:
          return "Runtime error";
        case AGATE_ERROR_STACKTRACE:
          return "Stack";
      }

      assert(false);
      return "???";
    }

    void vm_error([[maybe_unused]] AgateVM* vm, AgateErrorKind kind, const char* unit_name, int line, const char* message) {
      gf::Log::error("{}:{} [{}] {}", unit_name == nullptr ? "<main>" : unit_name, line, vm_error_to_string(kind), message);
    }

    template<typename T>
    void check_reference(const DataReference<T>& data, const char* name) {
      if (data.origin == nullptr) {
        gf::Log::error("A binding failed. Reference was '{}'", name);
        std::abort();
      }
    }

  }

  Script::Script(Akagoria* game)
  : m_game(game)
  {
  }

  Script::~Script()
  {
    for (auto* handle : { m_method_on_dialog, m_method_on_message, m_method_on_quest, m_method_on_quest_step, m_method_start, m_method_initialize, m_class_adventure }) {
      if (handle != nullptr) {
        agateReleaseHandle(m_vm, handle);
      }
    }

    if (m_vm != nullptr) {
      agateDeleteVM(m_vm);
    }
  }

  void Script::bind()
  {
    if (m_vm != nullptr) {
      return;
    }

    const char* script = load_module("adventure");

    AgateConfig configuration;
    agateConfigInitialize(&configuration);

    configuration.unit_handler = vm_unit_handler;
    configuration.foreign_method_handler = vm_foreign_method_handler;

    configuration.print = vm_print;
    configuration.error = vm_error;

    configuration.user_data = this;

    m_vm = agateNewVM(&configuration);

    AgateStatus status = agateCallString(m_vm, Unit, script);

    if (status != AGATE_STATUS_OK) {
      gf::Log::fatal("Could not load the main script.");
    }

    agateStackStart(m_vm);
    ptrdiff_t adventure_slot = agateSlotAllocate(m_vm);
    agateGetVariable(m_vm, Unit, "Adventure", adventure_slot);
    m_class_adventure = agateSlotGetHandle(m_vm, adventure_slot);
    agateStackFinish(m_vm);
    assert(m_class_adventure);

    m_method_initialize = agateMakeCallHandle(m_vm, "initialize()");
    assert(m_method_initialize);
    m_method_start = agateMakeCallHandle(m_vm, "start()");
    assert(m_method_start);
    m_method_on_message = agateMakeCallHandle(m_vm, "on_message(_)");
    assert(m_method_on_message);
    m_method_on_dialog = agateMakeCallHandle(m_vm, "on_dialog(_)");
    assert(m_method_on_dialog);
    m_method_on_quest = agateMakeCallHandle(m_vm, "on_quest(_)");
    assert(m_method_on_quest);
    m_method_on_quest_step = agateMakeCallHandle(m_vm, "on_quest_step(_,_)");
    assert(m_method_on_quest_step);
  }

  const char* Script::load_module(std::filesystem::path name)
  {
    auto path = m_game->resource_manager()->search("scripts" / name.replace_extension(".agate"));
    auto script = load_file(path);
    m_sources.push_back(std::move(script));
    return m_sources.back().c_str();
  }

  void Script::initialize()
  {
    gf::Log::info("[SCRIPT] Adventure.initialize()");

    agateStackStart(m_vm);
    ptrdiff_t arg0 = agateSlotAllocate(m_vm);
    agateSlotSetHandle(m_vm, arg0, m_class_adventure);
    AgateStatus result = agateCallHandle(m_vm, m_method_initialize);
    agateStackFinish(m_vm);

    if (result != AGATE_STATUS_OK) {
      gf::Log::error("Could not execute 'Adventure.initialize()'");
    }
  }

  void Script::start()
  {
    gf::Log::info("[SCRIPT] Adventure.start()");

    agateStackStart(m_vm);
    ptrdiff_t arg0 = agateSlotAllocate(m_vm);
    agateSlotSetHandle(m_vm, arg0, m_class_adventure);
    AgateStatus result = agateCallHandle(m_vm, m_method_start);
    agateStackFinish(m_vm);

    if (result != AGATE_STATUS_OK) {
      gf::Log::error("Could not execute 'Adventure.start()'\n");
    }
  }

  void Script::on_message(const std::string& name)
  {
    gf::Log::info("[SCRIPT] Adventure.on_message({})", name);

    agateStackStart(m_vm);
    ptrdiff_t arg0 = agateSlotAllocate(m_vm);
    agateSlotSetHandle(m_vm, arg0, m_class_adventure);
    ptrdiff_t arg1 = agateSlotAllocate(m_vm);
    agateSlotSetString(m_vm, arg1, name.c_str());
    AgateStatus result = agateCallHandle(m_vm, m_method_on_message);
    agateStackFinish(m_vm);

    if (result != AGATE_STATUS_OK) {
      gf::Log::error("Could not execute 'Adventure.on_message(_)'");
    }
  }

  void Script::on_dialog(const std::string& name)
  {
    gf::Log::info("[SCRIPT] Adventure.on_dialog({})", name);

    agateStackStart(m_vm);
    ptrdiff_t arg0 = agateSlotAllocate(m_vm);
    agateSlotSetHandle(m_vm, arg0, m_class_adventure);
    ptrdiff_t arg1 = agateSlotAllocate(m_vm);
    agateSlotSetString(m_vm, arg1, name.c_str());
    AgateStatus result = agateCallHandle(m_vm, m_method_on_dialog);
    agateStackFinish(m_vm);

    if (result != AGATE_STATUS_OK) {
      gf::Log::error("Could not execute 'Adventure.on_dialog(_)'");
    }
  }

  void Script::on_quest(const std::string& name)
  {
    gf::Log::info("[SCRIPT] Adventure.on_quest({})", name);

    agateStackStart(m_vm);
    ptrdiff_t arg0 = agateSlotAllocate(m_vm);
    agateSlotSetHandle(m_vm, arg0, m_class_adventure);
    ptrdiff_t arg1 = agateSlotAllocate(m_vm);
    agateSlotSetString(m_vm, arg1, name.c_str());
    AgateStatus result = agateCallHandle(m_vm, m_method_on_quest);
    agateStackFinish(m_vm);

    if (result != AGATE_STATUS_OK) {
      gf::Log::error("Could not execute 'Adventure.on_quest(_)'");
    }
  }

  void Script::on_quest_step(const std::string& name, std::size_t step)
  {
    gf::Log::info("[SCRIPT] Adventure.on_quest_step({}, {})", name, step);

    agateStackStart(m_vm);
    ptrdiff_t arg0 = agateSlotAllocate(m_vm);
    agateSlotSetHandle(m_vm, arg0, m_class_adventure);
    ptrdiff_t arg1 = agateSlotAllocate(m_vm);
    agateSlotSetString(m_vm, arg1, name.c_str());
    ptrdiff_t arg2 = agateSlotAllocate(m_vm);
    agateSlotSetInt(m_vm, arg2, int64_t(step));
    AgateStatus result = agateCallHandle(m_vm, m_method_on_quest_step);
    agateStackFinish(m_vm);

    if (result != AGATE_STATUS_OK) {
      gf::Log::error("Could not execute 'Adventure.on_quest_step(_,_)'");
    }
  }

  void Script::on_message_deferred(std::string name)
  {
    m_messages.push(std::move(name));
  }

  void Script::handle_deferred_messages()
  {
    while (!m_messages.empty()) {
      on_message(m_messages.front());
      m_messages.pop();
    }
  }

  void Script::not_implemented([[maybe_unused]] AgateVM* vm) {
    // assert(false);
  }

  // move_hero(location)
  void Script::move_hero(AgateVM* vm)
  {
    const char* location_id = agateSlotGetString(vm, 1);

    gf::Log::info("[SCRIPT] World.move_hero({})", location_id);

    const LocationData* location = data_lexicon_find(data(vm).locations, gf::hash_string(location_id));
    assert(location);

    state(vm).hero.spot = location->spot;
    runtime(vm).physics.hero.set_spot(location->spot);

    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // move_hero_down()
  void Script::move_hero_down(AgateVM* vm)
  {
    gf::Log::info("[SCRIPT] World.move_hero_down()");

    auto& hero = state(vm).hero;
    hero.spot.floor -= 2;
    runtime(vm).physics.hero.set_spot(hero.spot);

    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // move_hero_up()
  void Script::move_hero_up(AgateVM* vm)
  {
    gf::Log::info("[SCRIPT] World.move_hero_up()");

    auto& hero = state(vm).hero;
    hero.spot.floor += 2;
    runtime(vm).physics.hero.set_spot(hero.spot);

    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // post_notification(notification)
  void Script::post_notification(AgateVM* vm)
  {
    const char* notification_id = agateSlotGetString(vm, 1);

    gf::Log::info("[SCRIPT] World.post_notification({})", notification_id);

    NotificationState notification;
    notification.data.id = gf::hash_string(notification_id);
    notification.data.bind_from(data(vm).notifications);
    check_reference(notification.data, notification_id);
    assert(notification.data.origin != nullptr);

    state(vm).notifications.push_back(notification);

    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // add_requirement(requirement)
  void Script::add_requirement(AgateVM* vm)
  {
    const char* requirement_id = agateSlotGetString(vm, 1);

    gf::Log::info("[SCRIPT] World.add_requirement({})", requirement_id);

    state(vm).hero.requirements.insert(gf::hash_string(requirement_id));
    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // remove_requirement(requirement)
  void Script::remove_requirement(AgateVM* vm)
  {
    const char* requirement_id = agateSlotGetString(vm, 1);

    gf::Log::info("[SCRIPT] World.remove_requirement({})", requirement_id);

    state(vm).hero.requirements.erase(gf::hash_string(requirement_id));
    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // add_character(name, character, location, rotation)
  void Script::add_character(AgateVM* vm)
  {
    const char *name = agateSlotGetString(vm, 1);
    const char *character_id = agateSlotGetString(vm, 2);
    const char *location_id = agateSlotGetString(vm, 3);
    double rotation = agateSlotGetFloat(vm, 4);

    gf::Log::info("[SCRIPT] World.add_character({}, {}, {}, {})", name, character_id, location_id, rotation);

    const LocationData* location = data_lexicon_find(data(vm).locations, gf::hash_string(location_id));
    assert(location);

    CharacterState character;
    character.name = name;
    character.data.id = gf::hash_string(character_id);
    character.data.bind_from(data(vm).characters);
    character.spot = location->spot;
    character.rotation = static_cast<float>(gf::degrees_to_radians(rotation));

    character.behavior = CharacterStayState{ character.spot.location };

    state(vm).characters.push_back(character);

    const gf::Id character_name_id = gf::hash_string(name);

    auto& physics_runtime = runtime(vm).physics;
    physics_runtime.characters.emplace(character_name_id, physics_runtime.create_character(character.spot, character.rotation));

    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // start_dialog(name)
  void Script::start_dialog(AgateVM* vm)
  {
    const char* dialog_id = agateSlotGetString(vm, 1);

    gf::Log::info("[SCRIPT] World.start_dialog({})", dialog_id);

    auto& dialog = state(vm).hero.dialog;
    dialog.data.id = gf::hash_string(dialog_id);
    dialog.data.bind_from(data(vm).dialogs);
    check_reference(dialog.data, dialog_id);
    assert(dialog.data.origin != nullptr);

    dialog.current_line = 0;

    game(vm).replace_scene(&game(vm).world_act()->dialog_scene);

    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // add_dialog_to_character(dialog, name)
  void Script::add_dialog_to_character(AgateVM* vm)
  {
    const char *dialog_id = agateSlotGetString(vm, 1);
    const char *character_name = agateSlotGetString(vm, 2);

    gf::Log::info("[SCRIPT] World.add_dialog_to_character({}, {})", dialog_id, character_name);

    for (auto& character : state(vm).characters) {
      if (character.name == character_name) {
        character.dialog.id = gf::hash_string(dialog_id);
        character.dialog.bind_from(data(vm).dialogs);
        assert(character.dialog.check());
        break;
      }
    }

    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // add_item(item, location, rotation)
  void Script::add_item(AgateVM* vm)
  {
    const char *item_id = agateSlotGetString(vm, 1);
    const char *location_id = agateSlotGetString(vm, 2);
    double rotation = agateSlotGetFloat(vm, 3);

    gf::Log::info("[SCRIPT] World.add_item({}, {}, {})", item_id, location_id, rotation);

    const LocationData* location = data_lexicon_find(data(vm).locations, gf::hash_string(location_id));
    assert(location);

    ItemState item;
    item.data.id = gf::hash_string(item_id);
    item.data.bind_from(data(vm).items);
    assert(item.data.check());

    item.spot = location->spot;
    item.rotation = static_cast<float>(gf::degrees_to_radians(rotation));

    state(vm).items.push_back(std::move(item));

    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }

  // start_quest(quest)
  void Script::start_quest(AgateVM* vm)
  {
    const char* quest_id = agateSlotGetString(vm, 1);

    gf::Log::info("[SCRIPT] World.start_quest({})", quest_id);

    QuestState quest;
    quest.data.id = gf::hash_string(quest_id);
    quest.data.bind_from(data(vm).quests);
    check_reference(quest.data, quest_id);
    quest.last_update = std::time(nullptr);
    quest.status = (quest.data->scope == QuestScope::History) ? QuestStatus::Visible : QuestStatus::Started;
    quest.reset_features();

    state(vm).hero.quests.push_back(quest);

    agateSlotSetNil(vm, AGATE_RETURN_SLOT);
  }


  Akagoria& Script::game(AgateVM* vm)
  {
    auto* script = static_cast<Script *>(agateGetUserData(vm));
    return *script->m_game;
  }

  const WorldData& Script::data(AgateVM* vm)
  {
    auto* script = static_cast<Script *>(agateGetUserData(vm));
    return script->m_game->world_model()->data;
  }

  WorldState& Script::state(AgateVM* vm)
  {
    auto* script = static_cast<Script *>(agateGetUserData(vm));
    return script->m_game->world_model()->state;
  }

  WorldRuntime& Script::runtime(AgateVM* vm)
  {
    auto* script = static_cast<Script *>(agateGetUserData(vm));
    return script->m_game->world_model()->runtime;
  }

}
