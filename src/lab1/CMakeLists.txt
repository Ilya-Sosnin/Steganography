set(target_name ContainerProcessor)

add_library(${target_name} STATIC
  lab1/ContainerProcessor.cpp
  lab1/ContainerProcessor.hpp)

target_include_directories(
  ${target_name} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)


target_compile_definitions(
  ${target_name} PRIVATE 
  PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)