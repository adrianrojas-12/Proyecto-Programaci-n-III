#ifndef RANKING_H
#define RANKING_H

struct FactoresRanking {
    int cobertura = 0;
    int frecuencia = 0;
    int coincidenciasTitulo = 0;
};

struct PesosRanking {
    double cobertura = 50.0;
    double frecuencia = 1.0;
    double titulo = 30.0;
};

double calcularPuntajeRanking(const FactoresRanking& factores, const PesosRanking& pesos = PesosRanking{});

#endif // RANKING_H
