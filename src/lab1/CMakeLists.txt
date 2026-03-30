set(target_name container)

add_library(${target_name} STATIC
  lab1/Container.cpp
  lab1/Container.hpp)

target_include_directories(
  ${target_name}
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
