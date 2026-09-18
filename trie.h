#ifndef TRIE_H
#define TRIE_H

#include <vector>
#include <string>
#include <unordered_map>
#include "preprocesador.h"

using namespace std;

struct NodoTrie {
    // 36 hijos: a-z (0-25) y 0-9 (26-35)
    NodoTrie* hijos[36] = {nullptr};

    bool esFinDePalabra = false;

    // id -> frecuencia de apariciones en esa película
    unordered_map<int, int> frecuencias;

    ~NodoTrie() {
        for (int i = 0; i < 36; ++i) {
            delete hijos[i];
        }
    }
};

class SuffixTrie {
private:
    NodoTrie* raiz;
    int largoMinimoSufijo; // Evita explotar la memoria RAM

    int obtenerIndice(char c) const;
    void insertarSufijo(NodoTrie* nodo, const string& sufijo, int idPelicula);

public:
    SuffixTrie(int minSufijoLen = 2);
    ~SuffixTrie();

    void insertarPalabra(const string& palabra, int idPelicula);
    void construirIndice(const vector<Pelicula>& peliculas);
    NodoTrie* navegarPrefijo(const string& prefijo) const;
    void recolectarCoincidencias(NodoTrie* nodo, unordered_map<int, int>& acumulado) const;
    NodoTrie* getRaiz() const { return raiz; }
};

#endif // TRIE_H