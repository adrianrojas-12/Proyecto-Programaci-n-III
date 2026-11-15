#include "buscador_paralelo.h"
#include "ranking.h"
#include <future>
#include <unordered_map>
#include <algorithm>
using namespace std;

vector<ResultadoBusqueda> buscarParalelo(const SuffixTrie& trie,
    const vector<Pelicula>& peliculas, const string& consulta, unsigned hilos) {
    vector<string> terminos = dividir_consulta(consulta);
    if (terminos.empty()) return {};
    if (hilos < 2 || terminos.size() < 2) return buscar(trie, peliculas, consulta);
    hilos = min<unsigned>(hilos, static_cast<unsigned>(terminos.size()));
    vector<future<vector<unordered_map<int,int>>>> tareas;
    for (unsigned t = 0; t < hilos; ++t) {
        size_t inicio = terminos.size() * t / hilos;
        size_t fin = terminos.size() * (t + 1) / hilos;
        tareas.push_back(async(launch::async, [&trie, &terminos, inicio, fin]() {
            vector<unordered_map<int,int>> parciales;
            for (size_t i = inicio; i < fin; ++i)
                parciales.push_back(trie.buscarCoincidencias(terminos[i]));
            return parciales;
        }));
    }
    unordered_map<int,int> frecuencia, cobertura, enTitulo;
    unordered_map<int,string> titulos;
    size_t indiceTermino = 0;
    // Fusion en el hilo principal: sin escrituras simultaneas a los mapas.
    for (auto& tarea : tareas) {
        for (const auto& coincidencias : tarea.get()) {
            const string& termino = terminos[indiceTermino++];
            for (const auto& [id, frec] : coincidencias) {
                if (id < 1 || id > static_cast<int>(peliculas.size())) continue;
                frecuencia[id] += frec;
                ++cobertura[id];
                auto [it, nuevo] = titulos.try_emplace(id);
                if (nuevo) it->second = normalizar(peliculas[id-1].titulo);
                if (it->second.find(termino) != string::npos) ++enTitulo[id];
            }
        }
    }
    vector<ResultadoBusqueda> resultado;
    resultado.reserve(frecuencia.size());
    for (const auto& [id, frec] : frecuencia) {
        FactoresRanking factores;
        factores.cobertura = cobertura[id];
        factores.frecuencia = frec;
        factores.coincidenciasTitulo = enTitulo[id];
        resultado.push_back({id, calcularPuntajeRanking(factores)});
    }
    sort(resultado.begin(), resultado.end(), [](const ResultadoBusqueda& a, const ResultadoBusqueda& b) {
        if (a.score != b.score) return a.score > b.score;
        return a.id < b.id;
    });
    return resultado;
}
