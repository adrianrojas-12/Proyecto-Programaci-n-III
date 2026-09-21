#include "persistencia.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

// Carga los IDs guardados en el archivo de texto
vector<int> cargarIdsDeArchivo(const string& nombreArchivo) {
    vector<int> ids;
    ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        return ids;
    }

    int id;
    while (archivo >> id) {
        ids.push_back(id);
    }
    archivo.close();
    return ids;
}

// Verifica si un ID ya existe en la lista
bool estaEnLista(const string& nombreArchivo, int id) {
    vector<int> ids_guardados = cargarIdsDeArchivo(nombreArchivo);
    return find(ids_guardados.begin(), ids_guardados.end(), id) != ids_guardados.end();
}

// Guarda un ID en el archivo (devuelve true si lo agregó, false si ya existía o falló)
bool guardarIdEnArchivo(const string& nombreArchivo, int id) {
    if (estaEnLista(nombreArchivo, id)) {
        return false;
    }

    ofstream archivo(nombreArchivo, ios::app);
    if (archivo.is_open()) {
        archivo << id << "\n";
        archivo.close();
        return true;
    }

    return false;
}

bool guardarLike(int id) {
    return guardarIdEnArchivo("likes.txt", id);
}

bool guardarVerMasTarde(int id) {
    return guardarIdEnArchivo("ver_mas_tarde.txt", id);
}

vector<int> cargarLikes() {
    return cargarIdsDeArchivo("likes.txt");
}

vector<int> cargarVerMasTarde() {
    return cargarIdsDeArchivo("ver_mas_tarde.txt");
}