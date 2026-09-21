#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include <string>
#include <vector>

bool guardarLike(int id);
bool guardarVerMasTarde(int id);
std::vector<int> cargarLikes();
std::vector<int> cargarVerMasTarde();

#endif // PERSISTENCIA_H