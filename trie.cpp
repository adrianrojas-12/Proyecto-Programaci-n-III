#include "trie.h"
#include <iostream>
#include <vector>

using namespace std;

SuffixTrie::SuffixTrie(int minSufijoLen) : largoMinimoSufijo(minSufijoLen) {
    raiz = new NodoTrie();
}

SuffixTrie::~SuffixTrie() {
    delete raiz; // Libera toda la memoria en cascada desde NodoTrie
}

void SuffixTrie::insertarSufijoDesde(NodoTrie* nodoActual, string_view palabra, size_t start_idx, int idPelicula) {
    for (size_t i = start_idx; i < palabra.length(); ++i) {
        char c = palabra[i];

        // Optimización: 1 sola búsqueda en el mapa usando referencias
        auto& hijo = nodoActual->hijos[c];
        if (!hijo) {
            hijo = new NodoTrie();
        }
        nodoActual = hijo;
    }

    nodoActual->esFinDePalabra = true;
    nodoActual->frecuencias[idPelicula]++;
}

void SuffixTrie::insertarPalabra(string_view palabra, int idPelicula) {
    size_t n = palabra.length();
    if (n == 0) return;

    // 1. Insertar la palabra completa
    insertarSufijoDesde(raiz, palabra, 0, idPelicula);

    // 2. Insertar sufijos omitiendo asignaciones de string
    for (size_t i = 1; i < n; ++i) {
        if (static_cast<int>(n - i) >= largoMinimoSufijo) {
            insertarSufijoDesde(raiz, palabra, i, idPelicula);
        }
    }
}

void SuffixTrie::construirIndice(const vector<Pelicula>& peliculas) {
    cout << "Construyendo el Suffix Trie optimizado..." << endl;

    for (const auto& peli : peliculas) {
        for (const string& token : peli.tokens_txt) {
            insertarPalabra(token, peli.id);
        }
    }

    cout << "Indice Suffix Trie construido exitosamente." << endl;
}

NodoTrie* SuffixTrie::navegarPrefijo(string_view prefijo) const {
    NodoTrie* nodoActual = raiz;
    for (char c : prefijo) {
        auto it = nodoActual->hijos.find(c);
        if (it == nodoActual->hijos.end()) {
            return nullptr;
        }
        nodoActual = it->second;
    }
    return nodoActual;
}

// Recolección iterativa mediante DFS (Evita stack overflow y acelera la búsqueda)
void SuffixTrie::recolectarCoincidencias(NodoTrie* nodoInicial, unordered_map<int, int>& coincidencias) const {
    if (!nodoInicial) return;

    vector<NodoTrie*> pila;
    pila.push_back(nodoInicial);

    while (!pila.empty()) {
        NodoTrie* actual = pila.back();
        pila.pop_back();

        if (actual->esFinDePalabra) {
            for (const auto& [idPelicula, frec] : actual->frecuencias) {
                coincidencias[idPelicula] += frec;
            }
        }

        for (const auto& [caracter, hijo] : actual->hijos) {
            if (hijo) {
                pila.push_back(hijo);
            }
        }
    }
}