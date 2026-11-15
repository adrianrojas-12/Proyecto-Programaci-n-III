#ifndef TRIE_H
#define TRIE_H
#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <shared_mutex>
#include "preprocesador.h"

struct NodoTrie {
    std::unordered_map<char, NodoTrie*> hijos;
    bool esFinDePalabra = false;
    std::unordered_map<int,int> frecuencias;
    ~NodoTrie() { for (auto& par : hijos) delete par.second; }
};

class SuffixTrie {
private:
    NodoTrie* raiz;
    int largoMinimoSufijo;
    mutable std::shared_mutex mutexArbol;
    void insertarSufijoDesde(NodoTrie* actual, std::string_view palabra, size_t inicio, int id);
    void insertarPalabraSinBloqueo(std::string_view palabra, int id);
    NodoTrie* navegarPrefijoSinBloqueo(std::string_view prefijo) const;
    void recolectarSinBloqueo(NodoTrie* nodo, std::unordered_map<int,int>& salida) const;
public:
    explicit SuffixTrie(int minSufijoLen = 3);
    ~SuffixTrie();
    SuffixTrie(const SuffixTrie&) = delete;
    SuffixTrie& operator=(const SuffixTrie&) = delete;
    void insertarPalabra(std::string_view palabra, int idPelicula);
    void construirIndice(const std::vector<Pelicula>& peliculas);
    void construirIndiceParalelo(const std::vector<Pelicula>& peliculas, unsigned hilos = 4);
    // API segura: toda la lectura queda protegida por el mismo bloqueo.
    std::unordered_map<int,int> buscarCoincidencias(std::string_view termino) const;
    // API antigua: solo utilizar cuando NO hay escritores concurrentes.
    NodoTrie* navegarPrefijo(std::string_view prefijo) const;
    void recolectarCoincidencias(NodoTrie* nodo, std::unordered_map<int,int>& salida) const;
    NodoTrie* getRaiz() const { return raiz; } // legado: no seguro durante escritura
};
#endif
