# Documentation

:::{warning}
Partly outdated information...
:::

## Theme

The LUE documentation is generated using Sphinx. The configuration is stored in
[document/doc/conf.py.in](https://github.com/computationalgeography/lue/blob/master/document/doc/conf.py.in).

The theme we use is an updated version of the [Read the Docs Sphinx
Theme](https://sphinx-rtd-theme.readthedocs.io). We forked that project and based our theme on release 0.5.0.
When LUE is configured and the documentation target is requested, the CMake script will checkout our fork
containing the LUE Sphinx Theme in the build directory. The Sphinx configuration file will pick up the theme.

If you want to update the LUE Sphinx Theme, then this is more or less the procedure:

```bash
# Initialize a Conda environment specifically for changing the theme
cd lue
conda env create -f environment/configuration/conda_rtd_environment.yml
conda activate lue_rtd

# Checkout the theme's sources from its branch
cd ..
git clone git@github.com:computationalgeography/sphinx_rtd_theme.git
cd sphinx_rtd_theme
git checkout lue_theme

# Install software required for testing and generating the theme
# See https://sphinx-rtd-theme.readthedocs.io/en/latest/contributing.html
pip install -e '.[dev]'
npm install

# Start an HTTP server that will pick-up changes to SASS files
npm run dev

# Change the theme's SASS files
# ...

# Generate the theme's CSS
npm run build

# Check in changes to SASS files and CSS files
# ...
```

The colours used in the theme are taken from the [Nord theme](https://www.nordtheme.com). The goal of the
theme is to use less colours and hover effects than the original one.

The LUE-specific overrides of the underlying theme are located in `src/sass/_lue_theme_*.sass`. These SASS
files are included from `src/sass/theme.sass`.

When committing, skip `package-lock.json`. It doesn't seem to be necessary to commit.

:::{important}
Don't forget to generate and commit the theme's CSS (`sphinx_rtd_theme/static/css/theme.css`). Otherwise,
the changes to the theme will not be visible to users.
:::

Updating LUE to pick up the updated Sphinx theme involves these steps:

1. Note the commit hash in the updated `lue_theme` branch
1. Update commit hash in `environment/cmake/LueConfiguration.cmake` (search for
   `LUE_SPHINX_LUE_THEME_REQUIRED`)
1. If you use a local repository cache, pull in updates in the `LUE_REPOSITORY_CACHE/sphinx_rtd_theme`
   directory
1. Remove current theme sources from build directory:

    ```bash
    cd $LUE_OBJECTS
    rm -r _deps/sphinx_lue_theme-*
    ```

1. Rebuild documentation:

    ```bash
    cd $LUE_OBJECTS
    rm -r document/doc/sphinx/
    make sphinx_doc
    ```
