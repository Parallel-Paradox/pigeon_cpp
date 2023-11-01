set_languages("cxx20")

add_rules("mode.debug", "mode.release")

target("pigeon_engine")
    set_kind("shared")
    add_files("src/*.cpp")

target("basic_example")
    set_kind("binary")
    add_deps("pigeon_engine")
    add_files("examples/basic_example.cpp")
