#include "trie.h"
#include <iostream>
using namespace std;

SuffixTrie::SuffixTrie(int minSufijoLen) : largoMinimoSufijo(minSufijoLen) {
    raiz = new NodoTrie();
}

SuffixTrie::~SuffixTrie() {
    delete raiz;
}

void SuffixTrie::insertarSufijo(NodoTrie* nodoActual, const string& sufijo, int idPelicula) {
    for (char c : sufijo) {
        // Si la letra no existe en los hijos del nodo, se crea sobre la marcha
        if (nodoActual->hijos.find(c) == nodoActual->hijos.end()) {
            nodoActual->hijos[c] = new NodoTrie();
        }
        nodoActual = nodoActual->hijos[c];
    }

    nodoActual->esFinDePalabra = true;
    nodoActual->frecuencias[idPelicula]++;
}

void SuffixTrie::insertarPalabra(const string& palabra, int idPelicula) {
    int n = palabra.length();

    // Inserción de la palabra completa (siempre requerida para búsquedas exactas)
    insertarSufijo(raiz, palabra, idPelicula);

    // Inserción de sufijos filtrados por largo mínimo
    for (int i = 1; i < n; ++i) {
        if (n - i >= largoMinimoSufijo) {
            string sufijo = palabra.substr(i);
            insertarSufijo(raiz, sufijo, idPelicula);
        }
    }
}

void SuffixTrie::construirIndice(const vector<Pelicula>& peliculas) {
    cout << "Construyendo el Suffix Trie optimizado..." << endl;

    for (const auto& peli : peliculas) {
        for (const string& token : peli.tokens_combinados) {
            insertarPalabra(token, peli.id);
        }
    }
    
    cout << "Indice Suffix Trie construido exitosamente." << endl;
}