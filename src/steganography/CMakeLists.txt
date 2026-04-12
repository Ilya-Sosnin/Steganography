set(target_name ContainerProcessor)

add_library(${target_name} STATIC
  steganography/ContainerProcessor.cpp
  steganography/ContainerProcessor.hpp
  steganography/ImageIO.cpp
  steganography/ImageIO.hpp
  steganography/TextIO.cpp
  steganography/TextIO.hpp
  )

target_include_directories(
  ${target_name} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)


target_compile_definitions(
  ${target_name} PRIVATE 
  PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)