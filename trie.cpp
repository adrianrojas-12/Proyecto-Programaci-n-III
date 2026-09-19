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

    insertarSufijo(raiz, palabra, idPelicula);

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

NodoTrie* SuffixTrie::navegarPrefijo(const string& prefijo) const {
    NodoTrie* nodoActual = raiz;
    for (char c : prefijo) {
        if (nodoActual->hijos.find(c) == nodoActual->hijos.end()) {
            return nullptr;
        }
        nodoActual = nodoActual->hijos.at(c);
    }
    return nodoActual;
}

void SuffixTrie::recolectarCoincidencias(NodoTrie* nodo, unordered_map<int, int>& coincidencias) const {
    if (!nodo) return;

    if (nodo->esFinDePalabra) {
        for (const auto& par : nodo->frecuencias) {
            coincidencias[par.first] += par.second;
        }
    }

    for (const auto& par : nodo->hijos) {
        recolectarCoincidencias(par.second, coincidencias);
    }
}