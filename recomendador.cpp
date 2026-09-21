
#include "recomendador.h"
#include <unordered_map>
#include <algorithm>
#include <string>
#include <unordered_set>

using namespace std;

vector<Pelicula> generarRecomendaciones(
    const vector<Pelicula>& todas_las_peliculas,
    const vector<int>& likes_ids)
{
    if (likes_ids.empty()) return {};

    // 1. Identificar géneros preferidos por el usuario a partir de sus Likes
    unordered_map<string, int> conteo_generos;
    unordered_set<int> ids_liked(likes_ids.begin(), likes_ids.end());

    for (const auto& peli : todas_las_peliculas) {
        if (ids_liked.count(peli.id)) {
            conteo_generos[peli.genero]++;
        }
    }

    if (conteo_generos.empty()) return {};

    // Obtener el género con más likes
    string genero_top = "";
    int max_likes = -1;
    for (const auto& [genero, cantidad] : conteo_generos) {
        if (cantidad > max_likes) {
            max_likes = cantidad;
            genero_top = genero;
        }
    }

    // 2. Filtrar películas del mismo género que el usuario aún no haya marcado con Like
    vector<Pelicula> recomendaciones;
    for (const auto& peli : todas_las_peliculas) {
        if (!ids_liked.count(peli.id) && peli.genero == genero_top) {
            recomendaciones.push_back(peli);
            if (recomendaciones.size() >= 5) break; // Retornar máximo 5
        }
    }

    return recomendaciones;
}