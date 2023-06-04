from conan import ConanFile


class RaumAkustik(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("blaze/3.8")
        self.requires("catch2/3.0.1")
        self.requires("fftw/3.3.10")
        self.requires("mp-units/0.8.0@mpusz/testing")

    def config_options(self):
        self.options["fftw"].precision = "single"
        self.options["fftw"].simd = "avx2"
        if self.settings.os == "Macos" and self.settings.arch == "armv8":
            self.options["fftw"].simd = False
