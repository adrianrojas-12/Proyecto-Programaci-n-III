#include <iostream>
#include <vector>
#include <algorithm>
#include "buscador.h"
#include "cargador_csv.h"
#include "preprocesador.h"
#include "trie.h"
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

    int limite = min(10, (int)p.tokens_txt.size());
    for (int i = 0; i < limite; i++) {
        cout << "- " << p.tokens_txt[i] << endl;
    }

    cout << "--------------------------------------------------" << endl;
    cout << "El preprocesamiento funciona correctamente" << endl;
    cout << "--------------------------------------------------" << endl;

    SuffixTrie trie(2);
    trie.construirIndice(peliculas);

    cout << "--------------------------------------------------" << endl << endl;

    cout << "=== Prueba de busqueda ===" << endl;
    cout << "Escribe una palabra, sub-palabra o frase para buscar." << endl;
    cout << "Escribe 'salir' para terminar." << endl << endl;

    string consulta;
    while (true) {
        cout << "Buscar: ";
        getline(cin, consulta);

        if (consulta == "salir") break;
        if (consulta.empty()) continue;

        vector<ResultadoBusqueda> resultados = buscar(trie, peliculas, consulta);

        if (resultados.empty()) {
            cout << "No se encontraron coincidencias." << endl << endl;
            continue;
        }

        cout << "Total de coincidencias: " << resultados.size() << endl;
        mostrar_resultados(resultados, peliculas, 0, 5);  // primeros 5

        cout << endl << "Ver 5 mas? (s/n): ";
        string opcion;
        getline(cin, opcion);
        if (opcion == "s") {
            mostrar_resultados(resultados, peliculas, 5, 5);  // siguientes 5
        }
        cout << endl;
    }

    cout << "--------------------------------------------------" << endl;
    cout << "Prueba finalizada." << endl;

    return 0;
}