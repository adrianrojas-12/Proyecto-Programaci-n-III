# Programación III: Proyecto Final (2026-2)

## Integrantes 
* Adrian Gabriel Rojas Tejada
* Cristobal Javier Aranda Gallardo
* Walter Sebastián Aquino Pachas
* Evans Valentino Solis Mendoza
* Adrian Omar Llanos Cespedes

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

## Patrones de Diseño (patrones.h / patrones.cpp)

Se implementaron 4 patrones GoF más una fachada que conecta todo el backend con la interfaz, encapsulados en un solo módulo (`patrones.h`/`patrones.cpp`).

### Singleton — `GestorDatos`
Garantiza una única instancia de las películas cargadas y del Suffix Trie construido durante toda la ejecución, evitando duplicar el costo de carga (~15s, ~1.8GB de RAM) y posibles desincronizaciones entre copias.

```text
Clase GestorDatos:
    cargado: Booleano = Falso
    peliculas: Lista<Pelicula>
    trie: SuffixTrie

    Función instancia():
        // Variable estatica local: se crea una sola vez,
        // la primera vez que se llama a instancia()
        Si no existe unica_instancia:
            unica_instancia = Nuevo GestorDatos()
        Retornar unica_instancia

    Función cargar(rutaCSV, hilos):
        Si cargado:
            Retornar Verdadero  // ya cargado, no repetir trabajo

        peliculas = cargar_peliculas(rutaCSV)
        Si peliculas esta vacio:
            Retornar Falso

        trie = Nuevo SuffixTrie()
        trie.construirIndiceParalelo(peliculas, hilos)
        cargado = Verdadero
        Retornar Verdadero
```

### Strategy — `IEstrategiaBusqueda`
Define una interfaz común para las distintas formas de buscar: `EstrategiaBusquedaTexto` (texto libre vía el Trie en paralelo) y `EstrategiaBusquedaTag` (por Director, Cast o Género vía comparación de substring). Reemplaza el `if/else` manual que existía en `Interfaz::buscarPorTag`.

```text
Interfaz IEstrategiaBusqueda:
    Función buscar(consulta): Lista<ResultadoBusqueda>

Clase EstrategiaBusquedaTexto implementa IEstrategiaBusqueda:
    trie, peliculas, hilos

    Función buscar(consulta):
        Retornar buscarParalelo(trie, peliculas, consulta, hilos)

Clase EstrategiaBusquedaTag implementa IEstrategiaBusqueda:
    peliculas, tipoTag  // DIRECTOR, CAST o GENERO

    Función buscar(consulta):
        resultados = []
        objetivo = normalizar(consulta)
        Para cada pelicula en peliculas:
            campo = normalizar(campoSegunTipo(pelicula, tipoTag))
            Si campo contiene objetivo:
                resultados.push_back({pelicula.id, 1.0})
        Retornar resultados
```

### Factory — `FabricaEstrategiaBusqueda`
Construye la estrategia concreta adecuada según el `TipoBusqueda` elegido por el usuario, sin que el resto del programa conozca los detalles de construcción de cada una.

```text
Clase FabricaEstrategiaBusqueda:
    Función crear(tipo, trie, peliculas, hilos): IEstrategiaBusqueda
        Segun tipo:
            Caso TEXTO:
                Retornar Nueva EstrategiaBusquedaTexto(trie, peliculas, hilos)
            Caso TAG_DIRECTOR:
                Retornar Nueva EstrategiaBusquedaTag(peliculas, DIRECTOR)
            Caso TAG_CAST:
                Retornar Nueva EstrategiaBusquedaTag(peliculas, CAST)
            Caso TAG_GENERO:
                Retornar Nueva EstrategiaBusquedaTag(peliculas, GENERO)
```

### Observer — `SujetoLikes`
Cuando el usuario da "Like" a una película, notifica a dos observadores independientes: `ObservadorRecomendaciones` (invalida y recalcula el cache de recomendaciones) y `ObservadorEstadisticas` (cuenta likes totales y género favorito).

```text
Interfaz IObservadorLike:
    Función alRecibirLike(pelicula)

Clase SujetoLikes:
    observadores: Lista<IObservadorLike>

    Función suscribir(observador):
        observadores.push_back(observador)

    Función notificar(pelicula):
        Para cada observador en observadores:
            observador.alRecibirLike(pelicula)

Clase ObservadorRecomendaciones implementa IObservadorLike:
    cache: Lista<Pelicula>
    necesitaRecalcular: Booleano = Verdadero

    Función alRecibirLike(pelicula):
        necesitaRecalcular = Verdadero  // solo marca, no recalcula aqui

Clase ObservadorEstadisticas implementa IObservadorLike:
    totalLikes: Entero = 0
    conteoPorGenero: Mapa (genero -> entero)

    Función alRecibirLike(pelicula):
        totalLikes += 1
        Para cada genero en separarGeneros(pelicula.genero):
            conteoPorGenero[genero] += 1
```

### Facade — `ControladorApp`
Punto único de entrada para la interfaz (consola o GUI): expone `buscarTexto`, `buscarPorTag`, `darLike`, `agregarVerMasTarde`, `obtenerRecomendaciones`, etc., ocultando el Trie, la persistencia y el ranking detrás de métodos simples. Internamente coordina a los 4 patrones anteriores con `persistencia.cpp` y `recomendador.cpp`.

```text
Clase ControladorApp:
    sujetoLikes, obsRecomendaciones, obsEstadisticas

    Constructor(rutaCSV, hilos):
        sujetoLikes.suscribir(obsRecomendaciones)
        sujetoLikes.suscribir(obsEstadisticas)

    Función inicializar():
        Retornar GestorDatos.instancia().cargar(rutaCSV, hilos)

    Función buscarTexto(consulta):
        estrategia = Fabrica.crear(TEXTO, trie, peliculas, hilos)
        Retornar estrategia.buscar(consulta)

    Función buscarPorTag(tipo, consulta):
        estrategia = Fabrica.crear(tipoBusquedaSegun(tipo), trie, peliculas, hilos)
        Retornar estrategia.buscar(consulta)

    Función darLike(id):
        pelicula = obtenerPeliculaPorId(id)
        Si pelicula no existe: Retornar Falso

        esNuevo = guardarLike(id)
        Si esNuevo:
            sujetoLikes.notificar(pelicula)  // dispara Observer
        Retornar esNuevo

    Función obtenerRecomendaciones():
        Si obsRecomendaciones.necesitaRecalcular:
            likes = cargarLikes()
            obsRecomendaciones.actualizarCache(generarRecomendaciones(peliculas, likes))
        Retornar obsRecomendaciones.obtenerCache()
```
marcar la casilla "Emulate terminal in output console".

## Interfaz Gráfica de Usuario — GUI (`gui_python/`)

La capa de presentación está aislada dentro del directorio `gui_python/` y desarrollada en **Python 3** utilizando la librería estándar **Tkinter** (`ttk`), manteniendo una arquitectura desacoplada del núcleo en C++ mediante un contrato de datos (*Mock Data* en fase inicial) listo para consumir la fachada `ControladorApp`.

### Estructura del Módulo
* **`main.py`:** Punto de entrada de la aplicación visual; instancia la ventana principal y ejecuta el bucle de eventos (`mainloop()`).
* **`interfaz_gui.py`:** Contiene la clase encapsulada `AplicacionGUI` (heredando de `tk.Tk`), gestionando los widgets, eventos mediante *callbacks* y la paginación de resultados.
* **`mock_data.py`:** Define la estructura contractual de datos simulados (`PELICULAS_DUMMY`) con los atributos `id`, `titulo`, `likes` y `ver_mas_tarde`, permitiendo el desarrollo independiente de la GUI.

### Flujo de Paginación y Estructuras de Datos (POO y Queues)
Para evitar sobrecargar la vista cuando una búsqueda arroja cientos de coincidencias, se implementó una cola FIFO mediante `collections.deque` que despacha los resultados en bloques de **5 en 5**.

```text
Clase AplicacionGUI hereda de tk.Tk:
    todas_las_peliculas: Lista<Diccionario>
    cola_paginacion: Cola (deque)

    Constructor():
        Configurar ventana ("Sistema de Búsqueda y Recomendación de Películas", 600x500)
        Inicializar widgets (Barra de búsqueda, Tabla Treeview, Botón "Siguiente (5)")
        Vincular evento doble clic (<Double-1>) al callback abrir_modal_detalle
        ejecutar_busqueda()  // Carga inicial por defecto

    Función ejecutar_busqueda():
        consulta = obtener_texto(entry_busqueda).minusculas()
        filtrados = filtrar(todas_las_peliculas donde consulta en titulo)
        cola_paginacion = Nueva Cola(filtrados)
        mostrar_siguiente_pagina()

    Función mostrar_siguiente_pagina():
        Limpiar filas actuales de la tabla (Treeview)
        limite = minimo(5, longitud(cola_paginacion))
        Para i desde 1 hasta limite:
            pelicula = cola_paginacion.popleft()  // Extrae el primer elemento (FIFO)
            Insertar fila (pelicula.id, pelicula.titulo, pelicula.likes) en Treeview

    Callback abrir_modal_detalle(evento):
        fila = obtener_seleccion(Treeview)
        Si fila no existe: Retornar

        Crear ventana secundaria modal (tk.Toplevel)
        Mostrar título de la película seleccionada
        Botón "Dar Like" -> ejecuta callback de confirmación/actualización
        
