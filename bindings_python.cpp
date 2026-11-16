#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "patrones.h"

namespace py = pybind11;

PYBIND11_MODULE(motor_peliculas, m) {
    m.doc() = "Motor de busqueda y recomendacion de peliculas en C++";

    py::enum_<TipoTag>(m, "TipoTag")
        .value("DIRECTOR", TipoTag::DIRECTOR)
        .value("CAST", TipoTag::CAST)
        .value("GENERO", TipoTag::GENERO)
        .export_values();

    py::class_<Pelicula>(m, "Pelicula")
        .def_readonly("id", &Pelicula::id)
        .def_readonly("titulo", &Pelicula::titulo)
        .def_readonly("anho", &Pelicula::anho)
        .def_readonly("origen", &Pelicula::origen)
        .def_readonly("director", &Pelicula::director)
        .def_readonly("genero", &Pelicula::genero)
        .def_readonly("cast", &Pelicula::cast)
        .def_readonly("sinopsis", &Pelicula::sinopsis);

    py::class_<ResultadoBusqueda>(m, "ResultadoBusqueda")
        .def_readonly("id", &ResultadoBusqueda::id)
        .def_readonly("score", &ResultadoBusqueda::score);

    py::class_<ControladorApp>(m, "ControladorApp")
        .def(py::init<std::string, unsigned>(),
             py::arg("rutaCSV"), py::arg("hilosCarga") = 4)
        .def("inicializar", &ControladorApp::inicializar)
        .def("buscarTexto", &ControladorApp::buscarTexto)
        .def("buscarPorTag", &ControladorApp::buscarPorTag)
        .def("obtenerPeliculaPorId", &ControladorApp::obtenerPeliculaPorId,
             py::return_value_policy::reference_internal)
        .def("totalPeliculas", &ControladorApp::totalPeliculas)
        .def("darLike", &ControladorApp::darLike)
        .def("agregarVerMasTarde", &ControladorApp::agregarVerMasTarde)
        .def("obtenerVerMasTarde", &ControladorApp::obtenerVerMasTarde,
             py::return_value_policy::reference_internal)
        .def("obtenerRecomendaciones", &ControladorApp::obtenerRecomendaciones,
             py::return_value_policy::reference_internal)
        .def("totalLikesRegistrados", &ControladorApp::totalLikesRegistrados)
        .def("generoFavoritoDelUsuario", &ControladorApp::generoFavoritoDelUsuario);
}