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
    .def("add", [](const FiniteField &f, const std::string &a, const std::string &b) {
        if (a.size() != 1 || b.size() != 1) throw std::runtime_error("expected single-character string");
        return f.add(a[0], b[0]);
    })
    .def("subtract", [](const FiniteField &f, const std::string &a, const std::string &b) {
        if (a.size() != 1 || b.size() != 1) throw std::runtime_error("expected single-character string");
        return f.subtract(a[0], b[0]);
    })
    .def("multiply", [](const FiniteField &f, const std::string &a, const std::string &b) {
        if (a.size() != 1 || b.size() != 1) throw std::runtime_error("expected single-character string");
        return f.multiply(a[0], b[0]);
    })
    .def("divide", [](const FiniteField &f, const std::string &a, const std::string &b) {
        if (a.size() != 1 || b.size() != 1) throw std::runtime_error("expected single-character string");
        return f.divide(a[0], b[0]);
    })
    .def("getRules", &FiniteField::getRules, py::return_value_policy::reference_internal);

}