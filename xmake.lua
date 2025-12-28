set_project("akagoria")
set_version("0.1.0")

add_repositories("gf-repo https://github.com/GamedevFramework/xmake-repo")

-- set_policy("package.install_locally", true)
add_requires("gamedevframework2")
add_requires("nlohmann_json")

add_rules("mode.debug", "mode.releasedbg", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "$(builddir)"})

if is_mode("sanitizers") then
    set_symbols("debug")
    set_optimize("none")
    set_policy("build.sanitizer.address", true)
    set_policy("build.sanitizer.undefined", true)
end

set_policy("build.warning", true)
set_warnings("allextra")
set_languages("cxx17")
set_encodings("utf-8")

if is_plat("windows") then
  add_cxflags("/wd4251") -- Disable warning: class needs to have dll-interface to be used by clients of class blah blah blah
  add_defines("NOMINMAX", "_CRT_SECURE_NO_WARNINGS")
end

set_configdir("$(builddir)/config")
set_configvar("AKAGORIA_DATADIR", "$(projectdir)/data")
set_configvar("AKAGORIA_LOCALEDIR", "$(projectdir)/data/raw/i18n")
add_configfiles("code/config.h.in", {pattern = "@(.-)@"})

target("akagoria")
    set_kind("binary")
    add_files("code/akagoria.cc")
    add_files("code/bits/*.cc")
    add_files("code/bits/ui/*.cc")
    add_files("code/vendor/agate/agate.c")
    add_includedirs("code/vendor/agate")
    add_includedirs("$(builddir)/config")
--     add_packages("nlohmann_json")
    add_packages("gamedevframework2", { components = { "framework", "physics", "imgui" } })
--     add_packages("boost", "icu4c")

target("akagoria-compile-data")
    set_kind("binary")
    add_files("code/akagoria-compile-data.cc")
    add_files("code/bits/WorldData.cc")
    add_files("code/bits/DataLabel.cc")
    add_defines("JSON_DIAGNOSTICS=1")
    add_includedirs("$(builddir)/config")
    add_packages("gamedevframework2", { components = { "core" } })
    add_packages("nlohmann_json")

target("akagoria-editor")
    set_kind("binary")
    add_files("code/akagoria-editor.cc")
    add_includedirs("$(builddir)/config")
    add_packages("gamedevframework2", { components = { "imgui", "graphics" } })


target("akagoria-tileset")
    set_kind("binary")
    add_files("tools/akagoria-tileset.cc")
    add_files("tools/bits/Tileset*.cc")
    add_includedirs("$(builddir)/config")
    add_packages("gamedevframework2", { components = { "framework", "imgui" } })
    add_packages("nlohmann_json")
    set_rundir("$(projectdir)")
