set(target_name FileIO)

add_library(${target_name} STATIC
  FileIO/ImageIO.cpp
  FileIO/ImageIO.hpp
  FileIO/TextIO.cpp
  FileIO/TextIO.hpp
)

target_include_directories(
  ${target_name} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)


target_compile_definitions(
  ${target_name} PRIVATE 
  PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)