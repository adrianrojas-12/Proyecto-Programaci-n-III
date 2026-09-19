#include "cargador_csv.h"
#include <fstream>
#include <iostream>

std::vector<Pelicula> cargar_peliculas(const std::string& ruta_archivo) {
    std::vector<Pelicula> peliculas;
    std::ifstream archivo(ruta_archivo, std::ios::binary); 
    
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << ruta_archivo << std::endl;
        return peliculas;
    }

    bool en_comillas = false;
    std::string campo_actual = "";
    std::vector<std::string> fila_actual;
    char c;
    int contador_id = 1;

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
                campo_actual = "";
            }
            else if (c == '\n' || c == '\r') {
                if (c == '\r' && archivo.peek() == '\n') {
                    archivo.get();
                }

                if (!campo_actual.empty() || !fila_actual.empty()) {
                    fila_actual.push_back(campo_actual);

                    if (contador_id > 1 && fila_actual.size() >= 8) {
                        Pelicula p;
                        p.id = contador_id - 1;
                        p.anho = fila_actual[0].empty() ? 0 : std::stoi(fila_actual[0]);
                        p.titulo = fila_actual[1];
                        p.origen = fila_actual[2];
                        p.director = fila_actual[3];
                        p.cast = fila_actual[4];
                        p.genero = fila_actual[5];
                        p.sinopsis = fila_actual[7];
                        std::string texto_total = p.titulo + " " + p.sinopsis;
                        p.tokens_txt = tokenizar(normalizar(texto_total));

                        peliculas.push_back(std::move(p));
                    }
                    fila_actual.clear();
                    campo_actual = "";
                    contador_id++;
                }
            } 
            else {
                campo_actual += c;
            }
        }
    }
    archivo.close();
    return peliculas;
}