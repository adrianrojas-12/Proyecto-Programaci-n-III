#include "patrones.h"
#include <algorithm>
#include <sstream>
#include "cargador_csv.h"
#include "buscador_paralelo.h"
#include "persistencia.h"
#include "recomendador.h"
using namespace std;

// Patron: SINGLETON
GestorDatos& GestorDatos::instancia() {
    static GestorDatos unica_instancia;
    return unica_instancia;
}

bool GestorDatos::cargar(const string& rutaCSV, unsigned hilosIndexado) {
    lock_guard<mutex> lock(mutexCarga_);

    if (cargado_) return true;

    peliculas_ = cargar_peliculas(rutaCSV);
    if (peliculas_.empty()) return false;

    trie_ = make_unique<SuffixTrie>(2);
    trie_->construirIndiceParalelo(peliculas_, hilosIndexado);

    cargado_ = true;
    return true;
}

bool GestorDatos::estaCargado() const {
    lock_guard<mutex> lock(mutexCarga_);
    return cargado_;
}

const vector<Pelicula>& GestorDatos::peliculas() const { return peliculas_; }
const SuffixTrie& GestorDatos::trie() const { return *trie_; }

// Patron: STRATEGY
EstrategiaBusquedaTexto::EstrategiaBusquedaTexto(const SuffixTrie& trie,
                                                  const vector<Pelicula>& peliculas,
                                                  unsigned hilos)
    : trie_(trie), peliculas_(peliculas), hilos_(hilos) {}

vector<ResultadoBusqueda> EstrategiaBusquedaTexto::buscar(const string& consulta) const {
    return buscarParalelo(trie_, peliculas_, consulta, hilos_);
}

string EstrategiaBusquedaTexto::nombre() const {
    return "Texto libre (titulo + sinopsis)";
}

EstrategiaBusquedaTag::EstrategiaBusquedaTag(const vector<Pelicula>& peliculas, TipoTag tipo)
    : peliculas_(peliculas), tipo_(tipo) {}

const string& EstrategiaBusquedaTag::campoDe(const Pelicula& pelicula) const {
    switch (tipo_) {
        case TipoTag::DIRECTOR: return pelicula.director;
        case TipoTag::CAST:     return pelicula.cast;
        case TipoTag::GENERO:
        default:                return pelicula.genero;
    }
}

vector<ResultadoBusqueda> EstrategiaBusquedaTag::buscar(const string& consulta) const {
    vector<ResultadoBusqueda> resultados;
    string objetivo = normalizar(consulta);
    if (objetivo.empty()) return resultados;

    for (const Pelicula& pelicula : peliculas_) {
        string campo = normalizar(campoDe(pelicula));
        if (!campo.empty() && campo.find(objetivo) != string::npos) {
            resultados.push_back({pelicula.id, 1.0});
        }
    }
    return resultados;
}

string EstrategiaBusquedaTag::nombre() const {
    switch (tipo_) {
        case TipoTag::DIRECTOR: return "Tag: Director";
        case TipoTag::CAST:     return "Tag: Cast";
        case TipoTag::GENERO:
        default:                return "Tag: Genero";
    }
}

// Patron: FACTORY
unique_ptr<IEstrategiaBusqueda> FabricaEstrategiaBusqueda::crear(
    TipoBusqueda tipo,
    const SuffixTrie& trie,
    const vector<Pelicula>& peliculas,
    unsigned hilosBusquedaTexto) {

    switch (tipo) {
        case TipoBusqueda::TEXTO:
            return make_unique<EstrategiaBusquedaTexto>(trie, peliculas, hilosBusquedaTexto);
        case TipoBusqueda::TAG_DIRECTOR:
            return make_unique<EstrategiaBusquedaTag>(peliculas, TipoTag::DIRECTOR);
        case TipoBusqueda::TAG_CAST:
            return make_unique<EstrategiaBusquedaTag>(peliculas, TipoTag::CAST);
        case TipoBusqueda::TAG_GENERO:
            return make_unique<EstrategiaBusquedaTag>(peliculas, TipoTag::GENERO);
    }
    return nullptr;
}

// Patron: OBSERVER
void SujetoLikes::suscribir(IObservadorLike* observador) {
    if (observador == nullptr) return;
    observadores_.push_back(observador);
}

void SujetoLikes::desuscribir(IObservadorLike* observador) {
    observadores_.erase(
        remove(observadores_.begin(), observadores_.end(), observador),
        observadores_.end());
}

void SujetoLikes::notificar(const Pelicula& pelicula) {
    for (IObservadorLike* observador : observadores_) {
        observador->alRecibirLike(pelicula);
    }
}

void ObservadorRecomendaciones::alRecibirLike(const Pelicula&) {
    necesitaRecalcular_ = true;
}

bool ObservadorRecomendaciones::necesitaRecalcular() const { return necesitaRecalcular_; }

void ObservadorRecomendaciones::actualizarCache(vector<Pelicula> nuevasRecomendaciones) {
    cache_ = move(nuevasRecomendaciones);
    necesitaRecalcular_ = false;
}

const vector<Pelicula>& ObservadorRecomendaciones::obtenerCache() const { return cache_; }

namespace {
    vector<string> separarGeneros(const string& generoCrudo) {
        vector<string> generos;
        string normalizado = normalizar(generoCrudo);
        if (normalizado.empty() || normalizado == "unknown") return generos;

        stringstream ss(normalizado);
        string parte;
        while (ss >> parte) generos.push_back(parte);
        return generos;
    }
}

void ObservadorEstadisticas::alRecibirLike(const Pelicula& pelicula) {
    ++totalLikes_;
    for (const string& genero : separarGeneros(pelicula.genero)) {
        ++conteoPorGenero_[genero];
    }
}

int ObservadorEstadisticas::totalLikes() const { return totalLikes_; }

string ObservadorEstadisticas::generoFavorito() const {
    if (conteoPorGenero_.empty()) return "";
    auto mejor = conteoPorGenero_.begin();
    for (auto it = conteoPorGenero_.begin(); it != conteoPorGenero_.end(); ++it) {
        if (it->second > mejor->second) mejor = it;
    }
    return mejor->first;
}

// Patron: FACADE
ControladorApp::ControladorApp(string rutaCSV, unsigned hilosCarga)
    : rutaCSV_(move(rutaCSV)), hilosCarga_(hilosCarga) {
    sujetoLikes_.suscribir(&obsRecomendaciones_);
    sujetoLikes_.suscribir(&obsEstadisticas_);
}

bool ControladorApp::inicializar() {
    return GestorDatos::instancia().cargar(rutaCSV_, hilosCarga_);
}

vector<ResultadoBusqueda> ControladorApp::buscarTexto(const string& consulta) const {
    const GestorDatos& datos = GestorDatos::instancia();
    auto estrategia = FabricaEstrategiaBusqueda::crear(
        TipoBusqueda::TEXTO, datos.trie(), datos.peliculas(), hilosCarga_);
    return estrategia->buscar(consulta);
}

vector<ResultadoBusqueda> ControladorApp::buscarPorTag(TipoTag tipo, const string& consulta) const {
    const GestorDatos& datos = GestorDatos::instancia();

    TipoBusqueda tipoBusqueda;
    switch (tipo) {
        case TipoTag::DIRECTOR: tipoBusqueda = TipoBusqueda::TAG_DIRECTOR; break;
        case TipoTag::CAST:     tipoBusqueda = TipoBusqueda::TAG_CAST;     break;
        case TipoTag::GENERO:
        default:                tipoBusqueda = TipoBusqueda::TAG_GENERO;  break;
    }

    auto estrategia = FabricaEstrategiaBusqueda::crear(
        tipoBusqueda, datos.trie(), datos.peliculas(), hilosCarga_);
    return estrategia->buscar(consulta);
}

const Pelicula* ControladorApp::obtenerPeliculaPorId(int id) const {
    const vector<Pelicula>& peliculas = GestorDatos::instancia().peliculas();

    if (id >= 1 && id <= static_cast<int>(peliculas.size()) && peliculas[id - 1].id == id) {
        return &peliculas[id - 1];
    }
    auto it = find_if(peliculas.begin(), peliculas.end(),
                       [id](const Pelicula& p) { return p.id == id; });
    return it != peliculas.end() ? &(*it) : nullptr;
}

size_t ControladorApp::totalPeliculas() const {
    return GestorDatos::instancia().peliculas().size();
}

bool ControladorApp::darLike(int id) {
    const Pelicula* pelicula = obtenerPeliculaPorId(id);
    if (pelicula == nullptr) return false;

    bool esNuevo = guardarLike(id);
    if (esNuevo) sujetoLikes_.notificar(*pelicula);
    return esNuevo;
}

bool ControladorApp::agregarVerMasTarde(int id) {
    if (obtenerPeliculaPorId(id) == nullptr) return false;
    return guardarVerMasTarde(id);
}

vector<const Pelicula*> ControladorApp::obtenerVerMasTarde() const {
    vector<const Pelicula*> resultado;
    for (int id : cargarVerMasTarde()) {
        const Pelicula* pelicula = obtenerPeliculaPorId(id);
        if (pelicula != nullptr) resultado.push_back(pelicula);
    }
    return resultado;
}

const vector<Pelicula>& ControladorApp::obtenerRecomendaciones() {
    if (obsRecomendaciones_.necesitaRecalcular()) {
        vector<int> likes = cargarLikes();
        const vector<Pelicula>& peliculas = GestorDatos::instancia().peliculas();
        obsRecomendaciones_.actualizarCache(generarRecomendaciones(peliculas, likes));
    }
    return obsRecomendaciones_.obtenerCache();
}

int ControladorApp::totalLikesRegistrados() const { return obsEstadisticas_.totalLikes(); }
string ControladorApp::generoFavoritoDelUsuario() const { return obsEstadisticas_.generoFavorito(); }

void ControladorApp::suscribirObservadorLike(IObservadorLike* observador) {
    sujetoLikes_.suscribir(observador);
}