set(target_name image_processor)

add_library(${target_name} STATIC
  lab1/ImageProcessor.cpp
  lab1/ImageProcessor.hpp)

target_include_directories(
  ${target_name}
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
