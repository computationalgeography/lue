.. _install_conda:

Install using Conda
===================
Installing LUE using Conda is as easy as:

.. code-block:: bash

   conda install -c conda-forge lue

Information about Conda can be found in the `Conda documentation`_. If
you do not have the conda command available yet, then you can install
`Miniconda`_. Information about the LUE Conda package is available at the
`LUE Anaconda Cloud page`_.

.. _Conda documentation: https://docs.conda.io/en/latest/
.. _Miniconda: https://docs.conda.io/en/latest/miniconda.html
.. _LUE Anaconda Cloud page: https://anaconda.org/conda-forge/lue

In case you just want to test things out, this is a workflow that will allow you to clean-up
once you are done (adjust versions / names when needed):

.. code-block:: bash

   # Keep Conda up to date
   conda update -n base conda

   # Create new Conda environment
   conda create --name test_lue python=3.9
   conda activate test_lue

   # Install LUE from the standard location, or ...
   conda install -c conda-forge lue

   # ... install LUE from a special location
   conda install -c url_to_special_channel -c conda-forge lue

   # Test
   # ...

   # Clean-up
   conda deactivate test_lue
   conda remove --name test_lue --all
