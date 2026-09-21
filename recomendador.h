
#ifndef PROYECTO_PROGRAMACI_N_III_RECOMENDADOR_H
#define PROYECTO_PROGRAMACI_N_III_RECOMENDADOR_H

#include <vector>
#include "preprocesador.h"

std::vector<Pelicula> generarRecomendaciones(
    const std::vector<Pelicula>& todas_las_peliculas,
    const std::vector<int>& likes_ids
);


#endif //PROYECTO_PROGRAMACI_N_III_RECOMENDADOR_H