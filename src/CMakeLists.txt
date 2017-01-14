include_directories(.)
add_subdirectory(Core)
add_subdirectory(SkyGB)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c++11)
endif()