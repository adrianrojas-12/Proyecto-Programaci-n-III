#ifndef BUSCADOR_PARALELO_H
#define BUSCADOR_PARALELO_H
#include "buscador.h"
#include <vector>
#include <string>
std::vector<ResultadoBusqueda> buscarParalelo(const SuffixTrie& trie,
    const std::vector<Pelicula>& peliculas, const std::string& consulta, unsigned hilos = 4);
#endif
