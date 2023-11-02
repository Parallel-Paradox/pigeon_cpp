set_project("pigeon_engine")
set_version("0.1.0")
set_xmakever("2.8.3")

set_languages("cxx20")
set_warnings("all", "error")

option("test")
    set_default(false)
    set_description("Enable unittest")
option_end()

option("examples")
    set_default(false)
    set_description("Enable examples")
option_end()

add_rules("mode.debug", "mode.release")

if is_config("kind", "shared") then
    add_defines("PIGEON_SHARED")
end

target("pigeon_engine")
    set_kind(get_config("kind"))
    add_files("src/*.cpp")
target_end()

if get_config("examples") == true then
    target("basic_example")
        set_kind("binary")
        add_deps("pigeon_engine")
        add_files("examples/basic_example.cpp")
    target_end()
end

if get_config("test") == true then
    -- TODO: add test target
end
