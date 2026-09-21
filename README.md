# Programación III: Proyecto Final (2026-2)

## Integrantes 
* Adrian Gabriel Rojas Tejada
* Cristobal Javier Aranda Gallardo
* Walter Sebastián Aquino Pachas
* Evans Valentino Solis Mendoza

## Construcción del Suffix Trie (Inserción)

Se diseñó e implementó un **Suffix Trie** optimizado en memoria para habilitar la búsqueda por sub-palabras de forma eficiente, tokenizando el contenido y utilizando `std::string_view` para evitar la copia innecesaria de cadenas durante la indexación.

## Diagrama ASCII del Suffix Trie

Ejemplo visual de la estructura insertando los tokens "bar" y "arco" (asumiendo un largo mínimo de sufijo = 3 para simplificar).
*Sufijos insertados para "bar": "bar".*
*Sufijos insertados para "arco": "arco", "rco".*

```text
               (raiz)
              /   |   \
            (b)  (a)  (r)
            /     |     \
          (a)    (r)    (c)
          /       |       \
        (r)*     (c)      (o)* [id, frec]
                  |
                 (o)* [id, frec]

* = esFinDePalabra (El nodo almacena un mapa con los IDs de las películas y su frecuencia)
```

### Pseudocódigo de Inserción
```text
Estructura NodoTrie:
    hijos: Mapa (carácter -> NodoTrie*)
    esFinDePalabra: Booleano
    frecuencias: Mapa (idPelicula -> entero)

Clase SuffixTrie:
    Raiz: NodoTrie
    LargoMinimo: Entero = 2

    Función construirIndice(peliculas):
        Para cada peli en peliculas:
            Para cada token en peli.tokens_txt:
                insertarPalabra(token, peli.id)

    Función insertarPalabra(palabra, idPelicula):
        N = longitud(palabra)
        Si N == 0: Retornar
        
        insertarSufijoDesde(raiz, palabra, 0, idPelicula)
        
        Para i desde 1 hasta N - 1:
            Si (N - i) >= LargoMinimo:
                insertarSufijoDesde(raiz, palabra, i, idPelicula)

    Función insertarSufijoDesde(nodoActual, palabra, indiceInicio, idPelicula):
        Para i desde indiceInicio hasta longitud(palabra) - 1:
            c = palabra[i]
            Si nodoActual.hijos[c] no existe:
                nodoActual.hijos[c] = Nuevo NodoTrie()
            nodoActual = nodoActual.hijos[c]
        
        nodoActual.esFinDePalabra = Verdadero
        nodoActual.frecuencias[idPelicula] += 1
```

### Pseudocódigo de Busqueda
Para asegurar que los resultados más relevantes aparezcan primero 
se les da un puntaje las coincidencias, lo que le da peso a la frecuencia de aparición del término, 
sobre todo si el término está en el título, y tambien se considera 
cuántos términos diferentes de la consulta están presentes en la película.
```text
Función buscar(trie, peliculas, consulta):
    terminos = dividir_consulta(consulta)
    
    Para cada termino en terminos:
        nodo = trie.navegarPrefijo(termino)
        coincidencias = recolectarCoincidencias(nodo)
        
        Para cada (id, frecuencia) en coincidencias:
            frecuencia_total[id] += frecuencia
            cobertura[id] += 1
            
            Si normalizar(titulo_pelicula[id]) contiene termino:
                en_titulo[id] += 1

    Para cada id con coincidencias:
        score = (cobertura * 50.0) + frecuencia_total + (en_titulo * 30.0)
        resultados.push_back({id, score})

    Ordenar resultados por score descendente
    Retornar resultados
```

### Instrucciones de Compilación y Ejecución
1. Asegúrate de tener un compilador de C++ (que soporte C++17) y CMake instalados en tu sistema.
2. Clona este repositorio.
3. Asegúrate de que el archivo base de datos wiki_movie_plots_deduped.csv se encuentre en la misma 
carpeta donde se generará el ejecutable.
5. Si usas consola, ejecuta los siguientes comandos en la raíz del proyecto:
```text
Bash
mkdir build
cd build
cmake ..
make
./PlataformaStreaming
```
Nota para usuarios de CLion (o IDEs similares): El programa utiliza comandos del sistema para 
limpiar la pantalla interactiva (system("cls") / system("clear")). 
Para una correcta visualización en CLion, debes ir a Run -> Edit Configurations... y 
marcar la casilla "Emulate terminal in output console".
