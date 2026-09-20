#ifndef INTERFAZ_H
#define INTERFAZ_H
#include <vector>
#include <string>
#include "preprocesador.h"
#include "trie.h"
#include "buscador.h"
using namespace std;
class Interfaz {
private:
    vector<Pelicula> peliculas;
    SuffixTrie* trie;
public:
    Interfaz(const vector<Pelicula>& peliculas,SuffixTrie* trie);
    void iniciar();
private:
    void mostrarMenu();
    void buscarPeliculas();
    void buscarPorTag();
    void mostrarResultados(const vector<ResultadoBusqueda>& resultados,const string& consulta);
    void mostrarDetalle(const Pelicula& pelicula);
    void mostrarVerMasTarde();
    void mostrarInicio();
    Pelicula* obtenerPeliculaPorId(int id);
    void limpiarPantalla();
    void pausar();
    int leerOpcion();
};
#endif