set(target_name app)

add_executable(
  ${target_name}
  app/main.cpp
)

set_target_properties(
  ${target_name} PROPERTIES 
  OUTPUT_NAME app
)

target_link_libraries(
  ${target_name}
  PRIVATE
  CLI11
  #Container
  Mark
)

target_compile_definitions(
  ${target_name} PUBLIC 
  PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)