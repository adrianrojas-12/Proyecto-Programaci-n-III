#include "benchmark.h"
#include "trie.h"
#include "buscador.h"
#include "buscador_paralelo.h"
#include <chrono>
#include <iostream>
#include <algorithm>
using namespace std;
using Reloj = chrono::steady_clock;

void ejecutarBenchmark(const vector<Pelicula>& peliculas, unsigned cantidad) {
    // Limitar el volumen para evitar agotar memoria con el Trie de sufijos.
    size_t n = min<size_t>(peliculas.size(), 100);
    vector<Pelicula> muestra(peliculas.begin(), peliculas.begin() + n);
    cout << "\n=== BENCHMARK (" << n << " peliculas) ===\n";
    SuffixTrie secuencial(2);
    auto a = Reloj::now();
    secuencial.construirIndice(muestra);
    auto b = Reloj::now();
    double ts = chrono::duration<double, milli>(b-a).count();
    cout << "Indexacion 1 hilo: " << ts << " ms\n";
    SuffixTrie paralelo(2);
    a = Reloj::now();
    paralelo.construirIndiceParalelo(muestra, cantidad);
    b = Reloj::now();
    double tp = chrono::duration<double, milli>(b-a).count();
    cout << "Indexacion " << cantidad << " hilos: " << tp << " ms; speedup: " << (tp ? ts/tp : 0) << "\n";
    const vector<string> consultas = {"ship ghost", "war love", "king queen", "sea battle", "detective murder"};
    for (const auto& consulta : consultas) {
        a = Reloj::now();
        auto r1 = buscar(secuencial, muestra, consulta);
        b = Reloj::now();
        double t1 = chrono::duration<double, milli>(b-a).count();
        a = Reloj::now();
        auto r2 = buscarParalelo(paralelo, muestra, consulta, cantidad);
        b = Reloj::now();
        double t2 = chrono::duration<double, milli>(b-a).count();
        bool iguales = r1.size() == r2.size();
        if (iguales) for (size_t i = 0; i < r1.size(); ++i)
            if (r1[i].id != r2[i].id || r1[i].score != r2[i].score) { iguales = false; break; }
        cout << "Consulta '" << consulta << "': sec=" << t1 << " ms, par=" << t2
             << " ms, speedup=" << (t2 ? t1/t2 : 0) << ", iguales=" << (iguales ? "SI" : "NO") << '\n';
    }
}
