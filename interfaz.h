#ifndef INTERFAZ_H
#define INTERFAZ_H
#include <vector>
#include <string>
#include "pelicula.h"
using namespace std;
class Interfaz {
private:
    vector<Pelicula> peliculas;
public:
    Interfaz(const vector<Pelicula>& peliculas);
    void iniciar();
private:
    void mostrarMenu();
    void buscarPeliculas();
    void mostrarResultados(
        const vector<Pelicula>& resultados,
        const string& consulta
    );
    void mostrarDetalle(const Pelicula& pelicula);
    void mostrarVerMasTarde();
    void limpiarPantalla();
    void pausar();
};
#endif