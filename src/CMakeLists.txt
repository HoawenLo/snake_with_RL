cmake_minimum_required(VERSION 3.18)
project(Test)

# Set C++ standard
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# Find LibTorch
find_package(Torch REQUIRED)

# Add Raylib library
add_subdirectory(lib/raylib-master)

# Add your executable
add_executable(test main.cpp)

# Link LibTorch
target_link_libraries(test "${TORCH_LIBRARIES}")
set_property(TARGET test PROPERTY CXX_STANDARD 17)

if (MSVC)
  file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
  add_custom_command(TARGET test
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_if_different
                     ${TORCH_DLLS}
                     $<TARGET_FILE_DIR:test>)
endif (MSVC)

# Link Raylib
target_link_libraries(test raylib)

# Include Raylib headers
target_include_directories(test PRIVATE ${RAYLIB_INCLUDE_DIRS})
