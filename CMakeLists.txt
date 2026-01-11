cmake_minimum_required(VERSION 3.10)
project(SyncHell)

set(CMAKE_CXX_STANDARD 17)

# Найти SFML
find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

# Найти libsndfile и FFTW
find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED sndfile)
pkg_check_modules(FFTW3F REQUIRED fftw3f)

# Найти OpenCL
find_package(OpenCL REQUIRED)
find_package(OpenMP REQUIRED)

# Исходные файлы, получить список можно скриптом src/scripts/getSources.sh, выполнять в корне проекта: sh src/scripts/getSources.sh
set(SOURCES
    src/analysis/Spectro.cpp
    src/analysis/STF.cpp
    src/analysis/STFGram.cpp
    src/collide.cpp
    src/game/cycle.cpp
    src/game/game_control.cpp
    src/game/game_init.cpp
    src/game/projectile_spawn.cpp
    src/main.cpp
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