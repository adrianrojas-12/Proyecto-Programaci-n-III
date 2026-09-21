#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <vector>
#include <string>
#include "preprocesador.h"
#include "trie.h"
#include "buscador.h"
#include "persistencia.h"
#include "recomendador.h"

class Interfaz {
private:
    std::vector<Pelicula> peliculas;
    SuffixTrie* trie; // Puntero al árbol indexado

public:
    Interfaz(const std::vector<Pelicula>& peliculas, SuffixTrie* trie);
    void iniciar();

private:
    void mostrarInicio();
    void mostrarMenu();
    void buscarPeliculas();
    void buscarPorTag();
    void mostrarResultados(const std::vector<ResultadoBusqueda>& resultados, const std::string& consulta);
    void mostrarDetalle(const Pelicula& pelicula);
    void mostrarVerMasTarde();

    // Auxiliares
    const Pelicula* obtenerPeliculaPorId(int id) const;
    void limpiarPantalla() const;
    void pausar() const;
    int leerOpcion() const;
};

#endif // INTERFAZ_H