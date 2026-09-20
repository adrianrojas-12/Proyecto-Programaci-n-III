
#include "recomendador.h"
#include <unordered_map>
#include <algorithm>
#include <string>

using namespace std;

vector<Pelicula> generarRecomendaciones(const vector<Pelicula>& todas_las_peliculas, const vector<int>& likes_ids) {
    vector<Pelicula> recomendaciones;
    if (likes_ids.empty()) {
        return recomendaciones;
    }

    // conteo de los generos de las peliculas likeadas
    unordered_map<string, int> conteo_generos;
    for (int id : likes_ids) {
        for (const Pelicula& p : todas_las_peliculas) {
            if (p.id == id && !p.genero.empty() && p.genero != "unknown") {
                conteo_generos[p.genero]++;
                break;
            }
        }
    }

    if (conteo_generos.empty()) {
        return recomendaciones;
    }

    //Hallar el genero preferido
    string genero_favorito = "";
    int max_count = 0;
    for (const auto& par : conteo_generos) {
        if (par.second > max_count) {
            max_count = par.second;
            genero_favorito = par.first;
        }
    }

    // busca peliculas con genero similar, el limite es 3
    int limite = 3;
    for (const Pelicula& p : todas_las_peliculas) {
        if (recomendaciones.size() >= limite) break;

        if (p.genero == genero_favorito) {
            if (find(likes_ids.begin(), likes_ids.end(), p.id) == likes_ids.end()) {
                recomendaciones.push_back(p);
            }
        }
    }

    return recomendaciones;
}