cmake_minimum_required(VERSION 3.20)
project(Proyecto_Programacion_III)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Proyecto_Programacion_III
        main.cpp
        cargador_csv.cpp
        cargador_csv.h
        preprocesador.cpp
        preprocesador.h
        trie.cpp
        trie.h
        buscador.cpp
        buscador.h
        ranking.cpp
        ranking.h
        interfaz.cpp
        interfaz.h
        persistencia.cpp
        persistencia.h
        recomendador.cpp
        recomendador.h
)