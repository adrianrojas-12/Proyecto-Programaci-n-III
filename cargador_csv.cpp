#include "cargador_csv.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <utility>

namespace {
    int convertirAnho(const std::string& texto) {
        if (texto.empty()) {
            return 0;
        }

        try {
            return std::stoi(texto);
        }
        catch (const std::exception&) {
            return 0;
        }
    }

    bool construirPelicula(const std::vector<std::string>& campos, int id, Pelicula& pelicula) {
        if (campos.size() < 8) {
            return false;
        }

        if (campos[1].empty()) {
            return false;
        }

        pelicula.id = id;
        pelicula.anho = convertirAnho(campos[0]);
        pelicula.titulo = campos[1];
        pelicula.origen = campos[2];
        pelicula.director = campos[3];
        pelicula.cast = campos[4];
        pelicula.genero = campos[5];
        pelicula.sinopsis = campos[7];

        std::string texto_total = pelicula.titulo + " " + pelicula.sinopsis;

        pelicula.tokens_txt = tokenizar(normalizar(texto_total));

        return true;
    }
}

std::vector<Pelicula> cargar_peliculas(const std::string& ruta_archivo) {
    std::vector<Pelicula> peliculas;

    std::ifstream archivo(ruta_archivo, std::ios::binary);

    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << ruta_archivo << std::endl;
        return peliculas;
    }

    bool en_comillas = false;
    bool primera_fila = true;

    std::string campo_actual;
    std::vector<std::string> fila_actual;

    char c;

    auto procesarFila = [&]() {
        fila_actual.push_back(campo_actual);
        campo_actual.clear();

        if (primera_fila) {
            primera_fila = false;
            fila_actual.clear();
            return;
        }

        Pelicula pelicula;

        int nuevo_id = static_cast<int>(peliculas.size()) + 1;

        if (construirPelicula(fila_actual, nuevo_id, pelicula)) {
            peliculas.push_back(std::move(pelicula));
        }

        fila_actual.clear();
    };

    while (archivo.get(c)) {
        if (en_comillas) {
            if (c == '"') {
                if (archivo.peek() == '"') {
                    campo_actual += '"';
                    archivo.get();
                }
                else {
                    en_comillas = false;
                }
            }
            else {
                campo_actual += c;
            }
        }
        else {
            if (c == '"') {
                en_comillas = true;
            }
            else if (c == ',') {
                fila_actual.push_back(campo_actual);
                campo_actual.clear();
            }
            else if (c == '\n' || c == '\r') {
                if (c == '\r' && archivo.peek() == '\n') {
                    archivo.get();
                }

                if (!campo_actual.empty() || !fila_actual.empty()) {
                    procesarFila();
                }
            }
            else {
                campo_actual += c;
            }
        }
    }

    if (!en_comillas && (!campo_actual.empty() || !fila_actual.empty())) {
        procesarFila();
    }

    archivo.close();

    return peliculas;
}