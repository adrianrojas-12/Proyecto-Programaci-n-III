#ifndef PREPROCESADOR_H
#define PREPROCESADOR_H

#include <string>
#include <vector>

struct Pelicula {
    int id;
    int anho;
    std::string titulo;
    std::string origen;
    std::string director;
    std::string cast;
    std::string genero;
    std::string sinopsis;
    std::vector<std::string> tokens_combinados;
};

std::string normalizar(const std::string& txt);

std::vector<std::string> tokenizar(const std::string& txt_normalizado);

#endif //PREPROCESADOR_H
