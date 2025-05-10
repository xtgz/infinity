#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <vector>
#include "magicXorLauncher.cpp"

namespace py = pybind11;

PYBIND11_MODULE(magicXorMiner, m) {
    m.doc() = "Python binding for the profanity2 magicxor mode";
    
    m.def("runMagicXor", &runMagicXor,
          py::arg("strPublicKey"),
          py::arg("strMagicXorDifficulty"),
          py::arg("mineContract") = false,
          py::arg("worksizeLocal") = 64,
          py::arg("worksizeMax") = 0,
          py::arg("inverseSize") = 255,
          py::arg("inverseMultiple") = 1024,
          py::arg("bNoCache") = false,
          py::arg("vDeviceSkipIndex") = std::vector<size_t>(),
          py::arg("verboseStdOut") = false,
          "Executes the custom magicxor kernel and returns the magicxor result string");
}