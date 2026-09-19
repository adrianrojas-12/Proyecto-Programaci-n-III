#include "buscador.h"
#include <iostream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <unordered_map>
using namespace std;

namespace {
    constexpr double PESO_COBERTURA = 50.0;
    constexpr double PESO_TITULO = 30.0;
}

vector<string> dividir_consulta(const string& consulta) {
    string normalizada = normalizar(consulta);
    vector<string> terminos;
    stringstream ss(normalizada);
    string termino;

    while (ss >> termino) {
        terminos.push_back(termino);
    }
    return terminos;
}

vector<ResultadoBusqueda> buscar(
    const SuffixTrie& trie,
    const vector<Pelicula>& peliculas,
    const string& consulta) {

    vector<string> terminos = dividir_consulta(consulta);

    // id -> suma de frecuencias de todos los terminos encontrados
    unordered_map<int, double> frecuencia_total;
    // id -> cuantos terminos DISTINTOS de la consulta matcheo esta pelicula
    unordered_map<int, int> cobertura;
    // id -> cuantos de esos terminos aparecen ademas literalmente en el titulo
    unordered_map<int, int> en_titulo;

    for (const string& termino : terminos) {
        if (termino.empty()) continue;

        NodoTrie* nodo = trie.navegarPrefijo(termino);
        if (nodo == nullptr) continue;  // ningun sufijo indexado empieza con este termino

        unordered_map<int, int> coincidencias_termino;
        trie.recolectarCoincidencias(nodo, coincidencias_termino);

        for (const auto& [id, frecuencia] : coincidencias_termino) {
            frecuencia_total[id] += frecuencia;
            cobertura[id] += 1;

            // Bonus: ¿el termino aparece literalmente en el titulo?
            // (peliculas[id - 1] es seguro: cargador_csv asigna ids
            // secuenciales de 1..N sin huecos, verificado contra el CSV.)
            const string& titulo_normalizado = normalizar(peliculas[id - 1].titulo);
            if (titulo_normalizado.find(termino) != string::npos) {
                en_titulo[id] += 1;
            }
        }
    }

    vector<ResultadoBusqueda> resultados;
    resultados.reserve(frecuencia_total.size());

    for (const auto& [id, frecuencia] : frecuencia_total) {
        double score = PESO_COBERTURA * cobertura[id]
                      + frecuencia
                      + PESO_TITULO * en_titulo[id];
        resultados.push_back({id, score});
    }

    sort(resultados.begin(), resultados.end(),
         [](const ResultadoBusqueda& a, const ResultadoBusqueda& b) {
             if (a.score != b.score) return a.score > b.score;
             return a.id < b.id;
         });

    return resultados;
}

void mostrar_resultados(
    const vector<ResultadoBusqueda>& resultados,
    const vector<Pelicula>& peliculas,
    int desde,
    int cantidad) {
    if (desde >= static_cast<int>(resultados.size())) {
        cout << "No hay mas resultados.\n";
        return;
    }

    int hasta = min(desde + cantidad, static_cast<int>(resultados.size()));
    for (int i = desde; i < hasta; ++i) {
        const Pelicula& p = peliculas[resultados[i].id - 1];
        cout << (i + 1) << ". " << p.titulo
             << " (" << p.anho << ") - " << p.director
             << "  [score=" << resultados[i].score << "]\n";
    }
}