##################################################################
#
#   To compile, set target manually below:
#
#       "auto"      Attempt to autodetect
#       "osx"       MacOS
#       "windows"   Windows
#       "linux"     Linux
#
##################################################################
##################################################################


set(EXPORT_TARGET "auto")


##################################################################
##################################################################
# set cmake version
cmake_minimum_required(VERSION 3.10)

# set the project name
project(reflect_demo)

# glob the files, add the executable
file(GLOB SOURCE_CODE_FILES 
    "demo/*.c**"
)
add_executable(${PROJECT_NAME} ${SOURCE_CODE_FILES})

# include directories
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/demo)
include_directories(${CMAKE_SOURCE_DIR}/include)

# determine target
if (EXPORT_TARGET MATCHES "auto")
    if     (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        set(EXPORT_TARGET "osx")
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(EXPORT_TARGET "windows")
    elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        set(EXPORT_TARGET "linux")
    endif()
endif()

# export target
if (EXPORT_TARGET MATCHES "osx")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_CXX_COMPILER "g++")

elseif (EXPORT_TARGET MATCHES "windows")

    ### TODO ###

elseif (EXPORT_TARGET MATCHES "linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_CXX_COMPILER "g++")

endif()




