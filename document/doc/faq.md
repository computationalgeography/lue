# FAQ

## Why does importing the LUE Python package crash on Linux?

The HPX library used by LUE can optionally by built with support for a custom memory allocator. This is a good
idea, since such an allocator is more efficient than the default "system" memory allocator. LUE might crash
when Python (using the default memory allocator) imports LUE (using a custom memory allocator) since
deallocation of memory allocated with the default memory allocator now happens with the deallocation function
from the custom memory allocator.

The solution is to make the whole Python process use the allocation and deallocation functions from the custom
memory allocator library. This can be done by adding the pathname of the shared library containing the custom
memory allocator to the `LD_PRELOAD` variable.

In case LUE is built with support for the tcmalloc memory allocator, starting LUE Python scripts like this
solves the crash:

```bash
LD_PRELOAD=<prefix>/lib/libtcmalloc_minimal.so.4 python my_lue_script.py
```

Replace `<prefix>` by the location of the lib directory containing the library. In case the LUE Conda package
is used, this command can be generalized like this (assuming the Bash shell is used):

```bash
LD_PRELOAD=$(find $CONDA_PREFIX -name libtcmalloc_minimal.so.4) python my_lue_script.py
```

Depending on the actual memory allocator used, the name of the library to preload will be different.

Expand (don't overwrite) the `LD_PRELOAD` variable in case it is already set.
