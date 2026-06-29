cmake_minimum_required(VERSION 3.14)
project(phonetic LANGUAGES CXX)

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
  GIT_TAG        main
)
FetchContent_MakeAvailable(strings)

# --- Library target ---
# phonetic is header-only: the encoders are a CRTP base plus per-language
# subclasses, all inline in headers. So the target is a pure INTERFACE library
# that just puts the source dir on the consumer's include path.
add_library(phonetic INTERFACE)
target_include_directories(phonetic INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(phonetic INTERFACE cxx_std_20)

# Link strings INTERFACE so "strings.hh" rides onto the consumer's include path:
# the French/German/Spanish headers are part of the public surface and include
# it themselves, so a consumer of those headers needs it resolvable too.
target_link_libraries(phonetic INTERFACE strings::strings)

# Alias so consumers can link `phonetic::phonetic` whether they add this via
# add_subdirectory or FetchContent.
add_library(phonetic::phonetic ALIAS phonetic)

# Only build the test and demo when this repo is the top-level project, so
# consumers (e.g. via FetchContent) don't build them.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  enable_testing()
  add_executable(phonetic_test test/test.cc)
  target_link_libraries(phonetic_test PRIVATE phonetic)
  add_test(NAME phonetic COMMAND phonetic_test)

  # A runnable tour of the library (not a test): build & run ./build/phonetic_demo
  add_executable(phonetic_demo examples/demo.cc)
  target_link_libraries(phonetic_demo PRIVATE phonetic)
endif()
