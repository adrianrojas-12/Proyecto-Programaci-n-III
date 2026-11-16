#ifndef PATRONES_H
#define PATRONES_H

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "preprocesador.h"
#include "trie.h"
#include "buscador.h"

using namespace std;

// Patron: SINGLETON
class GestorDatos {
public:
    static GestorDatos& instancia();

    GestorDatos(const GestorDatos&) = delete;
    GestorDatos& operator=(const GestorDatos&) = delete;

    bool cargar(const string& rutaCSV, unsigned hilosIndexado = 4);
    bool estaCargado() const;

    const vector<Pelicula>& peliculas() const;
    const SuffixTrie& trie() const;

private:
    GestorDatos() = default;
    ~GestorDatos() = default;

    mutable mutex mutexCarga_;
    bool cargado_ = false;
    vector<Pelicula> peliculas_;
    unique_ptr<SuffixTrie> trie_;
};

// Patron: STRATEGY
enum class TipoTag { DIRECTOR, CAST, GENERO };

class IEstrategiaBusqueda {
public:
    virtual ~IEstrategiaBusqueda() = default;
    virtual vector<ResultadoBusqueda> buscar(const string& consulta) const = 0;
    virtual string nombre() const = 0;
};

class EstrategiaBusquedaTexto : public IEstrategiaBusqueda {
public:
    EstrategiaBusquedaTexto(const SuffixTrie& trie,
                             const vector<Pelicula>& peliculas,
                             unsigned hilos = 4);
    vector<ResultadoBusqueda> buscar(const string& consulta) const override;
    string nombre() const override;

private:
    const SuffixTrie& trie_;
    const vector<Pelicula>& peliculas_;
    unsigned hilos_;
};

class EstrategiaBusquedaTag : public IEstrategiaBusqueda {
public:
    EstrategiaBusquedaTag(const vector<Pelicula>& peliculas, TipoTag tipo);
    vector<ResultadoBusqueda> buscar(const string& consulta) const override;
    string nombre() const override;

private:
    const string& campoDe(const Pelicula& pelicula) const;
    const vector<Pelicula>& peliculas_;
    TipoTag tipo_;
};

// Patron: FACTORY
enum class TipoBusqueda { TEXTO, TAG_DIRECTOR, TAG_CAST, TAG_GENERO };

class FabricaEstrategiaBusqueda {
public:
    static unique_ptr<IEstrategiaBusqueda> crear(
        TipoBusqueda tipo,
        const SuffixTrie& trie,
        const vector<Pelicula>& peliculas,
        unsigned hilosBusquedaTexto = 4);
};

// Patron: OBSERVER
class IObservadorLike {
public:
    virtual ~IObservadorLike() = default;
    virtual void alRecibirLike(const Pelicula& pelicula) = 0;
};

class SujetoLikes {
public:
    void suscribir(IObservadorLike* observador);
    void desuscribir(IObservadorLike* observador);
    void notificar(const Pelicula& pelicula);

private:
    vector<IObservadorLike*> observadores_;
};

class ObservadorRecomendaciones : public IObservadorLike {
public:
    void alRecibirLike(const Pelicula& pelicula) override;
    bool necesitaRecalcular() const;
    void actualizarCache(vector<Pelicula> nuevasRecomendaciones);
    const vector<Pelicula>& obtenerCache() const;

private:
    bool necesitaRecalcular_ = true;
    vector<Pelicula> cache_;
};

class ObservadorEstadisticas : public IObservadorLike {
public:
    void alRecibirLike(const Pelicula& pelicula) override;
    int totalLikes() const;
    string generoFavorito() const;

private:
    int totalLikes_ = 0;
    unordered_map<string, int> conteoPorGenero_;
};

// Patron: FACADE
class ControladorApp {
public:
    explicit ControladorApp(string rutaCSV, unsigned hilosCarga = 4);

    bool inicializar();

    vector<ResultadoBusqueda> buscarTexto(const string& consulta) const;
    vector<ResultadoBusqueda> buscarPorTag(TipoTag tipo, const string& consulta) const;

    const Pelicula* obtenerPeliculaPorId(int id) const;
    size_t totalPeliculas() const;

    bool darLike(int id);
    bool agregarVerMasTarde(int id);
    vector<const Pelicula*> obtenerVerMasTarde() const;

    const vector<Pelicula>& obtenerRecomendaciones();

    int totalLikesRegistrados() const;
    string generoFavoritoDelUsuario() const;

    void suscribirObservadorLike(IObservadorLike* observador);

private:
    string rutaCSV_;
    unsigned hilosCarga_;

    SujetoLikes sujetoLikes_;
    ObservadorRecomendaciones obsRecomendaciones_;
    ObservadorEstadisticas obsEstadisticas_;
};

#endif  // PATRONES_H