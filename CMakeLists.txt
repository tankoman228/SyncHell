# Версия для линукса

cmake_minimum_required(VERSION 3.15)
project(SyncHell)

set(CMAKE_CXX_STANDARD 17)

# Найти SFML
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)
set(SFML_STATIC_LIBRARIES TRUE)

# Найти libsndfile и FFTW
find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED sndfile)
pkg_check_modules(FFTW3F REQUIRED fftw3f)

# Найти OpenCL
find_package(OpenCL REQUIRED)
find_package(OpenMP REQUIRED)

# Исходные файлы, получить список можно скриптом src/scripts/getSources.sh, выполнять в корне проекта: sh src/scripts/getSources.sh
set(SOURCES
    src/analysis/EIF.cpp
    src/analysis/Equalizer.cpp
    src/analysis/Spectro.cpp
    src/analysis/STF.cpp
    src/analysis/STFGram.cpp
    src/collide.cpp
    src/game/game_control.cpp
    src/game/game_cycle.cpp
    src/game/game_init.cpp
    src/game/game_projectiles_cycle.cpp
    src/game/game_visual_cycle.cpp
    src/game/projectiles/AbstractProjectile.cpp
    src/game/projectiles/FunctionsProjectile.cpp
    src/game/projectile_spawn.cpp
    src/game/projectiles/ProjectileAura.cpp
    src/game/projectiles/ProjectileDeathZone.cpp
    src/game/projectiles/ProjectileHealingLazer.cpp
    src/game/projectiles/ProjectileHexagon.cpp
    src/game/projectiles/ProjectileLazer.cpp
    src/game/projectiles/ProjectilePentagon.cpp
    src/game/projectiles/ProjectileRound.cpp
    src/game/projectiles/ProjectileSpiralMove.cpp
    src/game/projectiles/ProjectileTriangle.cpp
    src/game/projectiles_spawn/FeatureTriggerMode0.cpp
    src/game/projectiles_spawn/FeatureTriggerMode1.cpp
    src/game/projectiles_spawn/FeatureTriggerMode2.cpp
    src/game/projectiles_spawn/FeatureTriggerMode3.cpp
    src/game/projectiles_spawn/FeatureTriggerMode4.cpp
    src/game/projectiles_spawn/FeatureTriggerMode5.cpp
    src/game/projectiles_spawn/FeatureTriggerMode6.cpp
    src/game/projectiles_spawn/FeatureTriggerMode7.cpp
    src/main.cpp
    src/menu/sandbox/Cycle.cpp
    src/menu/sandbox/InitButtons.cpp
    src/menu/sandbox/InitDifficultyBlock.cpp
    src/menu/sandbox/InitLevelsCards.cpp
    src/menu/sandbox/InitLevelsList.cpp
    src/menu/sandbox/SandboxMenu.cpp
    src/menu/sandbox/TriggerSearch.cpp
)

# Заголовочные файлы
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${SNDFILE_INCLUDE_DIRS}
    ${FFTW3F_INCLUDE_DIRS}
    ${OpenCL_INCLUDE_DIRS}  # Добавить пути к заголовкам OpenCL
)

# Создать исполняемый файл
add_executable(${PROJECT_NAME} ${SOURCES})

# Привязываем OpenMP к цели
if(OpenMP_FOUND)
    message(STATUS "OpenMP поддержка включена")
endif()

# GUI biblioteka
find_package(TGUI REQUIRED)

# Подключить библиотеки
target_link_libraries(${PROJECT_NAME}
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    OpenMP::OpenMP_CXX
    tgui
    ${SNDFILE_LIBRARIES}
    ${FFTW3F_LIBRARIES}
    ${OpenCL_LIBRARIES}     # Добавить библиотеки OpenCL
    m  # математическая библиотека для Linux
)

# Копирование папки assets в директорию с исполняемым файлом при сборке
add_custom_command(TARGET SyncHell POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:SyncHell>/assets"
)
