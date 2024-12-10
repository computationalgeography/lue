(source-framework-howto-trace)=

# Trace framework code

Tracing framework code allows you to gain insights into what happens at runtime when LUE performs work. This
can help when optimizing the code.

Here is a quick list of the steps:

1. Configure an HPX build to use APEX, e.g.:

    ```cmake
    -D HPX_WITH_APEX=TRUE -D HPX_WITH_FETCH_APEX=TRUE
    ```

    See also [CMakeHPXPresets](https://github.com/computationalgeography/lue/blob/master/CMakeHPXPresets.json)
    which contains a `hpx_relwithdebinfo_configuration` section inherited by CMake presets for building a
    suitable HPX version for tracing.

1. Add annotations to your code, using `lue::AnnotateFunction`. For an example, see
   [clump.hpp](https://github.com/computationalgeography/lue/blob/master/source/framework/algorithm/include/lue/framework/algorithm/definition/clump.hpp)
1. Request APEX to dump trace information, by setting one or more environment variables or using the
   `apex_exec` program: https://uo-oaciss.github.io/apex/quickstarthpx
1. If it works, dumping trace info in a format that is importable by Perfetto can be especially useful
   (https://ui.perfetto.dev/). It doesn't always seem to work, though.

See also:

- https://hpx-docs.stellar-group.org/latest/html/manual/cmake_variables.html
- https://www.speedscope.app/
