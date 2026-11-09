# interfaz_gui
import tkinter as tk
from tkinter import ttk, messagebox
from collections import deque
from mock_data import PELICULAS_DUMMY

class AplicacionGUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Sistema de Búsqueda y Recomendación de Películas")
        self.geometry("600x500")

        self.todas_las_peliculas = PELICULAS_DUMMY
        self.cola_paginacion = deque()

        self._crear_componentes()

        # Cargar los primeros 5 elementos al abrir la ventana
        self.ejecutar_busqueda()

    def _crear_componentes(self):
        # Barra de Búsqueda
        frame_busqueda = tk.Frame(self)
        frame_busqueda.pack(fill=tk.X, padx=10, pady=5)

        tk.Label(frame_busqueda, text="Buscar:").pack(side=tk.LEFT)
        self.entry_busqueda = tk.Entry(frame_busqueda)
        self.entry_busqueda.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)

        btn_buscar = tk.Button(frame_busqueda, text="Buscar", command=self.ejecutar_busqueda)
        btn_buscar.pack(side=tk.LEFT)

        # Tabla de Resultados
        self.tree = ttk.Treeview(self, columns=("ID", "Título", "Likes"), show="headings")
        self.tree.heading("ID", text="ID")
        self.tree.heading("Título", text="Título")
        self.tree.heading("Likes", text="Likes")
        self.tree.pack(fill=tk.BOTH, expand=True, padx=10, pady=5)
        self.tree.bind("<Double-1>", self.abrir_modal_detalle)

        # Botón Paginación (5 en 5)
        frame_paginacion = tk.Frame(self)
        frame_paginacion.pack(fill=tk.X, padx=10, pady=5)

        self.btn_siguiente = tk.Button(frame_paginacion, text="Siguiente (5)", command=self.mostrar_siguiente_pagina)
        self.btn_siguiente.pack(side=tk.RIGHT)

    def ejecutar_busqueda(self):
        query = self.entry_busqueda.get().lower()
        resultados = [p for p in self.todas_las_peliculas if query in p["titulo"].lower()]
        self.cola_paginacion = deque(resultados)
        self.mostrar_siguiente_pagina()

    def mostrar_siguiente_pagina(self):
        for item in self.tree.get_children():
            self.tree.delete(item)

        for _ in range(min(5, len(self.cola_paginacion))):
            peli = self.cola_paginacion.popleft()
            self.tree.insert("", tk.END, values=(peli["id"], peli["titulo"], peli["likes"]))

    def abrir_modal_detalle(self, event):
        item_seleccionado = self.tree.selection()
        if not item_seleccionado:
            return

        valores = self.tree.item(item_seleccionado[0], "values")

        ventana_modal = tk.Toplevel(self)
        ventana_modal.title(f"Detalle: {valores[1]}")
        ventana_modal.geometry("300x150")

        tk.Label(ventana_modal, text=f"Título: {valores[1]}", font=("Arial", 12, "bold")).pack(pady=10)
        btn_like = tk.Button(ventana_modal, text="Dar Like", command=lambda: messagebox.showinfo("Like", "¡Te ha gustado esta película!"))
        btn_like.pack(pady=5)