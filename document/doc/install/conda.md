(install-conda)=

# Install using Conda

The LUE Conda package is a great way to get LUE installed quickly on your laptop or desktop computer. For
maximum portability, it is built using general sane settings. This version is fine for demo's, testing models,
and in education, for example. But,

:::{warning}
In case you want the best possible performance, you must build LUE yourself, from the sources and using
settings that match your specific platform.
:::

Building LUE yourself is also required if you want to perform distributed calculations, using multiple
cooroperating processes. For more information about building LUE, see [](#install-source).

Installing LUE using Conda is as easy as:

```bash
conda install -c conda-forge lue
```

Information about Conda can be found in the [Conda documentation](https://docs.conda.io/en/latest/). If you do
not have the conda command available yet, then you can install
[Miniconda](https://docs.conda.io/en/latest/miniconda.html). Information about the LUE Conda package is
available at the [LUE Anaconda Cloud page](https://anaconda.org/conda-forge/lue).

In case you just want to test things out, this is a workflow that will allow you to clean-up once you are done
(adjust versions / names when needed):

```bash
# Keep Conda up to date
conda update -n base -c defaults conda

# Create new Conda environment
conda create --name test_lue python=3.10
conda activate test_lue

# Install LUE
conda install -c conda-forge lue

# Test
# ...

# Clean-up
conda deactivate
conda env remove --name test_lue
```

For inspiration for testing LUE you can have a look at the
[lue_tutorial](https://github.com/computationalgeography/lue_tutorial) repository.
