# Programación III: Proyecto Final (2026-2)

## Integrantes 
* Adrian Rojas Tejada
* Cristobal Javier Aranda Gallardo
* Walter Sebastián Aquino Pachas
* Adrian Gabriel Rojas Tejada
* Evans Valentino Solis Mendoza



## Construcción del Suffix Trie (Inserción)

Se diseñó e implementó un **Suffix Trie** para habilitar la búsqueda por sub-palabras de forma eficiente, limitando la inserción por tokens para optimizar el consumo de memoria del dataset.

### Pseudocódigo de Inserción
```text
Estructura NodoTrie:
    hijos: Arreglo de 36 punteros
    esFinDePalabra: Booleano
    frecuencias: Mapa (idPelicula -> entero)

Clase SuffixTrie:
    Raiz: NodoTrie
    LargoMinimo: Entero = 2

    Función construirIndice(peliculas):
        Para cada peli en peliculas:
            Para cada token en peli.tokens_combinados:
                insertarPalabra(token, peli.id)

    Función insertarPalabra(palabra, idPelicula):
        N = longitud(palabra)
        Para i desde 0 hasta N - 1:
            Si (N - i) >= LargoMinimo:
                sufijo = subcadena(palabra, desde i hasta el final)
                insertarSufijo(raiz, sufijo, idPelicula)

    Función insertarSufijo(nodo, sufijo, idPelicula):
        nodoActual = nodo
        Para cada caracter c en sufijo:
            indice = mapearIndice(c)
            Si nodoActual.hijos[indice] es NULL:
                nodoActual.hijos[indice] = Nuevo NodoTrie()
            nodoActual = nodoActual.hijos[indice]
        
        nodoActual.esFinDePalabra = Verdadero
        nodoActual.frecuencias[idPelicula] += 1