cmake_minimum_required(VERSION 3.16)
project(Calculator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Папка, куда буду собираться все плагины
set(PLUGIN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/plugins)

# Плагин sin
add_library(sin MODULE plugins/sin/sin_plugin.cpp)
target_include_directories(sin PRIVATE src)
target_compile_definitions(sin PRIVATE PLUGIN_EXPORTS) # Флаг, изменяющий поведение plugin_api.h в зависимости от того, кто его компилирует.
set_target_properties(sin PROPERTIES
    PREFIX ""
    OUTPUT_NAME "sin"
    LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR} # для DLL на Windows
    LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
)

# Плагин ln
add_library(ln MODULE plugins/ln/ln_plugin.cpp)
target_include_directories(ln PRIVATE src)
target_compile_definitions(ln PRIVATE PLUGIN_EXPORTS)
set_target_properties(ln PROPERTIES
    PREFIX ""
    OUTPUT_NAME "ln"
    LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PLUGIN_OUTPUT_DIR}
)

# Исполняемый файл (без линковки plugins т.к они будут загружаться в Runtime через LoadLibraryW)
add_executable(calc
    src/main.cpp
    src/parser.cpp
    src/plugin_manager.cpp
)
target_include_directories(calc PRIVATE src)

# После сборки копируем плагины в папку plugins рядом с exe
add_custom_command(TARGET calc POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:calc>/plugins
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PLUGIN_OUTPUT_DIR} $<TARGET_FILE_DIR:calc>/plugins
    COMMENT "Copying plugins next to the executable"
)