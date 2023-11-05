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
  add_defines("BUILD_PIGEON")
  set_kind(get_config("kind"))
  add_files("src/**.cpp")
  add_includedirs("src", {public = true})
  add_headerfiles("src/(**.hpp)")
target_end()

if get_config("examples") == true then
  target("basic_example")
    set_kind("binary")
    add_deps("pigeon_engine")
    add_files("examples/basic_example.cpp")
  target_end()
end

task("examples")
  on_run(function ()
    import("core.base.option")
    local example = option.get("target")
    os.exec("xmake config -m debug --test=no --examples=yes --kind=shared")
    os.exec("xmake build " .. example)
    os.exec("xmake run " .. example)
  end)
  set_menu {
    usage = "xmake examples -t [target]",
    description = "Run the given target example",
    options = {
      {
        "t", "target", "kv", "basic_example",
        "Set the example target.",
        "    - basic_example"
      }
    }
  }
task_end()

if get_config("test") == true then
  includes("packages/googletest.lua")
  add_requires("googletest 1.14.0", {configs = {main = true}})
  for _, file in ipairs(os.files("tests/*.cpp")) do
    target("test." .. path.basename(file))
      set_kind("binary")
      set_group("test")
      add_files(file)

      -- Why add ldflags? https://github.com/xmake-io/xmake/discussions/4332
      add_packages("googletest")
      if is_plat("windows") then
        add_ldflags("/subsystem:console")
      end
    target_end()
  end
end

task("test")
  on_run(function () 
    os.exec("xmake config -m debug --test=yes --examples=no --kind=shared")
    os.exec("xmake build -g test")
    os.exec("xmake run -g test")
  end)
  set_menu {
    usage = "xmake test",
    description = "Run all unittest"
  }
task_end()
