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
