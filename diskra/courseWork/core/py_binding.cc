// core/py_binding.cc
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "FiniteRingRules.h"
#include "SmallRingArithmetic.h"
#include "BigRingArithmetic.h"
#include "RingNumber.h"

namespace py = pybind11;

PYBIND11_MODULE(finite_ring_module, m) {
     m.doc() = "Finite Ring Arithmetic Module - Small and Big arithmetic";
    
     // * --- FiniteRingRules * ---
     py::class_<FiniteRingRules>(m, "FiniteRingRules")
          .def(py::init<const std::string&, const std::string&>(),
               py::arg("config_file"), py::arg("variant_name"))
          .def("getSize", &FiniteRingRules::getSize)
          .def("getZeroElement", &FiniteRingRules::getZeroElement)
          .def("getOneElement", &FiniteRingRules::getOneElement)
          .def("getCharValue", &FiniteRingRules::getCharValue,
               py::arg("c"))
          .def("getValueChar", &FiniteRingRules::getValueChar,
               py::arg("value"))
          .def("isValidChar", &FiniteRingRules::isValidChar,
               py::arg("c"))
          .def("printRules", &FiniteRingRules::printRules);
    
     // * --- SmallRingArithmetic * ---
     py::class_<SmallRingArithmetic>(m, "SmallRingArithmetic")
          .def(py::init<const FiniteRingRules&>(),
               py::arg("rules"))
          .def("add", &SmallRingArithmetic::add,
                py::arg("a"), py::arg("b"))
          .def("subtract", &SmallRingArithmetic::subtract,
                py::arg("a"), py::arg("b"))
          .def("multiply", &SmallRingArithmetic::multiply,
             py::arg("a"), py::arg("b"))
          .def("divide", &SmallRingArithmetic::divide,
                py::arg("a"), py::arg("b"))
          .def("findMultiplicativeInverse", &SmallRingArithmetic::findMultiplicativeInverse,
               py::arg("element"))
          .def("getRules", &SmallRingArithmetic::getRules,
               py::return_value_policy::reference_internal);

     // * --- BigRingArithmetic * ---
     py::class_<BigRingArithmetic>(m, "BigRingArithmetic")
          .def(py::init<const FiniteRingRules&, const SmallRingArithmetic&>(),
                py::arg("rules"), py::arg("small"))
          .def("add", &BigRingArithmetic::add,
                py::arg("a"), py::arg("b"))
          .def("subtract", &BigRingArithmetic::subtract,
                py::arg("a"), py::arg("b"))
          .def("multiply", &BigRingArithmetic::multiply,
               py::arg("a"), py::arg("b"))
          .def("divide", &BigRingArithmetic::divide,
               py::arg("a"), py::arg("b"));
    
     // * --- RingNumber * ---
     py::class_<RingNumber>(m, "RingNumber")
          .def(py::init<const FiniteRingRules&>(),
               py::arg("rules"))
          .def(py::init<const FiniteRingRules&, const std::string&>(),
               py::arg("rules"), py::arg("value"))
          .def(py::init<const FiniteRingRules&, const std::vector<char>&>(),
               py::arg("rules"), py::arg("value"))
          .def("length", &RingNumber::length)
          .def("negate", &RingNumber::negate)
          .def("getDigit", &RingNumber::getDigit,
               py::arg("index"))
          .def("toString", &RingNumber::toString)
          .def("normalize", &RingNumber::normalize)
          .def("isZero", &RingNumber::isZero)
          .def("isValid", &RingNumber::isValid)
          .def("__eq__", &RingNumber::operator==)
          .def("__ne__", &RingNumber::operator!=)
          .def("__str__", &RingNumber::toString)
          .def("__repr__", [](const RingNumber& n) {
            return "RingNumber('" + n.toString() + "')";
        });
}