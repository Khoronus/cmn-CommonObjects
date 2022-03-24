######################################################################
# The project name
cmake_minimum_required (VERSION 3.14)
project (CommonObjectSample)

# The version number
set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)
set (VERSION_SUBMINOR 0)
set (VERSION_PATCH 0)
set (VERSION_NAME \"CommonObjectsSample\")

######################################################################
# Import macro
include ("../cmakemacro/cmake_macro_setvariables.txt")

######################################################################
# Read the path configuration file (global)
# get the location of the global configuration path
file(READ "../configure/global_path_configuration.txt" pt_global)
string(REPLACE "\n" ";" PATH_GLOBAL_LIST ${pt_global})
GET_PATH("${PATH_GLOBAL_LIST}" "path_global " path_global)
set(PLATFORM_PATH "${path_global}")
message(STATUS "PLATFORM_PATH = (${PLATFORM_PATH})")
# get the location of the global configuration projects path
find_file (pt_location_global NAMES "global_projects_path_configuration.txt" REQUIRED 
  PATHS ${PLATFORM_PATH} 
  DOC "Path to global configuration paths.")
string(REPLACE "\n" ";" PATH_LOCATION_LIST ${pt_location_global})
message(STATUS "list = ${PATH_LOCATION_LIST}")
# read the configuration file global
file(READ "${pt_location_global}" pt_conf_global)
#string(REPLACE "\n" ";" PATH_GLOBAL_FULL_LIST ${pt_conf_global})
#message(STATUS "list = ${PATH_GLOBAL_FULL_LIST}")

######################################################################
# Read the path configuration file (local)
set(path_local_cfg "../configure/path_configuration.txt")
if (EXISTS ${path_local_cfg})
  file(READ ${path_local_cfg} pt_conf)
  message(STATUS "[+] open:${path_local_cfg}")
else()
  message(STATUS "[-] open:${path_local_cfg}")
endif()
#string(REPLACE "\n" ";" PATH_FULL_LIST ${pt_conf})
#message(STATUS "list = ${PATH_FULL_LIST}")

######################################################################
# Combine global and local
string(CONCAT MULTI ${pt_conf_global} ${pt_conf})
# remove all the new lines
string(REPLACE "\n" ";" PATH_FULL_LIST ${MULTI})
message(STATUS "list = ${PATH_FULL_LIST}")

######################################################################
# Get the first variable
GET_PATH("${PATH_FULL_LIST}" "path_opencv " path_opencv)
message(STATUS "path_opencv = (${path_opencv})")
GET_PATH("${PATH_FULL_LIST}" "path_boost " path_boost)
message(STATUS "path_boost = (${path_boost})")
GET_PATH("${PATH_FULL_LIST}" "path_boost_lib " path_boost_lib)
message(STATUS "path_boost_lib = (${path_boost_lib})")
GET_PATH("${PATH_FULL_LIST}" "path_storedata " path_storedata)
message(STATUS "path_storedata = (${path_storedata})")
GET_PATH("${PATH_FULL_LIST}" "path_common " path_common)
message(STATUS "path_common = (${path_common})")

######################################################################
# OpenCV
find_package(OpenCV REQUIRED PATHS ${path_opencv})
# Please set "target_link_libraries( ${PROJ_NAME}  ${OpenCV_LIBS})"

######################################################################
# Boost
SET(BOOST_ROOT ${path_boost})
SET(BOOST_LIBRARYDIR "${path_boost_lib}")
set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost REQUIRED COMPONENTS system thread regex filesystem random serialization)
message("Boost_LIBRARY_DIR: " ${Boost_LIBRARIES})

######################################################################
# Use record package
find_package(StoreData REQUIRED PATHS "${path_storedata}/build/install/cmake")

######################################################################
# Add Common Library

SET( PROJ_INCLUDES  
    ${path_common}
)

######################################################################
# Definitions

ADD_DEFINITIONS(-DUNICODE)
ADD_DEFINITIONS(-D_UNICODE)
ADD_DEFINITIONS(-DNOMINMAX)

######################################################################
# Recurse into the subdirectories. 
add_subdirectory (code)
