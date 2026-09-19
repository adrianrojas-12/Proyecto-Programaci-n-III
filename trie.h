#ifndef TRIE_H
#define TRIE_H

#include <vector>
#include <string>
#include <unordered_map>
#include "preprocesador.h"

using namespace std;

struct NodoTrie {
    // Dinámico: solo ocupa memoria para los hijos que realmente existen
    unordered_map<char, NodoTrie*> hijos;

    bool esFinDePalabra = false;

    // id -> frecuencia de apariciones en esa película
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
    int largoMinimoSufijo; // Optimización de volumen de datos

    void insertarSufijo(NodoTrie* nodo, const string& sufijo, int idPelicula);

public:
    // Por defecto se filtran sufijos menores a 3 letras
    SuffixTrie(int minSufijoLen = 3);
    ~SuffixTrie();

    void insertarPalabra(const string& palabra, int idPelicula);
    void construirIndice(const vector<Pelicula>& peliculas);

    NodoTrie* getRaiz() const { return raiz; }
};

#endif // TRIE_H