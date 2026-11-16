import os
import tkinter as tk
from tkinter import ttk, messagebox
import motor_peliculas


class AplicacionGUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Sistema de Búsqueda y Recomendación de Películas")
        self.geometry("820x580")

        # Ruta al CSV en la raiz del proyecto
        ruta_csv = os.path.join(
            os.path.dirname(os.path.abspath(__file__)), "..", "wiki_movie_plots_deduped.csv"
        )
        self.controlador = motor_peliculas.ControladorApp(ruta_csv, 4)

        if not self.controlador.inicializar():
            messagebox.showerror("Error", f"No se pudo cargar el CSV en:\n{ruta_csv}")
            self.destroy()
            return

        # Estado de paginacion bidireccional (de 5 en 5)
        self.resultados_actuales = []  # Lista de tuplas: (id, titulo, anho, detalle_extra)
        self.pagina_actual = 0
        self.TAMANO_PAGINA = 5

        self._crear_componentes()
        self._actualizar_estadisticas()

        # Cargar recomendaciones iniciales al abrir (igual que mostrarInicio en consola)
        self.mostrar_recomendaciones(silencioso=True)

    def _crear_componentes(self):
        # --- 1. Barra Superior: Busqueda ---
        frame_busqueda = tk.Frame(self)
        frame_busqueda.pack(fill=tk.X, padx=10, pady=(10, 5))

        tk.Label(frame_busqueda, text="Buscar:").pack(side=tk.LEFT)
        self.entry_busqueda = tk.Entry(frame_busqueda)
        self.entry_busqueda.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)
        self.entry_busqueda.bind("<Return>", lambda event: self.ejecutar_busqueda())

        self.combo_tipo = ttk.Combobox(
            frame_busqueda,
            values=["Texto", "Director", "Cast", "Genero"],
            state="readonly",
            width=10,
        )
        self.combo_tipo.current(0)
        self.combo_tipo.pack(side=tk.LEFT, padx=5)

        btn_buscar = tk.Button(frame_busqueda, text="Buscar", command=self.ejecutar_busqueda)
        btn_buscar.pack(side=tk.LEFT)

        # --- 2. Barra de Estado y Estadisticas (Observer) ---
        frame_estado = tk.Frame(self)
        frame_estado.pack(fill=tk.X, padx=10, pady=2)

        self.lbl_vista_actual = tk.Label(
            frame_estado, text="Vista: Inicio", font=("Arial", 9, "bold"), fg="#333333"
        )
        self.lbl_vista_actual.pack(side=tk.LEFT)

        self.lbl_estadisticas = tk.Label(
            frame_estado, text="", font=("Arial", 9), fg="#1a5fb4"
        )
        self.lbl_estadisticas.pack(side=tk.RIGHT)

        # --- 3. Tabla Principal (Treeview) ---
        self.tree = ttk.Treeview(
            self, columns=("ID", "Título", "Año", "Info / Score"), show="headings"
        )
        self.tree.heading("ID", text="ID")
        self.tree.heading("Título", text="Título")
        self.tree.heading("Año", text="Año")
        self.tree.heading("Info / Score", text="Info / Score")

        self.tree.column("ID", width=60, anchor=tk.CENTER)
        self.tree.column("Título", width=360)
        self.tree.column("Año", width=70, anchor=tk.CENTER)
        self.tree.column("Info / Score", width=180)

        self.tree.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
        self.tree.bind("<Double-1>", self.abrir_modal_detalle)

        # --- 4. Barra Inferior: Acciones y Paginacion ---
        frame_inf = tk.Frame(self)
        frame_inf.pack(fill=tk.X, padx=10, pady=(5, 10))

        btn_rec = tk.Button(
            frame_inf, text="Recomendaciones", command=self.mostrar_recomendaciones
        )
        btn_rec.pack(side=tk.LEFT, padx=(0, 5))

        btn_vmt = tk.Button(
            frame_inf, text="Ver más tarde", command=self.mostrar_ver_mas_tarde
        )
        btn_vmt.pack(side=tk.LEFT)

        # Controles de paginacion a la derecha
        self.btn_siguiente = tk.Button(
            frame_inf, text="Siguiente >", command=self.pagina_siguiente, state=tk.DISABLED
        )
        self.btn_siguiente.pack(side=tk.RIGHT)

        self.lbl_paginacion = tk.Label(frame_inf, text="Página 0 de 0")
        self.lbl_paginacion.pack(side=tk.RIGHT, padx=10)

        self.btn_anterior = tk.Button(
            frame_inf, text="< Anterior", command=self.pagina_anterior, state=tk.DISABLED
        )
        self.btn_anterior.pack(side=tk.RIGHT)

    def _actualizar_estadisticas(self):
        total_cat = self.controlador.totalPeliculas()
        likes_sesion = self.controlador.totalLikesRegistrados()
        gen_fav = self.controlador.generoFavoritoDelUsuario()
        texto_fav = gen_fav.capitalize() if gen_fav else "N/A"
        self.lbl_estadisticas.config(
            text=f"Catálogo: {total_cat} | Likes (sesión): {likes_sesion} | Género favorito: {texto_fav}"
        )

    def _cargar_lista_en_tabla(self, items, titulo_vista):
        self.resultados_actuales = items
        self.pagina_actual = 0
        self.lbl_vista_actual.config(text=titulo_vista)
        self._renderizar_pagina()

    def _renderizar_pagina(self):
        for item in self.tree.get_children():
            self.tree.delete(item)

        total = len(self.resultados_actuales)
        total_paginas = (total + self.TAMANO_PAGINA - 1) // self.TAMANO_PAGINA if total > 0 else 0

        inicio = self.pagina_actual * self.TAMANO_PAGINA
        fin = min(inicio + self.TAMANO_PAGINA, total)

        for i in range(inicio, fin):
            self.tree.insert("", tk.END, values=self.resultados_actuales[i])

        pag_mostrada = self.pagina_actual + 1 if total > 0 else 0
        self.lbl_paginacion.config(
            text=f"Página {pag_mostrada} de {total_paginas} ({total} resultados)"
        )

        self.btn_anterior.config(state=tk.NORMAL if self.pagina_actual > 0 else tk.DISABLED)
        self.btn_siguiente.config(state=tk.NORMAL if fin < total else tk.DISABLED)

    def pagina_siguiente(self):
        if (self.pagina_actual + 1) * self.TAMANO_PAGINA < len(self.resultados_actuales):
            self.pagina_actual += 1
            self._renderizar_pagina()

    def pagina_anterior(self):
        if self.pagina_actual > 0:
            self.pagina_actual -= 1
            self._renderizar_pagina()

    def ejecutar_busqueda(self):
        query = self.entry_busqueda.get().strip()
        if not query:
            messagebox.showinfo("Búsqueda", "Ingrese un término para buscar.")
            return

        tipo = self.combo_tipo.get()
        if tipo == "Texto":
            resultados = self.controlador.buscarTexto(query)
        elif tipo == "Director":
            resultados = self.controlador.buscarPorTag(motor_peliculas.TipoTag.DIRECTOR, query)
        elif tipo == "Cast":
            resultados = self.controlador.buscarPorTag(motor_peliculas.TipoTag.CAST, query)
        else:
            resultados = self.controlador.buscarPorTag(motor_peliculas.TipoTag.GENERO, query)

        items = []
        for res in resultados:
            peli = self.controlador.obtenerPeliculaPorId(res.id)
            if peli:
                items.append((peli.id, peli.titulo, peli.anho, f"Score: {round(res.score, 2)}"))

        self._cargar_lista_en_tabla(items, f"Búsqueda ({tipo}): '{query}'")

    def mostrar_recomendaciones(self, silencioso=False):
        recomendadas = self.controlador.obtenerRecomendaciones()
        if not recomendadas and not silencioso:
            messagebox.showinfo(
                "Recomendaciones",
                "Dale 'Like' a algunas películas para recibir recomendaciones.",
            )

        items = [
            (p.id, p.titulo, p.anho, f"Recomendada ({p.genero})")
            for p in recomendadas
        ]
        self._cargar_lista_en_tabla(items, "Vista: Recomendadas para ti")

    def mostrar_ver_mas_tarde(self):
        lista_vmt = self.controlador.obtenerVerMasTarde()
        if not lista_vmt:
            messagebox.showinfo(
                "Ver más tarde", "No tienes películas guardadas en esta lista."
            )

        items = [
            (p.id, p.titulo, p.anho, f"Dir: {p.director}")
            for p in lista_vmt
        ]
        self._cargar_lista_en_tabla(items, "Vista: Ver más tarde")

    def abrir_modal_detalle(self, event):
        item_seleccionado = self.tree.selection()
        if not item_seleccionado:
            return

        valores = self.tree.item(item_seleccionado[0], "values")
        id_peli = int(valores[0])
        peli = self.controlador.obtenerPeliculaPorId(id_peli)
        if not peli:
            return

        ventana_modal = tk.Toplevel(self)
        ventana_modal.title(f"Detalle: {peli.titulo}")
        ventana_modal.geometry("540x440")
        ventana_modal.transient(self)
        ventana_modal.grab_set()

        frame_info = tk.Frame(ventana_modal)
        frame_info.pack(fill=tk.X, padx=15, pady=10)

        tk.Label(
            frame_info,
            text=f"{peli.titulo} ({peli.anho})",
            font=("Arial", 13, "bold"),
            wraplength=500,
            justify=tk.LEFT,
        ).pack(anchor=tk.W, pady=(0, 6))

        metadatos = (
            f"Origen:   {peli.origen}\n"
            f"Director: {peli.director}\n"
            f"Género:   {peli.genero}\n"
            f"Cast:     {peli.cast}"
        )
        tk.Label(
            frame_info, text=metadatos, justify=tk.LEFT, wraplength=500, font=("Arial", 9)
        ).pack(anchor=tk.W)

        tk.Label(
            ventana_modal, text="Sinopsis:", font=("Arial", 10, "bold")
        ).pack(anchor=tk.W, padx=15, pady=(5, 0))

        texto_sinopsis = tk.Text(ventana_modal, wrap=tk.WORD, height=10, font=("Arial", 9))
        texto_sinopsis.insert(tk.END, peli.sinopsis)
        texto_sinopsis.config(state=tk.DISABLED)
        texto_sinopsis.pack(fill=tk.BOTH, expand=True, padx=15, pady=5)

        frame_botones = tk.Frame(ventana_modal)
        frame_botones.pack(pady=10)

        def accion_like():
            es_nuevo = self.controlador.darLike(peli.id)
            self._actualizar_estadisticas()
            if es_nuevo:
                messagebox.showinfo("Like", "¡Película agregada a tus Likes!", parent=ventana_modal)
            else:
                messagebox.showinfo("Like", "Ya le habías dado Like a esta película.", parent=ventana_modal)

        def accion_ver_mas_tarde():
            es_nuevo = self.controlador.agregarVerMasTarde(peli.id)
            if es_nuevo:
                messagebox.showinfo(
                    "Ver más tarde", "¡Película agregada a 'Ver más tarde'!", parent=ventana_modal
                )
            else:
                messagebox.showinfo(
                    "Ver más tarde", "Esta película ya estaba en tu lista.", parent=ventana_modal
                )

        tk.Button(frame_botones, text="Dar Like", width=15, command=accion_like).pack(
            side=tk.LEFT, padx=8
        )
        tk.Button(
            frame_botones, text="Ver más tarde", width=15, command=accion_ver_mas_tarde
        ).pack(side=tk.LEFT, padx=8)
        tk.Button(
            frame_botones, text="Cerrar", width=12, command=ventana_modal.destroy
        ).pack(side=tk.LEFT, padx=8)