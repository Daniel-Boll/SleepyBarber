set_languages("c++20")
add_rules("mode.debug", "mode.release")

local libs = { "fmt" }

add_includedirs("include")
add_requires(table.unpack(libs))

target("sleepy_barber")
  set_kind("binary")
  add_files("src/*.cpp")
  add_packages(table.unpack(libs))
