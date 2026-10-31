cmake_minimum_required(VERSION 3.14)
project(soundex LANGUAGES CXX)

include(FetchContent)

# --- Dependency: strings ---
# The French, German, and Spanish encoders include "strings.hh" for
# strings::inplace_upper (their upper-casing step). It lives in the sibling
# strings library, pulled in here so the include resolves. We track its tip with
# GIT_TAG main, like the rest of the family. (The English encoder and the base
# soundex.h need nothing but std headers.)
FetchContent_Declare(
  strings
  GIT_REPOSITORY https://github.com/Kronuz/strings.git
  GIT_TAG        b42d3c4ce1130043a381e487ae0a693248c2c0f5
)
FetchContent_MakeAvailable(strings)

# --- Library target ---
# soundex is header-only: the encoders are a CRTP base plus per-language
# subclasses, all inline in headers. So the target is a pure INTERFACE library
# that just puts the source dir on the consumer's include path.
add_library(soundex INTERFACE)
target_include_directories(soundex INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(soundex INTERFACE cxx_std_20)

# Link strings INTERFACE so "strings.hh" rides onto the consumer's include path:
# the French/German/Spanish headers are part of the public surface and include
# it themselves, so a consumer of those headers needs it resolvable too.
target_link_libraries(soundex INTERFACE strings::strings)

# Alias so consumers can link `soundex::soundex` whether they add this via
# add_subdirectory or FetchContent.
add_library(soundex::soundex ALIAS soundex)

# Only build the test and demo when this repo is the top-level project, so
# consumers (e.g. via FetchContent) don't build them.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  enable_testing()
  add_executable(soundex_test test/test.cc)
  target_link_libraries(soundex_test PRIVATE soundex)
  add_test(NAME soundex COMMAND soundex_test)

  # Differential test of SoundexEnglish against a frozen copy of the original.
  add_executable(soundex_diff_test test/diff_test.cc)
  target_link_libraries(soundex_diff_test PRIVATE soundex)
  add_test(NAME soundex_diff COMMAND soundex_diff_test)

  # Benchmark against the frozen original (not part of ctest).
  add_executable(soundex_bench bench/bench.cc)
  target_link_libraries(soundex_bench PRIVATE soundex)

  # A runnable tour of the library (not a test): build & run ./build/soundex_demo
  add_executable(soundex_demo examples/demo.cc)
  target_link_libraries(soundex_demo PRIVATE soundex)
endif()
