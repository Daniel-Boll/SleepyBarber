set_languages("c++20")
add_rules("mode.debug", "mode.release")

local libs = { "fmt" }

add_requires(table.unpack(libs))

add_defines("DEBUG")
add_cflags("-O0")

target("working_shop")
  set_kind("binary")
  add_files("src/working.cpp")
  add_packages(table.unpack(libs))

target("non_working_shop")
  set_kind("binary")
  add_files("src/non-working.cpp")
  add_packages(table.unpack(libs))
