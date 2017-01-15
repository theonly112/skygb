set(SRCS Cpu.cpp System.cpp Registers.cpp Memory.cpp Instruction.cpp Gpu.cpp Interrupts.cpp Display.cpp Input.cpp)
set(HEADERS Cpu.h System.h Registers.h Memory.h Instruction.h Defines.h Gpu.h Interrupts.h Display.h Cartrige.h Input.h)

IF (WIN32)
    set(CAPI Wrapper.h dllmain.cpp stdafx.h targetver.h)
ENDIF()

# Build a .lib
add_library(core STATIC ${SRCS} ${HEADERS})

IF (WIN32)
    set(SDL2_INCLUDE_DIR ../../externals/SDL2/include)
    set(SDL2_LIBRARY ../../../externals/SDL2/lib/x86)
ELSE()
    INCLUDE(FindPkgConfig)
    PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
    MESSAGE(${SDL2_INCLUDE_DIRS})
    MESSAGE(${SDL2_LIBRARIES})
    set(SDL2_INCLUDE_DIR ${SDL2_INCLUDE_DIRS})
ENDIF()

MESSAGE( STATUS "SDL_INCLUDE_DIR:         " ${SDL2_INCLUDE_DIR})
MESSAGE( STATUS "SDL2_LIBRARY:         " ${SDL2_LIBRARY})

target_include_directories(core PUBLIC ${SDL2_INCLUDE_DIR})

IF (WIN32)
    target_link_libraries(core ${SDL2_LIBRARY}/SDL2 ${SDL2_LIBRARY}/SDL2main)
ELSE()
    target_link_libraries(core ${SDL2_LIBRARIES})
ENDIF()

#Build a .dll
add_library(SkyGBCore SHARED ${CAPI} ${SRCS} ${HEADERS})
target_include_directories(SkyGBCore PUBLIC ${SDL2_INCLUDE_DIR})

IF (WIN32)
    target_link_libraries(SkyGBCore ${SDL2_LIBRARY}/SDL2 ${SDL2_LIBRARY}/SDL2main)
ELSE()
    target_link_libraries(SkyGBCore ${SDL2_LIBRARIES})
ENDIF()   