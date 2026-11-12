#include "recomendador.h"
#include <algorithm>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

namespace {
    constexpr double PESO_GENERO = 50.0;
    constexpr double PESO_DIRECTOR = 20.0;
    constexpr double PESO_ACTORES = 15.0;
    constexpr double PESO_CONTENIDO = 15.0;

    struct PerfilUsuario {
        int likes_validos = 0;

        unordered_map<string, int> generos;
        unordered_map<string, int> directores;
        unordered_map<string, int> actores;

        unordered_set<string> tokens;
    };

    struct Candidato {
        int id;
        double score;
    };

    struct PeorCandidatoPrimero {
        bool operator()(const Candidato& a, const Candidato& b) const {
            if (a.score != b.score) {
                return a.score > b.score;
            }

            return a.id < b.id;
        }
    };

    string recortar(const string& texto) {
        const string espacios = " \t\n\r";

        size_t inicio =texto.find_first_not_of(espacios);

        if (inicio == string::npos) {
            return "";
        }

        size_t fin = texto.find_last_not_of(espacios);

        return texto.substr(inicio, fin - inicio + 1);
    }

    string normalizarEtiqueta(const string& etiqueta) {
        string resultado = recortar(normalizar(etiqueta));

        if (resultado == "unknown") {
            return "";
        }

        return resultado;
    }

    vector<string> separarEtiquetas(const string& texto) {
        vector<string> etiquetas;
        unordered_set<string> valores_vistos;

        stringstream ss(texto);
        string valor;

        while (getline(ss, valor, ',')) {
            valor = normalizarEtiqueta(valor);

            if (!valor.empty() && valores_vistos.insert(valor).second) {
                etiquetas.push_back(valor);
            }
        }

        return etiquetas;
    }

    void agregarPreferencias(const Pelicula& pelicula, PerfilUsuario& perfil) {
        for (const string& genero : separarEtiquetas(pelicula.genero)) {
            perfil.generos[genero]++;
        }

        for (const string& director : separarEtiquetas(pelicula.director)) {
            perfil.directores[director]++;
        }

        for (const string& actor : separarEtiquetas(pelicula.cast)) {
            perfil.actores[actor]++;
        }

        unordered_set<string> tokens_unicos(pelicula.tokens_txt.begin(), pelicula.tokens_txt.end());

        perfil.tokens.insert(tokens_unicos.begin(), tokens_unicos.end());
    }

    PerfilUsuario construirPerfil(const vector<Pelicula>& peliculas, const unordered_set<int>& ids_liked) {
        PerfilUsuario perfil;

        for (const Pelicula& pelicula : peliculas) {
            if (!ids_liked.count(pelicula.id)) {
                continue;
            }

            perfil.likes_validos++;
            agregarPreferencias(pelicula, perfil);
        }

        return perfil;
    }

    double calcularAfinidad(const vector<string>& valores, const unordered_map<string, int>& preferencias, int total_likes) {
        if (total_likes == 0) {
            return 0.0;
        }

        int mejor_coincidencia = 0;

        for (const string& valor : valores) {
            auto it = preferencias.find(valor);

            if (it != preferencias.end()) {
                mejor_coincidencia = max(mejor_coincidencia, it->second);
            }
        }

        return static_cast<double>(mejor_coincidencia) / total_likes;
    }

    double calcularSimilitud(const Pelicula& pelicula, const PerfilUsuario& perfil) {
        if (perfil.likes_validos == 0) {
            return 0.0;
        }

        double score = 0.0;

        double afinidad_genero = calcularAfinidad(separarEtiquetas(pelicula.genero), perfil.generos, perfil.likes_validos);

        score += afinidad_genero * PESO_GENERO;

        double afinidad_director = calcularAfinidad(separarEtiquetas(pelicula.director), perfil.directores, perfil.likes_validos);

        score += afinidad_director * PESO_DIRECTOR;

        int actores_compartidos = 0;

        for (const string& actor : separarEtiquetas(pelicula.cast)) {
            if (perfil.actores.count(actor)) {
                actores_compartidos++;
            }
        }

        double afinidad_actores = min(actores_compartidos / 2.0, 1.0);

        score += afinidad_actores * PESO_ACTORES;

        unordered_set<string> tokens_coincidentes;

        for (const string& token : pelicula.tokens_txt) {
            if (perfil.tokens.count(token)) {
                tokens_coincidentes.insert(token);
            }
        }

        double afinidad_contenido = min(tokens_coincidentes.size() / 20.0, 1.0);

        score += afinidad_contenido * PESO_CONTENIDO;

        return score;
    }

    bool esMejor(const Candidato& candidato, const Candidato& peor_actual) {
        if (candidato.score != peor_actual.score) {
            return candidato.score > peor_actual.score;
        }

        return candidato.id < peor_actual.id;
    }

}

vector<Recomendacion> rankearRecomendaciones(const vector<Pelicula>& peliculas, const vector<int>& likes_ids, size_t limite) {
    if (likes_ids.empty() || limite == 0) {
        return {};
    }

    unordered_set<int> ids_liked(likes_ids.begin(), likes_ids.end());

    PerfilUsuario perfil = construirPerfil(peliculas, ids_liked);

    if (perfil.likes_validos == 0) {
        return {};
    }

    priority_queue<Candidato, vector<Candidato>, PeorCandidatoPrimero> mejores;

    for (const Pelicula& pelicula : peliculas) {
        if (ids_liked.count(pelicula.id)) {
            continue;
        }

        Candidato candidato{pelicula.id, calcularSimilitud(pelicula, perfil)};

        if (candidato.score <= 0.0) {
            continue;
        }

        if (mejores.size() < limite) {
            mejores.push(candidato);
        }
        else if (esMejor(candidato, mejores.top())) {
            mejores.pop();
            mejores.push(candidato);
        }
    }

    vector<Recomendacion> resultado;
    resultado.reserve(mejores.size());

    while (!mejores.empty()) {
        resultado.push_back({mejores.top().id, mejores.top().score});

        mejores.pop();
    }

    sort(
        resultado.begin(),
        resultado.end(),
        [](const Recomendacion& a,
           const Recomendacion& b) {

            if (a.score != b.score) {
                return a.score > b.score;
            }

            return a.id < b.id;
        }
    );

    return resultado;
}

vector<Pelicula> generarRecomendaciones(const vector<Pelicula>& peliculas, const vector<int>& likes_ids) {
    vector<Recomendacion> ranking = rankearRecomendaciones(peliculas, likes_ids, 5);

    vector<Pelicula> recomendadas;
    recomendadas.reserve(ranking.size());

    for (const Recomendacion& recomendacion : ranking) {

        int indice = recomendacion.id - 1;

        if (indice >= 0 && indice < static_cast<int>(peliculas.size()) && peliculas[indice].id == recomendacion.id) {
            recomendadas.push_back(peliculas[indice]);
            continue;
        }

        auto it = find_if(peliculas.begin(), peliculas.end(), [&](const Pelicula& pelicula) {
                return pelicula.id ==recomendacion.id;
            }
        );

        if (it != peliculas.end()) {
            recomendadas.push_back(*it);
        }
    }

    return recomendadas;
}