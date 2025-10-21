#include <pybind11/pybind11.h>
#include "FiniteField.h"

namespace py = pybind11;

PYBIND11_MODULE(finite_field_module, m) {
    py::class_<FiniteFieldRules>(m, "FiniteFieldRules")
        .def(py::init<const std::string&, const std::string&>())
        .def("getZeroElement", &FiniteFieldRules::getZeroElement)
        .def("getOneElement", &FiniteFieldRules::getOneElement)
        .def("getCharValue", &FiniteFieldRules::getCharValue)
        .def("getValueChar", &FiniteFieldRules::getValueChar)
        .def("getSize", &FiniteFieldRules::getSize);

    py::class_<FiniteField>(m, "FiniteField")
        .def(py::init<const FiniteFieldRules&>())
        .def("add", &FiniteField::add)
        .def("subtract", &FiniteField::subtract)
        .def("multiply", &FiniteField::multiply)
        .def("divide", &FiniteField::divide)
        .def("getRules", &FiniteField::getRules, py::return_value_policy::reference_internal);
}