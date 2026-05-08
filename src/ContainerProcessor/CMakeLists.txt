set(target_name Container)

add_library(${target_name} STATIC
  ContainerProcessor/ContainerProcessor.cpp
  ContainerProcessor/ContainerProcessor.hpp
)

target_link_libraries(
  ${target_name} PUBLIC
  FileIO
)

target_include_directories(
  ${target_name} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)


target_compile_definitions(
  ${target_name} PUBLIC 
  PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)