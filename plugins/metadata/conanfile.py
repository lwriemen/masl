import conan
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.errors import ConanInvalidConfiguration


class ConanFile(conan.ConanFile):
    name = "xtuml_metadata"
    version = "1.0"
    user = "xtuml"

    package_type = "shared-library"

    license = "Apache-2.0"
    url = "https://github.com/xtuml/masl"
    description = "xtUML C++ Software Architecture Meta Data"
    topics = ("xtuml", "masl", "metadata")

    settings = "os", "compiler", "build_type", "arch"

    exports_sources = "CMakeLists.txt", "src/*", "include/*"

    def requirements(self):
        self.requires(
            "xtuml_swa/[>=1.0 <2]@xtuml",
            transitive_headers=True,
            transitive_libs=True,
        )

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["MetaData"]
        self.cpp_info.requires.append("xtuml_swa::xtuml_swa")
