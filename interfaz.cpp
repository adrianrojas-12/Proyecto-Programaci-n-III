#include "interfaz.h"
#include <iostream>
#include <limits>
#include <algorithm>

using namespace std;

Interfaz::Interfaz(ControladorApp& controlador) : controlador_(controlador) {}

void Interfaz::iniciar() {
    int opcion = 0;
    do {
        limpiarPantalla();
        mostrarInicio();
        cout << endl;
        cout << "========================================" << endl;
        cout << "             MENU PRINCIPAL             " << endl;
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
                cout << endl << "Saliendo del programa..." << endl;
                break;
            default:
                cout << endl << "Opcion no valida." << endl;
                pausar();
                break;
        }
    } while (opcion != 4);
}

void Interfaz::mostrarInicio() {
    cout << "========================================" << endl;
    cout << "        PLATAFORMA DE STREAMING         " << endl;
    cout << "========================================" << endl << endl;
    cout << "Peliculas disponibles: " << controlador_.totalPeliculas() << endl << endl;

    const vector<Pelicula>& recomendadas = controlador_.obtenerRecomendaciones();

    if (!recomendadas.empty()) {
        cout << "--- RECOMENDADAS PARA TI ---" << endl;
        cout << "Basado en la similitud con tus Likes:" << endl;

        for (const Pelicula& pelicula : recomendadas) {
            cout << " * " << pelicula.titulo << " (" << pelicula.anho << ")" << endl;
        }
        cout << "----------------------------" << endl;
    }
    else {
        cout << "-> Dale 'Like' a algunas peliculas " << "para recibir recomendaciones." << endl;
    }
}

void Interfaz::mostrarMenu() {
    cout << "1. Buscar pelicula" << endl;
    cout << "2. Buscar por tag" << endl;
    cout << "3. Ver \"Ver mas tarde\"" << endl;
    cout << "4. Salir" << endl << endl;
}

// BUSQUEDA GENERAL POR PALABRA / FRASE / SUB-PALABRA
void Interfaz::buscarPeliculas() {
    while (true) {
        limpiarPantalla();
        cout << "========================================" << endl;
        cout << "                BUSCAR                  " << endl;
        cout << "========================================" << endl;
        cout << "Puede buscar una palabra, frase o sub-palabra." << endl;
        cout << "(Ingrese '0' para regresar al Menu Principal)" << endl << endl;

        string consulta;
        cout << "Ingrese su busqueda: ";
        getline(cin, consulta);

        if (consulta.empty() || consulta == "0") {
            return;
        }

        vector<ResultadoBusqueda> resultados = controlador_.buscarTexto(consulta);
        if (resultados.empty()) {
            cout << endl << "No se encontraron peliculas para '" << consulta << "'." << endl;
            pausar();
            continue;
        }

        mostrarResultados(resultados, consulta);
    }
}

// BUSQUEDA POR TAG (DIRECTOR, CAST, GENERO) -- ahora via Strategy + Factory
void Interfaz::buscarPorTag() {
    while (true) {
        limpiarPantalla();
        cout << "========================================" << endl;
        cout << "           BUSQUEDA POR TAG             " << endl;
        cout << "========================================" << endl << endl;
        cout << "1. Director" << endl;
        cout << "2. Cast" << endl;
        cout << "3. Genero" << endl;
        cout << "0. Volver al Menu Principal" << endl << endl;

        int opcion = leerOpcion();

        if (opcion == 0) {
            return;
        }

        if (opcion < 1 || opcion > 3) {
            cout << endl << "Opcion no valida." << endl;
            pausar();
            continue;
        }

        string consulta;
        cout << endl << "Ingrese el valor a buscar (o '0' para regresar): ";
        getline(cin, consulta);

        if (consulta.empty() || consulta == "0") {
            continue;
        }

        TipoTag tipo = (opcion == 1) ? TipoTag::DIRECTOR
                     : (opcion == 2) ? TipoTag::CAST
                                     : TipoTag::GENERO;

        vector<ResultadoBusqueda> resultados = controlador_.buscarPorTag(tipo, consulta);

        if (resultados.empty()) {
            cout << endl << "No se encontraron coincidencias para '" << consulta << "'." << endl;
            pausar();
            continue;
        }

        mostrarResultados(resultados, consulta);
    }
}

void Interfaz::mostrarResultados(const vector<ResultadoBusqueda>& resultados, const string& consulta) {
    int pagina = 0;
    const int TAMANO_PAGINA = 5;

    while (true) {
        limpiarPantalla();
        cout << "========================================" << endl;
        cout << "              RESULTADOS                " << endl;
        cout << "========================================" << endl;
        cout << "Busqueda: '" << consulta << "'" << endl;
        cout << "Coincidencias totales: " << resultados.size() << endl << endl;

        int inicio = pagina * TAMANO_PAGINA;
        int fin = min(inicio + TAMANO_PAGINA, static_cast<int>(resultados.size()));

        for (int i = inicio; i < fin; ++i) {
            int num_opcion = (i - inicio) + 1;
            const Pelicula* p = controlador_.obtenerPeliculaPorId(resultados[i].id);
            if (p) {
                cout << num_opcion << ". " << p->titulo << " (" << p->anho << ")" << endl;
                cout << "   Director: " << p->director << endl;
                cout << "   Genero:   " << p->genero << " [Score: " << resultados[i].score << "]" << endl << endl;
            }
        }

        cout << "----------------------------------------" << endl;
        cout << "Seleccione una pelicula (1-" << (fin - inicio) << ")" << endl;
        if (fin < static_cast<int>(resultados.size())) {
            cout << "6. Siguiente pagina" << endl;
        }
        if (pagina > 0) {
            cout << "7. Pagina anterior" << endl;
        }
        cout << "0. Volver a ingresar una busqueda" << endl << endl;

        int opcion = leerOpcion();

        if (opcion == 0) {
            return;
        }

        if (opcion >= 1 && opcion <= (fin - inicio)) {
            int indice_real = inicio + (opcion - 1);
            const Pelicula* p = controlador_.obtenerPeliculaPorId(resultados[indice_real].id);
            if (p) {
                mostrarDetalle(*p);
            }
            continue;
        }

        if (opcion == 6 && fin < static_cast<int>(resultados.size())) {
            pagina++;
            continue;
        }

        if (opcion == 7 && pagina > 0) {
            pagina--;
            continue;
        }

        cout << endl << "Opcion no valida." << endl;
        pausar();
    }
}

void Interfaz::mostrarDetalle(const Pelicula& pelicula) {
    while (true) {
        limpiarPantalla();
        cout << "========================================" << endl;
        cout << "        INFORMACION DE PELICULA         " << endl;
        cout << "========================================" << endl << endl;
        cout << "Titulo:   " << pelicula.titulo << endl;
        cout << "Anho:     " << pelicula.anho << endl;
        cout << "Origen:   " << pelicula.origen << endl;
        cout << "Director: " << pelicula.director << endl;
        cout << "Genero:   " << pelicula.genero << endl;
        cout << "Cast:     " << pelicula.cast << endl << endl;
        cout << "--- SINOPSIS ---" << endl;
        cout << pelicula.sinopsis << endl << endl;
        cout << "----------------------------------------" << endl;
        cout << "1. Like" << endl;
        cout << "2. Ver mas tarde" << endl;
        cout << "0. Volver a lista de resultados" << endl << endl;

        int opcion = leerOpcion();
        if (opcion == 1) {
            controlador_.darLike(pelicula.id);
            cout << endl << "Pelicula agregada a tus Likes!" << endl;
            pausar();
        } else if (opcion == 2) {
            controlador_.agregarVerMasTarde(pelicula.id);
            cout << endl << "Pelicula agregada a 'Ver mas tarde'!" << endl;
            pausar();
        } else if (opcion == 0) {
            return;
        } else {
            cout << endl << "Opcion no valida." << endl;
            pausar();
        }
    }
}

void Interfaz::mostrarVerMasTarde() {
    limpiarPantalla();
    cout << "========================================" << endl;
    cout << "            VER MAS TARDE               " << endl;
    cout << "========================================" << endl << endl;

    vector<const Pelicula*> lista_validos = controlador_.obtenerVerMasTarde();

    if (lista_validos.empty()) {
        cout << "No tienes peliculas guardadas en esta lista." << endl << endl;
        pausar();
        return;
    }

    for (size_t i = 0; i < lista_validos.size(); ++i) {
        cout << (i + 1) << ". " << lista_validos[i]->titulo
             << " (" << lista_validos[i]->anho << ") - Dir: "
             << lista_validos[i]->director << endl;
    }

    cout << endl << "----------------------------------------" << endl;
    cout << "Seleccione una pelicula (1-" << lista_validos.size() << ") para ver detalle" << endl;
    cout << "0. Volver al Menu Principal" << endl << endl;

    while (true) {
        int opcion = leerOpcion();
        if (opcion == 0) {
            return;
        }

        if (opcion >= 1 && static_cast<size_t>(opcion) <= lista_validos.size()) {
            mostrarDetalle(*lista_validos[opcion - 1]);
            return;
        }

        cout << "Opcion no valida. Seleccione un numero entre 1 y "
             << lista_validos.size() << " (o 0 para salir): ";
    }
}

void Interfaz::limpiarPantalla() const {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void Interfaz::pausar() const {
    cout << endl << "Presione ENTER para continuar...";
    cin.get();
}

int Interfaz::leerOpcion() const {
    int opcion;
    while (true) {
        cout << "Seleccione una opcion: ";
        if (cin >> opcion) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return opcion;
        }
        cout << "Ingrese un numero valido." << endl;
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}