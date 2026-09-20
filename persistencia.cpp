
#include "persistencia.h"
#include <fstream>
#include <iostream>
#include <algorithm>

using namespace std;

//devuelve lo guardado
vector<int> cargarIdsDeArchivo(const string& nombreArchivo) {
    vector<int> ids;
    ifstream archivo(nombreArchivo);
    int id;

    // Leer cada ID línea por línea hasta el final
    while (archivo >> id) {
        ids.push_back(id);
    }
    archivo.close();
    return ids;
}


bool estaEnLista(const string& nombreArchivo, int id) {
    vector<int> ids_guardados = cargarIdsDeArchivo(nombreArchivo);

    return find(ids_guardados.begin(), ids_guardados.end(), id) != ids_guardados.end();
}

//crea y/o guarda el id
void guardarIdEnArchivo(const string& nombreArchivo, int id) {
    if (estaEnLista(nombreArchivo, id)) {
        cerr << "Esta pelicula ya esta guardado en tu lista de "
        << (nombreArchivo == "ver_mas_tarde.txt" ? "Ver mas tarde" : "Likes") << endl;
        return;
    }
    ofstream archivo(nombreArchivo, ios::app);
    if (archivo.is_open()) {
        archivo << id << "\n";
        cout << "La pelicula fue agregada a "
        << (nombreArchivo == "ver_mas_tarde.txt" ? "Ver mas tarde" : "Likes") << endl;
        archivo.close();
    } else {
        cerr << "Error al abrir/crear el archivo: " << nombreArchivo << endl;
    }

}



void guardarLike(int id) {
    guardarIdEnArchivo("likes.txt", id);
}

void guardarVerMasTarde(int id) {
    guardarIdEnArchivo("ver_mas_tarde.txt", id);
}

vector<int> cargarLikes() {
    return cargarIdsDeArchivo("likes.txt");
}

vector<int> cargarVerMasTarde() {
    return cargarIdsDeArchivo("ver_mas_tarde.txt");
}