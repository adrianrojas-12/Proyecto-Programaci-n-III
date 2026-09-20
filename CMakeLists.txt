cmake_minimum_required(VERSION 3.20)
project(Proyecto_Programacion_III)

set(CMAKE_CXX_STANDARD 17)

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
    interfaz.h
    interfaz.cpp
    persistencia.cpp
    persistencia.h
    recomendador.cpp
    recomendador.h
)