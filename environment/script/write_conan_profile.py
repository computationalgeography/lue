#!/usr/bin/env python
"""
Implementation of script for dumping Conan profiles that can be used to install Conan packages
that can be used during the build of LUE.
"""
import os
import platform
import subprocess
import sys

import docopt
from ruamel.yaml import YAML


# arch=armv8
# build_type=Release
# compiler=apple-clang
# compiler.cppstd=gnu17
# compiler.libcxx=libc++
# compiler.version=14
# os=Macos


# arch=x86_%ARCH%
# build_type=Release
# compiler=msvc
# compiler.version=%compiler_version%
# compiler.runtime=dynamic
# compiler.cppstd=17
# os=Windows


def conan_os():
    """
    Return the operating system
    """
    os_by_system = {
        "Darwin": "Macos",
        "Linux": "Linux",
        "Windows": "Windows",
    }
    system = platform.system()

    return os_by_system[system]


def conan_arch():
    """
    Return the architecture
    """
    arch_by_machine = {
        "x86_64": "x86_64",
    }
    machine = platform.machine()

    return arch_by_machine[machine]


def global_settings():
    """
    Return a dictionary with the global settings
    """
    os = conan_os()
    build_type = "Release"
    arch = conan_arch()

    return {
        "os": os,
        "arch": arch,
        "build_type": build_type,
    }


def compiler_is_gcc(compiler_filename):
    """
    Return whether the compiler is the GCC compiler
    """
    basename = os.path.basename(compiler_filename)

    if "gcc" in basename:
        return True
    elif "g++" in basename:
        return True

    return False


def compiler_is_apple_clang(compiler_filename):
    """
    Return whether the compiler is the Apple Clang compiler
    """
    basename = os.path.basename(compiler_filename)

    if conan_os() == "Macos":
        if "clang" in basename:
            return True

    return False


def compiler_is_clang(compiler_filename):
    """
    Return whether the compiler is the Clang compiler
    """
    basename = os.path.basename(compiler_filename)

    if conan_os() != "Macos":
        if "clang" in basename:
            return True

    return False


def compiler_is_msvc(compiler_filename):
    """
    Return whether the compiler is the MSVC compiler
    """
    basename = os.path.basename(compiler_filename)

    if basename in ["cl", "cl.exe"]:
        return True

    return False


def gcc_version(compiler_filename):
    """
    Return the version of the GCC compiler
    """
    command = f"{compiler_filename} -dumpversion"
    output = subprocess.run(command, capture_output=True, shell=True, text=True)

    assert output.returncode == 0, f"{command}: {output.stderr}"

    version = output.stdout.strip()

    # Some version return only a major version, some major.minor.patch
    version = version.split(".")[0]

    return version


def clang_version(compiler_filename):
    """
    Return the version of the Clang compiler
    """
    return gcc_version(compiler_filename)


def gcc_settings(compiler_filename):
    """
    Return a dictionary with the GCC compiler settings
    """
    compiler = "gcc"
    compiler_cppstd = "17"
    compiler_libcxx = "libstdc++11"
    compiler_version = gcc_version(compiler_filename)

    return {
        "compiler": compiler,
        "compiler.cppstd": compiler_cppstd,
        "compiler.libcxx": compiler_libcxx,
        "compiler.version": compiler_version,
    }


def apple_clang_settings(compiler_filename):
    """
    Return a dictionary with the Apple Clang compiler settings
    """
    compiler = "apple-clang"
    compiler_cppstd = "17"
    compiler_libcxx = "libc++"
    compiler_version = clang_version(compiler_filename)

    return {
        "compiler": compiler,
        "compiler.cppstd": compiler_cppstd,
        "compiler.libcxx": compiler_libcxx,
        "compiler.version": compiler_version,
    }


def clang_settings(compiler_filename):
    """
    Return a dictionary with the Clang compiler settings
    """
    compiler = "clang"
    compiler_cppstd = "17"
    compiler_libcxx = "libc++"
    compiler_version = clang_version(compiler_filename)

    return {
        "compiler": compiler,
        "compiler.cppstd": compiler_cppstd,
        "compiler.libcxx": compiler_libcxx,
        "compiler.version": compiler_version,
    }


def msvc_settings(compiler_filename):
    """
    Return a dictionary with the MSVC compiler settings
    """
    assert False, "TODO"
    return {}


def compiler_settings(compiler_filename):
    """
    Return a dictionary with the compiler settings
    """
    if compiler_is_gcc(compiler_filename):
        settings = gcc_settings(compiler_filename)
    elif compiler_is_apple_clang(compiler_filename):
        settings = apple_clang_settings(compiler_filename)
    elif compiler_is_clang(compiler_filename):
        settings = clang_settings(compiler_filename)
    elif compiler_is_msvc(compiler_filename):
        settings = msvc_settings(compiler_filename)
    else:
        raise RuntimeError(f"Unknown compiler: {compiler_filename}")

    return settings


def conan_home_pathname():
    """
    Return the pathname of the Conan home directory
    """
    command = "conan config home"
    output = subprocess.run(
        command, capture_output=True, check=False, shell=True, text=True
    )

    assert output.returncode == 0, f"{output.stderr}"

    pathname = output.stdout.strip()

    return pathname


def conan_settings_pathname():
    """
    Return the pathname to the global Conan settings file
    """
    return os.path.join(conan_home_pathname(), "settings.yml")


yaml = YAML()
yaml.preserve_quotes = True
yaml.indent(mapping=4, sequence=4)


def read_conan_settings():
    """
    Read the global Conan settings file
    """
    pathname = conan_settings_pathname()

    with open(pathname, "r") as conan_settings_file:
        settings = yaml.load(conan_settings_file)

    # We may be changing the settings file. Get rid of the toplevel comment to prevent Conan
    # from overwriting it.

    if settings.ca.comment is not None:
        settings.ca.comment[1] = None

    return settings


def write_conan_settings(conan_settings):
    """
    Overwrite the Conan global settings file
    """
    pathname = conan_settings_pathname()

    with open(pathname, "w") as conan_settings_file:
        yaml.dump(conan_settings, conan_settings_file)


def write_conan_profile(compiler_filename, profile_pathname):
    """
    Write a Conan profile
    """
    profile_settings = {}
    profile_settings |= global_settings()
    profile_settings |= compiler_settings(compiler_filename)

    if "compiler.version" in profile_settings:
        compiler = profile_settings["compiler"]
        compiler_version = profile_settings["compiler.version"]

        conan_settings = read_conan_settings()
        compiler_versions = conan_settings["compiler"][compiler]["version"]

        # This happens if the compiler is more recent than the Conan settings file. Here,
        # we just add the newer version to the list of supported versions. Otherwise Conan wil
        # not install packages.
        if compiler_version not in compiler_versions:

            String = type(compiler_versions[0])

            compiler_versions.append(String(compiler_version))

            write_conan_settings(conan_settings)

    with open(profile_pathname, "w") as profile_file:
        profile_string = "[settings]:\n{}\n".format(
            "\n".join([f"{key}={profile_settings[key]}" for key in profile_settings])
        )
        profile_file.write(profile_string)


def main():
    usage = """\
Write a Conan profile

Usage:
    {command} <compiler> <profile>

Options:
    compiler   C/C++ compiler to write profile for. Must be the name of an
               executable whose location is in the PATH, or a pathname to an
               executable.
    profile    Pathname of profile to write
    -h --help  Show this screen

This command tries to write a Conan profile that is useful for installing
packages that can be used to build LUE with. It is not a replacement for the
'conan profile detect' command.

As a side effect, Conan's settings.yml may be updated in case it does not yet
contain the version of the compiler.
""".format(
        command=os.path.basename(sys.argv[0])
    )

    arguments = docopt.docopt(usage)

    compiler_filename = arguments["<compiler>"]
    profile_pathname = arguments["<profile>"]

    write_conan_profile(compiler_filename, profile_pathname)


if __name__ == "__main__":
    sys.exit(main())
