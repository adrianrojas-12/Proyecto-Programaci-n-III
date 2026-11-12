#ifndef RECOMENDADOR_H
#define RECOMENDADOR_H

#include <cstddef>
#include <vector>
#include "preprocesador.h"

struct Recomendacion {
    int id;
    double score;
};

std::vector<Recomendacion> rankearRecomendaciones(const std::vector<Pelicula>& peliculas, const std::vector<int>& likes_ids, std::size_t limite = 5);

std::vector<Pelicula> generarRecomendaciones(const std::vector<Pelicula>& peliculas, const std::vector<int>& likes_ids);

#endif // RECOMENDADOR_H