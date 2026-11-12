#include "buscador.h"
#include "ranking.h"
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
using namespace std;

vector<string> dividir_consulta(const string& consulta) {
    vector<string> tokens = tokenizar(normalizar(consulta));

    vector<string> terminos;
    unordered_set<string> terminos_vistos;

    for (const string& token : tokens) {
        if (terminos_vistos.insert(token).second) {
            terminos.push_back(token);
        }
    }

    return terminos;
}

vector<ResultadoBusqueda> buscar(const SuffixTrie& trie, const vector<Pelicula>& peliculas, const string& consulta) {
    vector<string> terminos = dividir_consulta(consulta);

    if (terminos.empty()) {
        return {};
    }

    unordered_map<int, int> frecuencia_total;

    unordered_map<int, int> cobertura;

    unordered_map<int, int> en_titulo;

    unordered_map<int, string> titulos_normalizados;

    for (const string& termino : terminos) {
        if (termino.empty()) {
            continue;
        }

        NodoTrie* nodo = trie.navegarPrefijo(termino);

        if (nodo == nullptr) {
            continue;
        }

        unordered_map<int, int> coincidencias_termino;

        trie.recolectarCoincidencias(nodo, coincidencias_termino);

        for (const auto& [id, frecuencia] : coincidencias_termino) {

            if (id < 1 || id > static_cast<int>(peliculas.size())) {
                continue;
            }

            frecuencia_total[id] += frecuencia;
            cobertura[id] += 1;

            auto resultado_insercion = titulos_normalizados.try_emplace(id);

            auto iterador_titulo = resultado_insercion.first;

            bool fue_insertado = resultado_insercion.second;

            if (fue_insertado) {
                iterador_titulo->second = normalizar(peliculas[id - 1].titulo);
            }

            const string& titulo_normalizado = iterador_titulo->second;

            if (titulo_normalizado.find(termino) != string::npos) {
                en_titulo[id] += 1;
            }
        }
    }

    vector<ResultadoBusqueda> resultados;
    resultados.reserve(frecuencia_total.size());

    for (const auto& [id, frecuencia] : frecuencia_total) {

        FactoresRanking factores;

        factores.cobertura = cobertura[id];
        factores.frecuencia = frecuencia;
        factores.coincidenciasTitulo = en_titulo[id];

        double score = calcularPuntajeRanking(factores);

        resultados.push_back({id, score});
    }

    sort(
        resultados.begin(),
        resultados.end(),
        [](const ResultadoBusqueda& a,
           const ResultadoBusqueda& b) {

            if (a.score != b.score) {
                return a.score > b.score;
            }

            return a.id < b.id;
        }
    );

    return resultados;
}

void mostrar_resultados(const vector<ResultadoBusqueda>& resultados, const vector<Pelicula>& peliculas, int desde, int cantidad) {
    if (desde >= static_cast<int>(resultados.size())) {
        cout << "No hay mas resultados.\n";
        return;
    }

    int hasta = min(desde + cantidad, static_cast<int>(resultados.size()));

    for (int i = desde; i < hasta; ++i) {
        int id = resultados[i].id;

        if (id < 1 || id > static_cast<int>(peliculas.size())) {
            continue;
        }

        const Pelicula& pelicula = peliculas[id - 1];

        cout << (i + 1) << ". " << pelicula.titulo << " (" << pelicula.anho << ") - " << pelicula.director << " [score=" << resultados[i].score << "]\n";
    }
}