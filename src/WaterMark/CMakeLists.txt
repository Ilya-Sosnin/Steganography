set(target_name Mark)

add_library(${target_name} STATIC
  WaterMark/WaterMark.cpp
  WaterMark/WaterMark.hpp
)

target_link_libraries(
  ${target_name} PRIVATE
  FileIO
)

target_include_directories(
  ${target_name} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)


target_compile_definitions(
  ${target_name} PRIVATE 
  PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)