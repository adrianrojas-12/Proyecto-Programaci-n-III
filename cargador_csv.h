#ifndef CARGADOR_CSV_H
#define CARGADOR_CSV_H

#include <string>
#include <vector>
#include "preprocesador.h"

std::vector<Pelicula> cargar_peliculas(const std::string& ruta_archivo);

#endif //CARGADOR_CSV_H
