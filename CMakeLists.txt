cmake_minimum_required(VERSION 3.20)
project(Proyecto_Programacion_III)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

# Busca automaticamente Python en la PC de cada integrante del grupo
set(PYBIND11_FINDPYTHON ON)
find_package(Python COMPONENTS Interpreter Development REQUIRED)

# Descarga automatica de pybind11 al recargar CMake
include(FetchContent)
FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG        v2.13.6
)
FetchContent_MakeAvailable(pybind11)

set(NUCLEO_BACKEND
        cargador_csv.cpp
        preprocesador.cpp
        trie.cpp
        buscador.cpp
        ranking.cpp
        buscador_paralelo.cpp
        benchmark.cpp
        patrones.cpp
        patrones.h
        persistencia.cpp
        recomendador.cpp
)

# 1. Target original: Consola (.exe)
add_executable(Proyecto_Programacion_III main.cpp interfaz.cpp ${NUCLEO_BACKEND})
target_link_libraries(Proyecto_Programacion_III PRIVATE Threads::Threads)

# 2. Target nuevo: Libreria para Python (.pyd en Windows / .so en Linux/Mac)
pybind11_add_module(motor_peliculas bindings_python.cpp ${NUCLEO_BACKEND})
target_link_libraries(motor_peliculas PRIVATE Threads::Threads)

# Deja el archivo compilado directamente dentro de gui_python/
set_target_properties(motor_peliculas PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/gui_python"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/gui_python"
)

if(MINGW)
    target_link_options(motor_peliculas PRIVATE -static -static-libgcc -static-libstdc++)
endif()