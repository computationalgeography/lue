"""
Conan recipe
"""
from conan import ConanFile
from conan.tools.cmake import cmake_layout


# TODO Have multiple of these, for different LUE targets?
class LUE(ConanFile):
    # Profile-wide variables
    settings = "os", "compiler", "build_type", "arch"
    # generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        # TODO Here we can test the environment and only require packages that are actually needed
        # - Unfortunately, this doesn't allow our CMake logic to determine whether packages
        #   are required or not. Not a huge problem maybe. Just set the environment variables,
        #   depending on the platform, instead of setting/computing the CMake variables.
        # TODO Get rid of the LUE_HAVE_xxx variables.
        # TODO Can the version names be ranges?

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

        # LUE_BOOST_REQUIRED AND NOT LUE_HAVE_BOOST
        self.requires("boost/1.78.0")

        # LUE_DOCOPT_REQUIRED AND NOT LUE_HAVE_DOCOPT
        self.requires("docopt.cpp/0.6.3")

        # LUE_FMT_REQUIRED AND NOT LUE_HAVE_FMT
        self.requires("fmt/9.0.0")

        # LUE_GDAL_REQUIRED AND NOT LUE_HAVE_GDAL
        self.requires("gdal/3.4.3")

        # LUE_GLFW_REQUIRED AND NOT LUE_HAVE_GLFW
        self.requires("glfw/3.3.7")

        # LUE_IMGUI_REQUIRED AND NOT LUE_HAVE_IMGUI
        # LUE_CONAN_IMPORTS
        #     "./res/bindings, imgui_impl_glfw.h -> ${CMAKE_BINARY_DIR}/source/view/imgui/src"
        #     "./res/bindings, imgui_impl_glfw.cpp -> ${CMAKE_BINARY_DIR}/source/view/imgui/src"
        #     "./res/bindings, imgui_impl_opengl3.h -> ${CMAKE_BINARY_DIR}/source/view/imgui/src"
        #     "./res/bindings, imgui_impl_opengl3.cpp -> ${CMAKE_BINARY_DIR}/source/view/imgui/src"
        #     "./res/bindings, imgui_impl_opengl3_loader.h -> ${CMAKE_BINARY_DIR}/source/view/imgui/src"
        #     "./res/bindings, imgui_impl_vulkan.h -> ${CMAKE_BINARY_DIR}/source/view/imgui/src"
        #     "./res/bindings, imgui_impl_vulkan.cpp -> ${CMAKE_BINARY_DIR}/source/view/imgui/src"
        self.requires("imgui/1.89.4")

        # LUE_MS_GSL_REQUIRED AND NOT LUE_HAVE_MS_GSL
        self.requires("ms-gsl/4.0.0")

        # LUE_HDF5_REQUIRED AND NOT LUE_HAVE_HDF5
        self.requires("hdf5/1.13.1")

        # LUE_NLOHMANN_JSON_REQUIRED AND NOT LUE_HAVE_NLOHMANN_JSON
        self.requires("nlohmann_json/3.10.5")

        def layout(self):
            cmake_layout(self)

            ### # We make the assumption that if the compiler is msvc the
            ### # CMake generator is multi-config
            ### # TODO Can this be queried somehow?
            ### multi = True if self.settings.get_safe("compiler") == "msvc" else False

            ### # TODO Instead of build, install in $LUE_OBJECTS

            ### if multi:
            ###     self.folders.generators = os.path.join("build", "generators")
            ### else:
            ###     self.folders.generators = os.path.join("build", str(self.settings.build_type), "generators")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        toolchain = CMakeToolchain(self)

        # TODO Is this the way to get custom things in the toolchain?
        # - See docs. There is a way to include custom toolchain stuff as well, it seems
        # https://docs.conan.io/2/reference/tools/cmake/cmaketoolchain.html#cmaketoolchain
        # toolchain.variables["MYVAR"] = "MYVAR_VALUE"
        # toolchain.preprocessor_definitions["MYDEFINE"] = "MYDEF_VALUE"

        toolchain.generate()
