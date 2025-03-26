cmake_minimum_required(VERSION 3.16)
project(RoguelikeDeckbuilder)

set(CMAKE_CXX_STANDARD 17)

# Enable verbose output for debugging
set(CMAKE_VERBOSE_MAKEFILE ON)

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
    src/scenes/RewardScene.cpp src/scenes/RewardScene.h
    src/systems/TextureManager.h
    src/common/Constants.h
    src/scenes/OptionsScene.cpp
)

# Define the executable
add_executable(RoguelikeDeckbuilder ${SOURCES})

# Link SDL2
target_link_libraries(RoguelikeDeckbuilder SDL2 SDL2main SDL2_ttf SDL2_image)

# Set output directory (optional, ensures consistency)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/x64-debug)

if(WIN32)
    # Copy SDL2.dll
    set(SDL2_DLL "${SDL2_DIR}/lib/x64/SDL2.dll")
    add_custom_command(TARGET RoguelikeDeckbuilder POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_DLL}"
        $<TARGET_FILE_DIR:RoguelikeDeckbuilder>
        COMMAND ${CMAKE_COMMAND} -E echo "Copied SDL2.dll to $<TARGET_FILE_DIR:RoguelikeDeckbuilder>"
        COMMENT "Copying SDL2.dll to output directory"
    )

    # Copy SDL2_ttf.dll
    set(SDL2_TTF_DLL "${SDL2_DIR}/lib/x64/SDL2_ttf.dll")
    add_custom_command(TARGET RoguelikeDeckbuilder POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_TTF_DLL}"
        $<TARGET_FILE_DIR:RoguelikeDeckbuilder>
        COMMAND ${CMAKE_COMMAND} -E echo "Copied SDL2_ttf.dll to $<TARGET_FILE_DIR:RoguelikeDeckbuilder>"
        COMMENT "Copying SDL2_ttf.dll to output directory"
    )

    # Copy SDL2_image.dll (corrected name)
    set(SDL2_IMAGE_DLL "${SDL2_DIR}/lib/x64/SDL2_image.dll") # Corrected from SDL_image.dll
    add_custom_command(TARGET RoguelikeDeckbuilder POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_IMAGE_DLL}"
        $<TARGET_FILE_DIR:RoguelikeDeckbuilder>
        COMMAND ${CMAKE_COMMAND} -E echo "Copied SDL2_image.dll to $<TARGET_FILE_DIR:RoguelikeDeckbuilder>"
        COMMENT "Copying SDL2_image.dll to output directory"
    )

    # Copy libpng16.dll (dependency of SDL2_image)
    set(LIBPNG_DLL "${SDL2_DIR}/lib/x64/libpng16.dll")
    add_custom_command(TARGET RoguelikeDeckbuilder POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${LIBPNG_DLL}"
        $<TARGET_FILE_DIR:RoguelikeDeckbuilder>
        COMMAND ${CMAKE_COMMAND} -E echo "Copied libpng16.dll to $<TARGET_FILE_DIR:RoguelikeDeckbuilder>"
        COMMENT "Copying libpng16.dll to output directory"
    )

    # Copy zlib1.dll (dependency of SDL2_image)
    set(ZLIB_DLL "${SDL2_DIR}/lib/x64/zlib1.dll")
    add_custom_command(TARGET RoguelikeDeckbuilder POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ZLIB_DLL}"
        $<TARGET_FILE_DIR:RoguelikeDeckbuilder>
        COMMAND ${CMAKE_COMMAND} -E echo "Copied zlib1.dll to $<TARGET_FILE_DIR:RoguelikeDeckbuilder>"
        COMMENT "Copying zlib1.dll to output directory"
    )
endif()

# Copy assets folder to output directory
add_custom_command(TARGET RoguelikeDeckbuilder POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    $<TARGET_FILE_DIR:RoguelikeDeckbuilder>/assets
    COMMAND ${CMAKE_COMMAND} -E echo "Copied assets folder to $<TARGET_FILE_DIR:RoguelikeDeckbuilder>/assets"
    COMMENT "Copying assets folder to output directory"
)