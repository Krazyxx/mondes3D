# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget ::glbinding)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Create imported target ::glbinding
add_library(::glbinding STATIC IMPORTED)

set_target_properties(::glbinding PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "\$<\$<NOT:\$<BOOL:OFF>>:GLBINDING_STATIC_DEFINE>;SYSTEM_LINUX;\$<\$<BOOL:>:EXTENSIVE_META>"
  INTERFACE_COMPILE_OPTIONS "-Wall;-Wextra;-Wunused;-Wreorder;-Wignored-qualifiers;-Wmissing-braces;-Wreturn-type;-Wswitch;-Wswitch-default;-Wuninitialized;-Wmissing-field-initializers;\$<\$<CXX_COMPILER_ID:GNU>:;-Wmaybe-uninitialized;\$<\$<VERSION_GREATER:\$<CXX_COMPILER_VERSION>,4.8>:;-Wpedantic;-Wreturn-local-addr;>;>;\$<\$<CXX_COMPILER_ID:Clang>:;-Wpedantic;-Wreturn-stack-address;>;\$<\$<PLATFORM_ID:Darwin>:;-pthread;>"
  INTERFACE_INCLUDE_DIRECTORIES "/net/cremi/jimgouraud/mondes3D/mds3d_td4_opengl/ext/glbinding/include;/net/cremi/jimgouraud/mondes3D/mds3d_td4_opengl/build/ext/glbinding/include"
  INTERFACE_LINK_LIBRARIES "/usr/lib/x86_64-linux-gnu/libGLU.so;/usr/lib/x86_64-linux-gnu/libGL.so;-pthread"
)

# Import target "::glbinding" for configuration ""
set_property(TARGET ::glbinding APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(::glbinding PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "/net/cremi/jimgouraud/mondes3D/mds3d_td4_opengl/build/ext/glbinding/libglbinding.a"
  )

# This file does not depend on other imported targets which have
# been exported from the same project but in a separate export set.

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)
