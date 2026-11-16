#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <string>
#include <vector>

#include "patrones.h"  // ControladorApp

class Interfaz {
public:
    explicit Interfaz(ControladorApp& controlador);

    void iniciar();

private:
    void mostrarInicio();
    void mostrarMenu();
    void buscarPeliculas();
    void buscarPorTag();
    void mostrarResultados(const std::vector<ResultadoBusqueda>& resultados, const std::string& consulta);
    void mostrarDetalle(const Pelicula& pelicula);
    void mostrarVerMasTarde();

    void limpiarPantalla() const;
    void pausar() const;
    int leerOpcion() const;

    ControladorApp& controlador_;
};

#endif  // INTERFAZ_H