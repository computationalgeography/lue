"""
Conan recipe
"""
import os
import shutil

from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain, cmake_layout


def install_conan_package(name):
    result = False

    if "LUE_CONAN_PACKAGES" in os.environ:
        conan_package_names = os.environ["LUE_CONAN_PACKAGES"].split()
        result = name in conan_package_names

    return result


class LUERecipe(ConanFile):
    # Profile-wide variables. Factors that affect binary compatibility.
    settings = "os", "compiler", "build_type", "arch"

    def requirements(self):

        # TODO Here we can test the environment and only require packages that are actually needed
        # - Unfortunately, this doesn't allow our CMake logic to determine whether packages
        #   are required or not. Not a huge problem maybe. Just set the environment variables,
        #   depending on the platform, instead of setting/computing the CMake variables.
        # TODO Get rid of the LUE_HAVE_xxx variables.

        # Requirements also have traits:
        # - headers
        # - libs
        # - transitive_headers
        # - transitive_libs
        # - run
        # - build
        # - visible
        # - override
        # - force
        # - package_id_mode

        ### # LUE_BOOST_REQUIRED AND NOT LUE_HAVE_BOOST
        ### self.requires("boost/1.78.0")

        ### # LUE_DOCOPT_REQUIRED AND NOT LUE_HAVE_DOCOPT
        if install_conan_package("docopt.cpp"):
            self.requires("docopt.cpp/[>=0.6.3]")

        ### # LUE_FMT_REQUIRED AND NOT LUE_HAVE_FMT
        ### self.requires("fmt/9.0.0")

        ### # LUE_GDAL_REQUIRED AND NOT LUE_HAVE_GDAL
        ### self.requires("gdal/3.4.3")

        ### if install_conan_package("glfw"):
        ###     self.requires("glfw/[>3.3.7]")

        if install_conan_package("imgui"):
            self.requires("imgui/[>=1.89]")

        if install_conan_package("span-lite"):
            self.requires("span-lite/[>=0.10]")

        ### # LUE_HDF5_REQUIRED AND NOT LUE_HAVE_HDF5
        ### self.requires("hdf5/1.13.1")

        ### # LUE_NLOHMANN_JSON_REQUIRED AND NOT LUE_HAVE_NLOHMANN_JSON
        ### self.requires("nlohmann_json/3.10.5")

        def layout(self):
            cmake_layout(self)

    def generate(self):
        if install_conan_package("imgui"):
            cpp_info = self.dependencies["imgui"].cpp_info
            binding_directory_pathname = cpp_info.srcdirs[0]

            binding_filenames = [
                "imgui_impl_glfw.h",
                "imgui_impl_glfw.cpp",
                "imgui_impl_opengl3.h",
                "imgui_impl_opengl3.cpp",
                "imgui_impl_opengl3_loader.h",
                "imgui_impl_vulkan.h",
                "imgui_impl_vulkan.cpp",
            ]

            output_directory_pathname = os.path.join(
                self.build_folder, "source", "view", "imgui", "src"
            )

            os.makedirs(output_directory_pathname, exist_ok=True)

            for binding_filename in binding_filenames:
                shutil.copy(
                    os.path.join(binding_directory_pathname, binding_filename),
                    output_directory_pathname,
                )

        cmake = CMakeDeps(self)
        cmake.generate()

        # https://docs.conan.io/2/reference/tools/cmake/cmaketoolchain.html
        toolchain = CMakeToolchain(self, generator="Ninja")
        toolchain.generate()
