#include "ranking.h"

double calcularPuntajeRanking(const FactoresRanking& factores, const PesosRanking& pesos) {
    double puntaje_cobertura = factores.cobertura * pesos.cobertura;

    double puntaje_frecuencia = factores.frecuencia * pesos.frecuencia;

    double puntaje_titulo = factores.coincidenciasTitulo * pesos.titulo;

    return puntaje_cobertura + puntaje_frecuencia + puntaje_titulo;
}
