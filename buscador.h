#ifndef PROYECTO_PROGRAMACI_N_III_BUSCADOR_H
#define PROYECTO_PROGRAMACI_N_III_BUSCADOR_H

#include <iostream>
#include <string>
#include <vector>
#include "preprocesador.h"
#include "trie.h"
using namespace std;

struct ResultadoBusqueda {
    int id;
    double score;
};

vector<string> dividir_consulta(const string& consulta);

vector<ResultadoBusqueda> buscar(
    const SuffixTrie& trie,
    const vector<Pelicula>& peliculas,
    const string& consulta
);

void mostrar_resultados(
    const vector<ResultadoBusqueda>& resultados,
    const vector<Pelicula>& peliculas,
    int desde,
    int cantidad
);

#endif //PROYECTO_PROGRAMACI_N_III_BUSCADOR_H