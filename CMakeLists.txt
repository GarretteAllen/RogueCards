cmake_minimum_required(VERSION 3.16)
project(RoguelikeDeckbuilder)

set(CMAKE_CXX_STANDARD 17)

# SDL2 Paths
set(SDL2_DIR "E:/Game Dev/Libraries/SDL2")
include_directories(${SDL2_DIR}/include)
link_directories(${SDL2_DIR}/lib/x64)

# Include all source directories
include_directories(src)

# Collect all source files
set(SOURCES
    src/main.cpp
    src/core/Game.cpp src/core/Game.h
    src/systems/InputManager.cpp src/systems/InputManager.h
    src/scenes/Scene.cpp src/scenes/Scene.h
    src/scenes/GameScene.cpp src/scenes/GameScene.h
    src/scenes/MenuScene.cpp src/scenes/MenuScene.h
    src/ui/Button.cpp src/ui/Button.h
    src/ui/Node.cpp src/ui/Node.h
    src/scenes/BattleScene.cpp src/scenes/BattleScene.h
    src/ui/Card.cpp src/ui/Card.h
    src/ui/CardEffect.h
    src/scenes/DeckSelectionScene.cpp src/scenes/DeckSelectionScene.h
)

# Define the executable
add_executable(RoguelikeDeckbuilder ${SOURCES})

# Link SDL2
target_link_libraries(RoguelikeDeckbuilder SDL2 SDL2main SDL2_ttf)

# Set output directory (optional, ensures consistency)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/x64-debug)

if(WIN32)
    set(SDL2_TTF_DLL "${SDL2_DIR}/lib/x64/SDL2_ttf.dll")
    add_custom_command(TARGET RoguelikeDeckbuilder POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_TTF_DLL}"
        $<TARGET_FILE_DIR:RoguelikeDeckbuilder>
        COMMENT "Copying SDL2_ttf.dll to output directory"
    )
    set(SDL2_DLL "${SDL2_DIR}/lib/x64/SDL2.dll")
    add_custom_command(TARGET RoguelikeDeckbuilder POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_DLL}"
        $<TARGET_FILE_DIR:RoguelikeDeckbuilder>
        COMMENT "Copying SDL2.dll to output directory"
    )
endif()

# Copy assets folder to output directory
add_custom_command(TARGET RoguelikeDeckbuilder POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    $<TARGET_FILE_DIR:RoguelikeDeckbuilder>/assets
    COMMENT "Copying assets folder to output directory"
)