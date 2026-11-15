#include "trie.h"
#include <algorithm>
#include <mutex>
#include <thread>
#include <iostream>
using namespace std;

SuffixTrie::SuffixTrie(int minSufijoLen) : raiz(new NodoTrie()), largoMinimoSufijo(max(1,minSufijoLen)) {}
SuffixTrie::~SuffixTrie() { delete raiz; }

void SuffixTrie::insertarSufijoDesde(NodoTrie* actual, string_view palabra, size_t inicio, int id) {
    for (size_t i = inicio; i < palabra.size(); ++i) {
        NodoTrie*& hijo = actual->hijos[palabra[i]];
        if (!hijo) hijo = new NodoTrie();
        actual = hijo;
    }
    actual->esFinDePalabra = true;
    ++actual->frecuencias[id];
}

void SuffixTrie::insertarPalabraSinBloqueo(string_view palabra, int id) {
    if (palabra.empty()) return;
    // Cada sufijo indexado permite localizar subcadenas, incluso al final.
    for (size_t i = 0; i < palabra.size(); ++i) {
        if (palabra.size() - i >= static_cast<size_t>(largoMinimoSufijo) || i == 0)
            insertarSufijoDesde(raiz, palabra, i, id);
    }
}

void SuffixTrie::insertarPalabra(string_view palabra, int id) {
    unique_lock<shared_mutex> lock(mutexArbol);
    insertarPalabraSinBloqueo(palabra, id);
}

void SuffixTrie::construirIndice(const vector<Pelicula>& peliculas) {
    cout << "Construyendo indice secuencial...\n";
    unique_lock<shared_mutex> lock(mutexArbol);
    for (const auto& peli : peliculas)
        for (const auto& token : peli.tokens_txt)
            insertarPalabraSinBloqueo(token, peli.id);
    cout << "Indice construido.\n";
}

void SuffixTrie::construirIndiceParalelo(const vector<Pelicula>& peliculas, unsigned hilos) {
    if (hilos < 2 || peliculas.size() < 2) { construirIndice(peliculas); return; }
    hilos = min<unsigned>(hilos, static_cast<unsigned>(peliculas.size()));
    cout << "Indexando con " << hilos << " hilos...\n";
    vector<thread> trabajadores;
    trabajadores.reserve(hilos);
    for (unsigned t = 0; t < hilos; ++t) {
        size_t inicio = peliculas.size() * t / hilos;
        size_t fin = peliculas.size() * (t + 1) / hilos;
        trabajadores.emplace_back([this, &peliculas, inicio, fin]() {
            for (size_t i = inicio; i < fin; ++i) {
                // Bloqueo por pelicula: menos contencion que uno por palabra.
                unique_lock<shared_mutex> lock(mutexArbol);
                for (const auto& token : peliculas[i].tokens_txt)
                    insertarPalabraSinBloqueo(token, peliculas[i].id);
            }
        });
    }
    for (auto& hilo : trabajadores) hilo.join();
    cout << "Indice paralelo construido.\n";
}

NodoTrie* SuffixTrie::navegarPrefijoSinBloqueo(string_view prefijo) const {
    NodoTrie* actual = raiz;
    for (char c : prefijo) {
        auto it = actual->hijos.find(c);
        if (it == actual->hijos.end()) return nullptr;
        actual = it->second;
    }
    return actual;
}

void SuffixTrie::recolectarSinBloqueo(NodoTrie* inicial, unordered_map<int,int>& salida) const {
    if (!inicial) return;
    vector<NodoTrie*> pila{inicial};
    while (!pila.empty()) {
        NodoTrie* actual = pila.back(); pila.pop_back();
        if (actual->esFinDePalabra)
            for (const auto& par : actual->frecuencias) salida[par.first] += par.second;
        for (const auto& par : actual->hijos)
            if (par.second) pila.push_back(par.second);
    }
}

unordered_map<int,int> SuffixTrie::buscarCoincidencias(string_view termino) const {
    unordered_map<int,int> resultado;
    if (termino.empty()) return resultado;
    shared_lock<shared_mutex> lock(mutexArbol);
    recolectarSinBloqueo(navegarPrefijoSinBloqueo(termino), resultado);
    return resultado;
}

NodoTrie* SuffixTrie::navegarPrefijo(string_view prefijo) const {
    shared_lock<shared_mutex> lock(mutexArbol);
    return navegarPrefijoSinBloqueo(prefijo);
}
void SuffixTrie::recolectarCoincidencias(NodoTrie* nodo, unordered_map<int,int>& salida) const {
    shared_lock<shared_mutex> lock(mutexArbol);
    recolectarSinBloqueo(nodo, salida);
}
