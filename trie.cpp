#include "trie.h"
#include <iostream>

using namespace std;

SuffixTrie::SuffixTrie(int minSufijoLen) : largoMinimoSufijo(minSufijoLen) {
    raiz = new NodoTrie();
}

SuffixTrie::~SuffixTrie() {
    delete raiz;
}

int SuffixTrie::obtenerIndice(char c) const {
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c >= '0' && c <= '9') return 26 + (c - '0');
    return -1;
}

void SuffixTrie::insertarSufijo(NodoTrie* nodoActual, const string& sufijo, int idPelicula) {
    for (char c : sufijo) {
        int idx = obtenerIndice(c);
        if (idx == -1) continue; 

        if (nodoActual->hijos[idx] == nullptr) {
            nodoActual->hijos[idx] = new NodoTrie();
        }
        nodoActual = nodoActual->hijos[idx];
    }
    
    nodoActual->esFinDePalabra = true;
    nodoActual->frecuencias[idPelicula]++;
}

void SuffixTrie::insertarPalabra(const string& palabra, int idPelicula) {
    int n = palabra.length();
    
    // Se generan sufijos palabra por palabra
    for (int i = 0; i < n; ++i) {
        if (n - i >= largoMinimoSufijo) {
            string sufijo = palabra.substr(i);
            insertarSufijo(raiz, sufijo, idPelicula);
        }
    }
}

NodoTrie* SuffixTrie::navegarPrefijo(const string& prefijo) const {
    NodoTrie* actual = raiz;
    for (char c : prefijo) {
        int idx = obtenerIndice(c);
        if (idx == -1 || actual->hijos[idx] == nullptr) {
            return nullptr;
        }
        actual = actual->hijos[idx];
    }
    return actual;
}

void SuffixTrie::recolectarCoincidencias(NodoTrie* nodo, unordered_map<int, int>& acumulado) const {
    if (nodo == nullptr) return;

    if (nodo->esFinDePalabra) {
        for (const auto& par : nodo->frecuencias) {
            acumulado[par.first] += par.second;
        }
    }

    for (int i = 0; i < 36; ++i) {
        if (nodo->hijos[i] != nullptr) {
            recolectarCoincidencias(nodo->hijos[i], acumulado);
        }
    }
}

void SuffixTrie::construirIndice(const vector<Pelicula>& peliculas) {
    cout << "Construyendo el Suffix Trie..." << endl;
    
    for (const auto& peli : peliculas) {
        // Usa peli.tokens_combinados y peli.id exactos de preprocesador.h
        for (const string& token : peli.tokens_combinados) {
            insertarPalabra(token, peli.id);
        }
    }
    
    cout << "Indice Suffix Trie construido exitosamente." << endl;
}