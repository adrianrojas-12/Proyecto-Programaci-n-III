cmake_minimum_required(VERSION 3.20)
project(Proyecto_Programacion_III)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Threads REQUIRED)
set(CODIGO_BASE cargador_csv.cpp preprocesador.cpp trie.cpp buscador.cpp ranking.cpp buscador_paralelo.cpp benchmark.cpp
        patrones.cpp
        patrones.h
)
add_executable(Proyecto_Programacion_III main.cpp interfaz.cpp persistencia.cpp recomendador.cpp ${CODIGO_BASE}
        )
target_link_libraries(Proyecto_Programacion_III PRIVATE Threads::Threads)
