#include <pybind11/pybind11.h>
#include "FiniteField.h"

namespace py = pybind11;

PYBIND11_MODULE(finite_field_module, m) {
    py::class_<FiniteFieldRules>(m, "FiniteFieldRules")
        .def(py::init<const std::string&, const std::string&>());

    py::class_<FiniteField>(m, "FiniteField")
        .def(py::init<const FiniteFieldRules&>())
        .def("add", [](const FiniteField &ff, const std::string &a, const std::string &b) {
            if (a.size() != 1 || b.size() != 1)
                throw std::runtime_error("add: arguments must be single-character strings");
            return std::string(1, ff.add(a[0], b[0]));
        })
        .def("subtract", [](const FiniteField &ff, const std::string &a, const std::string &b) {
            if (a.size() != 1 || b.size() != 1)
                throw std::runtime_error("subtract: arguments must be single-character strings");
            return std::string(1, ff.subtract(a[0], b[0]));
        })
        .def("multiply", [](const FiniteField &ff, const std::string &a, const std::string &b) {
            if (a.size() != 1 || b.size() != 1)
                throw std::runtime_error("multiply: arguments must be single-character strings");
            return std::string(1, ff.multiply(a[0], b[0]));
        })
        .def("divide", [](const FiniteField &ff, const std::string &a, const std::string &b) {
            if (a.size() != 1 || b.size() != 1)
                throw std::runtime_error("divide: arguments must be single-character strings");
            return std::string(1, ff.divide(a[0], b[0]));
        });
}
