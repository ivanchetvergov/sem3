#include <pybind11/pybind11.h>
#include "FiniteRing.h"

namespace py = pybind11;

PYBIND11_MODULE(finite_field_module, m) {
    py::class_<FiniteRingRules>(m, "FiniteRingRules")
        .def(py::init<const std::string&, const std::string&>())
        .def("getZeroElement", &FiniteRingRules::getZeroElement)
        .def("getOneElement", &FiniteRingRules::getOneElement)
        .def("getCharValue", &FiniteRingRules::getCharValue)
        .def("getValueChar", &FiniteRingRules::getValueChar)
        .def("getSize", &FiniteRingRules::getSize);

    py::class_<FiniteRing>(m, "FiniteRing")
        .def(py::init<const FiniteRingRules&>())
        .def("add", &FiniteRing::add)
        .def("subtract", &FiniteRing::subtract)
        .def("multiply", &FiniteRing::multiply)
        .def("divide", &FiniteRing::divide)
        .def("getRules", &FiniteRing::getRules, py::return_value_policy::reference_internal);
}