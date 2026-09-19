#ifndef TRIE_H
#define TRIE_H

#include <vector>
#include <string>
#include <unordered_map>
#include "preprocesador.h"

using namespace std;

struct NodoTrie {
    unordered_map<char, NodoTrie*> hijos;
    bool esFinDePalabra = false;
    unordered_map<int, int> frecuencias;

    ~NodoTrie() {
        for (auto& par : hijos) {
            delete par.second;
        }
    }
};

class SuffixTrie {
private:
    NodoTrie* raiz;
    int largoMinimoSufijo;

    void insertarSufijo(NodoTrie* nodo, const string& sufijo, int idPelicula);

public:
    SuffixTrie(int minSufijoLen = 3);
    ~SuffixTrie();

    void insertarPalabra(const string& palabra, int idPelicula);
    void construirIndice(const vector<Pelicula>& peliculas);

    NodoTrie* navegarPrefijo(const string& prefijo) const;

    // Método que requiere buscador.cpp para recolectar frecuencias recursivamente
    void recolectarCoincidencias(NodoTrie* nodo, unordered_map<int, int>& coincidencias) const;

    NodoTrie* getRaiz() const { return raiz; }
};

#endif // TRIE_H