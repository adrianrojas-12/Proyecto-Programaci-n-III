#include <iostream>
#include <vector>
#include <algorithm>
#include "cargador_csv.h"
#include "preprocesador.h"

using namespace std;

int main() {
    string nombre_archivo = "wiki_movie_plots_deduped.csv";

    cout << "Iniciando la lectura del archivo: " << nombre_archivo << "..." << endl;

    vector<Pelicula> peliculas = cargar_peliculas(nombre_archivo);

    if (peliculas.empty()) {
        cout << "Error: No se cargaron peliculas" << endl;
        cout << "Asegurate de que el archivo CSV este en la misma carpeta que el ejecutable" << endl;
        return 1;
    }

    cout << "Exito: Se cargaron " << peliculas.size() << " peliculas correctamente" << endl;
    cout << "--------------------------------------------------" << endl;

    Pelicula p = peliculas[0];

    cout << "Verificacion de la primera pelicula:" << endl;
    cout << "* ID: " << p.id << endl;
    cout << "* Anho: " << p.anho << endl;
    cout << "* Titulo: " << p.titulo << endl;
    cout << "* Director: " << p.director << endl;
    cout << "* Genero: " << p.genero << endl;
    cout << "* Longitud de la sinopsis: " << p.sinopsis.length() << " caracteres" << endl;

    cout << endl << "Primeros 10 tokens generados (listos para el indice):" << endl;

    int limite = min(10, (int)p.tokens_combinados.size());
    for (int i = 0; i < limite; i++) {
        cout << "- " << p.tokens_combinados[i] << endl;
    }

    cout << "--------------------------------------------------" << endl;
    cout << "El preprocesamiento funciona correctamente" << endl;

    return 0;
}