set(SRCS SkyGB.cpp)
set(HEADERS )

add_executable(SkyGB ${SRCS} ${HEADERS})

target_link_libraries(SkyGB core)

IF (WIN32)
    set(SDL2_INCLUDE_DIR ../../externals/SDL2/include)
    set(SDL2_LIBRARY ../../../externals/SDL2/lib/x86)
ELSE()
    INCLUDE(FindPkgConfig)
    PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
    find_package(OpenGL REQUIRED)
    MESSAGE(${SDL2_INCLUDE_DIRS})
    MESSAGE(${SDL2_LIBRARIES})
    set(SDL2_INCLUDE_DIR ${SDL2_INCLUDE_DIRS})
ENDIF()

target_include_directories(SkyGB PUBLIC ${SDL2_INCLUDE_DIR})

IF (WIN32)
    target_link_libraries(SkyGB ${SDL2_LIBRARY}/SDL2 ${SDL2_LIBRARY}/SDL2main)   
ELSE()
    target_link_libraries(SkyGB ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES})
ENDIF()   

IF (WIN32)
    configure_file(../../externals/SDL2/lib/x86/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/SDL2.dll COPYONLY)
ENDIF()