#include "interfaz.h"
#include <iostream>
#include <limits>
#include <algorithm>
using namespace std;
Interfaz::Interfaz(const vector<Pelicula>& peliculas,SuffixTrie* trie):peliculas(peliculas), trie(trie) {}
// ============================================================
// INICIAR
// ============================================================
void Interfaz::iniciar() {
    int opcion;
    do {
        mostrarInicio();
        cout << endl;
        cout << "========================================" << endl;
        cout << "              MENU PRINCIPAL" << endl;
        cout << "========================================" << endl;
        mostrarMenu();
        opcion = leerOpcion();
        switch (opcion) {
            case 1:
                buscarPeliculas();
                break;
            case 2:
                buscarPorTag();
                break;
            case 3:
                mostrarVerMasTarde();
                break;
            case 4:
                cout << endl;
                cout << "Saliendo del programa..." << endl;
                break;
            default:
                cout << endl;
                cout << "Opcion no valida." << endl;
                pausar();
                break;
        }
    } while (opcion != 4);
}
// ============================================================
// PANTALLA DE INICIO
// ============================================================
void Interfaz::mostrarInicio() {
    limpiarPantalla();
    cout << "========================================" << endl;
    cout << "       PLATAFORMA DE STREAMING" << endl;
    cout << "========================================" << endl;
    cout << endl;
    cout << "Peliculas disponibles: " << peliculas.size() << endl;
    cout << endl;
    cout << "Bienvenido a la plataforma." << endl;

    //--------Seccion de recomendacion-----
    vector<int> mis_likes = cargarLikes();
    vector<Pelicula> recomendadas = generarRecomendaciones(peliculas, mis_likes);

    if (!recomendadas.empty()) {
        cout << "--- RECOMENDADAS PARA TI ---" << endl;
        cout << "Basado en tu interes por el genero: " << recomendadas[0].genero << endl;
        for (const Pelicula& r : recomendadas) {
            cout << "- " << r.titulo << " (" << r.anho << ")" << endl;
        }
        cout << "----------------------------" << endl;
        cout << endl;
    } else {
        cout << "-> Dale 'Like' a algunas peliculas para recibir recomendaciones personalizadas." << endl;
        cout << endl;
    }
    // ---------
}
// ============================================================
// MENU
// ============================================================
void Interfaz::mostrarMenu() {
    cout << endl;
    cout << "1. Buscar pelicula" << endl;
    cout << "2. Buscar por tag" << endl;
    cout << "3. Ver \"Ver mas tarde\"" << endl;
    cout << "4. Salir" << endl;
    cout << endl;
}
// ============================================================
// BUSQUEDA NORMAL
// ============================================================
void Interfaz::buscarPeliculas() {
    limpiarPantalla();
    cout << "========================================" << endl;
    cout << "                BUSCAR" << endl;
    cout << "========================================" << endl;
    cout << endl;
    cout << "Puede buscar una palabra, frase o sub-palabra." << endl;
    cout << endl;
    string consulta;
    cout << "Ingrese su busqueda: ";
    getline(cin, consulta);
    if (consulta.empty()) {
        cout << endl;
        cout << "La consulta no puede estar vacia." << endl;
        pausar();
        return;
    }
    vector<ResultadoBusqueda> resultados = buscar(*trie, peliculas, consulta);
    if (resultados.empty()) {
        cout << endl;
        cout << "No se encontraron peliculas." << endl;
        pausar();
        return;
    }
    mostrarResultados(resultados, consulta);
}
// ============================================================
// MOSTRAR RESULTADOS
// ============================================================
void Interfaz::mostrarResultados(const vector<ResultadoBusqueda>& resultados,const string& consulta) {
    int pagina = 0;
    const int TAMANO_PAGINA = 5;
    while (true) {
        limpiarPantalla();
        cout << "========================================" << endl;
        cout << "              RESULTADOS" << endl;
        cout << "========================================" << endl;
        cout << endl;
        cout << "Busqueda: " << consulta << endl;
        cout << "Coincidencias: " << resultados.size() << endl;
        cout << endl;
        int inicio = pagina * TAMANO_PAGINA;
        int fin = min(inicio + TAMANO_PAGINA,static_cast<int>(resultados.size()));
        // ----------------------------------------------------
        // MOSTRAR LAS 5 PELICULAS
        // ----------------------------------------------------
        for (int i = inicio; i < fin; ++i) {
            int id = resultados[i].id;
            Pelicula* pelicula = obtenerPeliculaPorId(id);
            if (pelicula == nullptr) {
                continue;
            }
            cout << i - inicio + 1 << ". " << pelicula->titulo << endl;
            cout << "   Anho: " << pelicula->anho << endl;
            cout << "   Genero: " << pelicula->genero << endl;
            cout << endl;
        }
        cout << "----------------------------------------" << endl;
        cout << "Seleccione una pelicula: 1-5" << endl;
        if (fin < static_cast<int>(resultados.size())) {
            cout << "6. Siguiente pagina" << endl;
        }
        if (pagina > 0) {
            cout << "7. Pagina anterior" << endl;
        }
        cout << "0. Volver" << endl;
        cout << endl;
        int opcion = leerOpcion();
        // ----------------------------------------------------
        // VOLVER
        // ----------------------------------------------------
        if (opcion == 0) {
            return;
        }
        // ----------------------------------------------------
        // SELECCIONAR PELICULA
        // ----------------------------------------------------
        if (opcion >= 1 && opcion <= 5) {
            int posicion = inicio + opcion - 1;
            if (posicion < fin) {
                Pelicula* pelicula = obtenerPeliculaPorId(resultados[posicion].id);
                if (pelicula != nullptr) {
                    mostrarDetalle(*pelicula);
                }
            }
            else {
                cout << endl;
                cout << "No existe esa pelicula." << endl;
                pausar();
            }
            continue;
        }
        // ----------------------------------------------------
        // SIGUIENTE PAGINA
        // ----------------------------------------------------
        if (opcion == 6 && fin < static_cast<int>(resultados.size())) {
            pagina++;
            continue;
        }
        // ----------------------------------------------------
        // PAGINA ANTERIOR
        // ----------------------------------------------------
        if (opcion == 7 && pagina > 0) {
            pagina--;
            continue;
        }
        cout << endl;
        cout << "Opcion no valida." << endl;
        pausar();
    }
}
// ============================================================
// DETALLE DE PELICULA
// ============================================================
void Interfaz::mostrarDetalle(const Pelicula& pelicula) {
    while (true) {
        limpiarPantalla();
        cout << "========================================" << endl;
        cout << "          INFORMACION DE PELICULA" << endl;
        cout << "========================================" << endl;
        cout << endl;
        cout << "Titulo: " << pelicula.titulo << endl;
        cout << "Anho: " << pelicula.anho << endl;
        cout << "Origen: " << pelicula.origen << endl;
        cout << "Director: " << pelicula.director << endl;
        cout << "Genero: " << pelicula.genero << endl;
        cout << "Cast: " << pelicula.cast << endl;
        cout << endl;
        cout << "----------------------------------------" << endl;
        cout << "SINOPSIS" << endl;
        cout << "----------------------------------------" << endl;
        cout << pelicula.sinopsis << endl;
        cout << endl;
        cout << "----------------------------------------" << endl;
        cout << "1. Like" << endl;
        cout << "2. Ver mas tarde" << endl;
        cout << "3. Volver" << endl;
        cout << endl;
        int opcion = leerOpcion();
        switch (opcion) {
            case 1:
                cout << endl;

                guardarLike(pelicula.id);

                pausar();
                break;
            case 2:
                cout << endl;

                guardarVerMasTarde(pelicula.id);

                pausar();
                break;
            case 3:
                return;
            default:
                cout << endl;
                cout << "Opcion no valida." << endl;
                pausar();
                break;
        }
    }
}
// ============================================================
// BUSQUEDA POR TAG
// ============================================================
void Interfaz::buscarPorTag() {
    limpiarPantalla();
    cout << "========================================" << endl;
    cout << "             BUSQUEDA POR TAG" << endl;
    cout << "========================================" << endl;
    cout << endl;
    cout << "1. Director" << endl;
    cout << "2. Cast" << endl;
    cout << "3. Genero" << endl;
    cout << "0. Volver" << endl;
    cout << endl;
    int opcion = leerOpcion();
    if (opcion == 0) {
        return;
    }
    string consulta;
    cout << endl;
    cout << "Ingrese el valor a buscar: ";
    getline(cin, consulta);
    if (consulta.empty()) {
        cout << endl;
        cout << "La busqueda no puede estar vacia." << endl;
        pausar();
        return;
    }
    vector<ResultadoBusqueda> resultados;
    // --------------------------------------------------------
    // DIRECTOR
    // --------------------------------------------------------
    if (opcion == 1) {
        for (const Pelicula& pelicula : peliculas) {
            string director = normalizar(pelicula.director);
            string busqueda = normalizar(consulta);
            if (director.find(busqueda) != string::npos) {
                resultados.push_back({pelicula.id,1.0});
            }
        }
    }
    // --------------------------------------------------------
    // CAST
    // --------------------------------------------------------
    else if (opcion == 2) {
        for (const Pelicula& pelicula : peliculas) {
            string cast = normalizar(pelicula.cast);
            string busqueda = normalizar(consulta);
            if (cast.find(busqueda) != string::npos) {
                resultados.push_back({pelicula.id,1.0});
            }
        }
    }
    // --------------------------------------------------------
    // GENERO
    // --------------------------------------------------------
    else if (opcion == 3) {
        for (const Pelicula& pelicula : peliculas) {
            string genero = normalizar(pelicula.genero);
            string busqueda = normalizar(consulta);
            if (genero.find(busqueda) != string::npos) {
                resultados.push_back({pelicula.id,1.0});
            }
        }
    }
    else {
        cout << endl;
        cout << "Opcion no valida." << endl;
        pausar();
        return;
    }
    if (resultados.empty()) {
        cout << endl;
        cout << "No se encontraron peliculas." << endl;
        pausar();
        return;
    }
    mostrarResultados(
        resultados,
        consulta
    );
}
// ============================================================
// VER MAS TARDE
// ============================================================
void Interfaz::mostrarVerMasTarde() {
    limpiarPantalla();
    cout << "========================================" << endl;
    cout << "             VER MAS TARDE" << endl;
    cout << "========================================" << endl;
    cout << endl;

    vector<int> ids = cargarVerMasTarde();
    //itera por cada id guardado y muestra sus titulos, etc.
    if (ids.empty()) {
        cout << "No tienes peliculas guardadas en esta lista." << endl;
    } else {
        for (size_t i = 0; i < ids.size(); ++i) {
            Pelicula* p = obtenerPeliculaPorId(ids[i]);
            if (p != nullptr) {
                cout << (i + 1) << ". " << p->titulo << " (" << p->anho << ") - " << p->director << endl;
            }
        }
    }
    pausar();
}
// ============================================================
// BUSCAR PELICULA POR ID
// ============================================================
Pelicula* Interfaz::obtenerPeliculaPorId(int id) {
    for (Pelicula& pelicula : peliculas) {
        if (pelicula.id == id) {
            return &pelicula;
        }
    }
    return nullptr;
}
// ============================================================
// LIMPIAR PANTALLA
// ============================================================
void Interfaz::limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
// ============================================================
// PAUSAR
// ============================================================
void Interfaz::pausar() {
    cout << endl;
    cout << "Presione ENTER para continuar...";
    cin.get();
}
// ============================================================
// LEER OPCION
// ============================================================
int Interfaz::leerOpcion() {
    int opcion;
    while (true) {
        cout << "Seleccione una opcion: ";
        if (cin >> opcion) {
            cin.ignore(numeric_limits<streamsize>::max(),'\n');
            return opcion;
        }
        cout << endl;
        cout << "Ingrese un numero valido." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(),'\n');
    }
}