set(CMAKE_CXX_FLAGS "-fpermissive")
set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11

include_directories(.)
add_subdirectory(Core)
add_subdirectory(SkyGB)
