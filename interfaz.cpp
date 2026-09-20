#include "interfaz.h"
#include <iostream>
#include <limits>
using namespace std;
Interfaz::Interfaz(const vector<Pelicula>& peliculas):peliculas(peliculas) {}
void Interfaz::iniciar() {
    int opcion;
    do {
        limpiarPantalla();
        cout << "========================================" << endl;
        cout << "       PLATAFORMA DE STREAMING" << endl;
        cout << "========================================" << endl;
        cout << endl;
        mostrarMenu();
        cout << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        switch (opcion) {
            case 1:
                buscarPeliculas();
                break;
            case 2:
                mostrarVerMasTarde();
                break;
            case 3:
                cout << endl;
                cout << "Saliendo del programa..." << endl;
                break;
            default:
                cout << endl;
                cout << "Opcion no valida." << endl;
                pausar();
                break;
        }
    } while (opcion != 3);
}
void Interfaz::mostrarMenu() {
    cout << "1. Buscar pelicula" << endl;
    cout << "2. Ver \"Ver mas tarde\"" << endl;
    cout << "3. Salir" << endl;
}
void Interfaz::buscarPeliculas() {
    limpiarPantalla();
    cout << "========================================" << endl;
    cout << "               BUSCAR" << endl;
    cout << "========================================" << endl;
    cout << endl;
    string consulta;
    cout << "Ingrese palabra, frase o sub-palabra: ";
    getline(cin, consulta);
    if (consulta.empty()) {
        cout << endl;
        cout << "La consulta no puede estar vacia." << endl;
        pausar();
        return;
    }
    /*
        Aqui se llamara a la funcion de Persona 3.
        Por ejemplo:
        vector<Resultado> resultados = buscar(raiz, consulta);
        Por ahora usamos un vector vacio.
    */
    vector<Pelicula> resultados;
    mostrarResultados(resultados, consulta);
}
void Interfaz::mostrarResultados(
    const vector<Pelicula>& resultados,
    const string& consulta) {
    int pagina = 0;
    const int tamanoPagina = 5;
    while (true) {
        limpiarPantalla();
        cout << "========================================" << endl;
        cout << "             RESULTADOS" << endl;
        cout << "========================================" << endl;
        cout << endl;
        cout << "Busqueda: " << consulta << endl;
        cout << endl;
        int inicio = pagina * tamanoPagina;
        int fin = inicio + tamanoPagina;
        if (inicio >= resultados.size()) {
            cout << "No hay resultados para mostrar." << endl;
        }
        else {
            for (int i = inicio; i < fin && i < resultados.size(); i++) {
                cout << i - inicio + 1 << ". " << resultados[i].titulo << endl;
                cout << "   Año: " << resultados[i].anio << endl;
                cout << "   Genero: " << resultados[i].genero << endl;
                cout << endl;
            }
        }
        cout << "----------------------------------------" << endl;
        cout << "6. Siguiente pagina" << endl;
        cout << "0. Volver" << endl;
        cout << endl;
        int opcion;
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (opcion == 0) {
            return;
        }
        if (opcion >= 1 && opcion <= 5) {
            int posicion = inicio + opcion - 1;
            if (posicion < resultados.size()) {
                mostrarDetalle(resultados[posicion]);
            }
            else {
                cout << endl;
                cout << "No existe esa pelicula." << endl;
                pausar();
            }
        }
        else if (opcion == 6) {
            if (fin < resultados.size()) {
                pagina++;
            }
            else {
                cout << endl;
                cout << "No hay mas resultados." << endl;
                pausar();
            }
        }
        else {
            cout << endl;
            cout << "Opcion no valida." << endl;
            pausar();
        }
    }
}
void Interfaz::mostrarDetalle(const Pelicula& pelicula) {
    limpiarPantalla();
    cout << "========================================" << endl;
    cout << "        INFORMACION DE PELICULA" << endl;
    cout << "========================================" << endl;
    cout << endl;
    cout << "Titulo: " << pelicula.titulo << endl;
    cout << "Año: " << pelicula.anio << endl;
    cout << "Origen: " << pelicula.origen << endl;
    cout << "Director: " << pelicula.director << endl;
    cout << "Genero: " << pelicula.genero << endl;
    cout << "Cast: " << pelicula.cast << endl;
    cout << endl;
    cout << "Sinopsis:" << endl;
    cout << pelicula.sinopsis << endl;
    cout << endl;
    cout << "----------------------------------------" << endl;
    cout << "1. Like" << endl;
    cout << "2. Ver mas tarde" << endl;
    cout << "3. Volver" << endl;
    cout << endl;
    int opcion;
    cout << "Seleccione una opcion: ";
    cin >> opcion;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    switch (opcion) {
        case 1:
            cout << endl;
            cout << "Pelicula agregada a Likes." << endl;
            pausar();
            break;
        case 2:
            cout << endl;
            cout << "Pelicula agregada a Ver mas tarde." << endl;
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
void Interfaz::mostrarVerMasTarde() {
    limpiarPantalla();
    cout << "========================================" << endl;
    cout << "            VER MAS TARDE" << endl;
    cout << "========================================" << endl;
    cout << endl;
    /*
        Aqui Persona 5 debe entregar los IDs
        guardados en ver_mas_tarde.txt.
        Ejemplo:
        vector<int> ids = cargarVerMasTarde();
        Luego buscamos las peliculas correspondientes.
    */
    cout << "Aqui apareceran las peliculas guardadas." << endl;
    pausar();
}
void Interfaz::limpiarPantalla() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
void Interfaz::pausar() {
    cout << endl;
    cout << "Presione ENTER para continuar...";
    cin.get();
}