#ifndef TRIE_H
#define TRIE_H

#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include "preprocesador.h"

struct NodoTrie {
    std::unordered_map<char, NodoTrie*> hijos;
    bool esFinDePalabra = false;
    std::unordered_map<int, int> frecuencias;

    ~NodoTrie() {
        for (auto& [caracter, nodoHijo] : hijos) {
            delete nodoHijo;
        }
    }
};

class SuffixTrie {
private:
    NodoTrie* raiz;
    int largoMinimoSufijo;

    // Inserción interna usando índice de inicio para evitar copias de string
    void insertarSufijoDesde(NodoTrie* nodoActual, std::string_view palabra, size_t start_idx, int idPelicula);

public:
    explicit SuffixTrie(int minSufijoLen = 3);
    ~SuffixTrie();

    void insertarPalabra(std::string_view palabra, int idPelicula);
    void construirIndice(const std::vector<Pelicula>& peliculas);

    NodoTrie* navegarPrefijo(std::string_view prefijo) const;
    void recolectarCoincidencias(NodoTrie* nodoInicial, std::unordered_map<int, int>& coincidencias) const;

    NodoTrie* getRaiz() const { return raiz; }
};

#endif // TRIE_H