set(SRCS SkyGB.cpp)
set(HEADERS )

add_executable(SkyGB ${SRCS} ${HEADERS})

target_link_libraries(SkyGB core)

IF (WIN32)
    set(SDL2_INCLUDE_DIR ../../externals/SDL2/include)
    set(SDL2_LIBRARY ../../../externals/SDL2/lib/x86)
ELSE()
    MESSAGE("TODO: linking of SDL on other platforms")
ENDIF()

target_include_directories(SkyGB PUBLIC ${SDL2_INCLUDE_DIR})
target_link_libraries(SkyGB ${SDL2_LIBRARY}/SDL2 ${SDL2_LIBRARY}/SDL2main)   

IF (WIN32)
    configure_file(../../externals/SDL2/lib/x86/SDL2.dll ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/SDL2.dll COPYONLY)
ENDIF()