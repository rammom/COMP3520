cmake_minimum_required(VERSION 3.12)

project(SDL2TemplateCMake)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(main main.cpp)

#Default build is to enable all safe optimizations (-O3, LTO)
#If debugging needed, you can override this with
#cmake -D CMAKE_BUILD_TYPE=Debug ..

include(CheckIPOSupported)
# Optional LTO. Do not use LTO if it's not supported by compiler.
check_ipo_supported(RESULT result OUTPUT output)
if(result)
  set_property(TARGET main PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(WARNING "LTO is not supported: ${output}")
endif()

#If no build type set, default to Release build
set(default_build_type "Release")
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


target_include_directories(main
  PRIVATE   
  ${CMAKE_CURRENT_SOURCE_DIR}
  SYSTEM PUBLIC
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_TTF_INCLUDE_DIR}
)

target_compile_options(main
  PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall;-Werror>
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

target_compile_features(main PRIVATE cxx_std_17)

target_link_libraries(main
  PUBLIC
  ${SDL2_LIBRARIES}
  ${SDL2_TTF_LIBRARY}
)
