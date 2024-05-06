set_project("akagoria")
set_version("0.1.0")

add_repositories("gf-repo https://github.com/GamedevFramework/xmake-repo")

add_requires("gamedevframework2", { components = { "framework" }})

add_rules("mode.debug", "mode.releasedbg", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "$(buildir)"})

set_policy("build.warning", true)
set_warnings("allextra")
set_languages("cxx17")
set_encodings("utf-8")

if is_plat("windows") then
  add_cxflags("/wd4251") -- Disable warning: class needs to have dll-interface to be used by clients of class blah blah blah
  add_defines("NOMINMAX", "_CRT_SECURE_NO_WARNINGS")
end

set_configdir("$(buildir)/config")
set_configvar("AKAGORIA_DATADIR", "$(projectdir)/data/akagoria")
set_configvar("AKAGORIA_LOCALEDIR", "$(projectdir)/data/raw/i18n")
add_configfiles("code/config.h.in", {pattern = "@(.-)@"})

target("akagoria")
    set_kind("binary")
    add_files("code/akagoria.cc")
    add_files("code/bits/*.cc")
    add_includedirs("$(buildir)/config")
--     add_packages("nlohmann_json")
    add_packages("gamedevframework2")
--     add_packages("boost", "icu4c")


