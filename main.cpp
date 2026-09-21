#include <iostream>
#include <vector>
#include <algorithm>
#include "buscador.h"
#include "cargador_csv.h"
#include "preprocesador.h"
#include "trie.h"
#include "interfaz.h"
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

    SuffixTrie trie(2);
    trie.construirIndice(peliculas);
    cout << "--------------------------------------------------" << endl;

    // Pausa para confirmar la carga antes de limpiar la consola
    cout << "Presione ENTER para ingresar a la plataforma...";
    cin.get();

    Interfaz interfaz(peliculas, &trie);
    interfaz.iniciar();

    // cout << "=== Prueba de busqueda ===" << endl;
    // cout << "Escribe una palabra, sub-palabra o frase para buscar." << endl;
    // cout << "Escribe 'salir' para terminar." << endl << endl;
    //
    // string consulta;
    // while (true) {
    //     cout << "Buscar: ";
    //     getline(cin, consulta);
    //
    //     if (consulta == "salir") break;
    //     if (consulta.empty()) continue;
    //
    //     vector<ResultadoBusqueda> resultados = buscar(trie, peliculas, consulta);
    //
    //     if (resultados.empty()) {
    //         cout << "No se encontraron coincidencias." << endl << endl;
    //         continue;
    //     }
    //
    //     cout << "Total de coincidencias: " << resultados.size() << endl;
    //     mostrar_resultados(resultados, peliculas, 0, 5);  // primeros 5
    //
    //     cout << endl << "Ver 5 mas? (s/n): ";
    //     string opcion;
    //     getline(cin, opcion);
    //     if (opcion == "s") {
    //         mostrar_resultados(resultados, peliculas, 5, 5);  // siguientes 5
    //     }
    //     cout << endl;
    // }
    //
    // cout << "--------------------------------------------------" << endl;
    // cout << "Prueba finalizada." << endl;

    return 0;
}