#include <iostream>
#include "patrones.h"
#include "interfaz.h"

using namespace std;

int main() {
    string nombre_archivo = "wiki_movie_plots_deduped.csv";
    cout << "Iniciando la lectura del archivo: " << nombre_archivo << "..." << endl;

    ControladorApp controlador(nombre_archivo, 4);
    if (!controlador.inicializar()) {
        cout << "Error: No se cargaron peliculas" << endl;
        cout << "Asegurate de que el archivo CSV este en la misma carpeta que el ejecutable" << endl;
        return 1;
    }

    cout << "Exito: Se cargaron " << controlador.totalPeliculas() << " peliculas correctamente" << endl;
    cout << "--------------------------------------------------" << endl;

    // Pausa para confirmar la carga antes de limpiar la consola
    cout << "Presione ENTER para ingresar a la plataforma...";
    cin.get();

    Interfaz interfaz(controlador);
    interfaz.iniciar();

    return 0;
}