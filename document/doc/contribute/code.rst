Code
====

Thank you for investing your time in contributing to our project!


Issues
------

Create a new issue
^^^^^^^^^^^^^^^^^^

If you spot a problem with LUE software or documentation, and a related
issue doesn't already exist, you can open a new issue using a relevant
`issue form <https://github.com/computationalgeography/lue/issues/new/choose>`_.


Solve an issue
^^^^^^^^^^^^^^

Scan through our `existing issues <https://github.com/computationalgeography/docs/issues>`_ to find
one that interests you. If you find an issue to work on, you are welcome to open a PR with a fix.


Make Changes
------------

1. Fork the repository.

2. Setup the development environment

3. Create a working branch and start with your changes!

   We name branches according to the issue number, e.g.: gh5 for the branch on Github in which
   the issue with number 5 got solved. You may want to do the same, but this is not a requirement.


Commit your update
------------------

Commit the changes once you are happy with them.


Pull Request
------------

When you're finished with the changes, create a pull request.

- Fill the "Ready for review" template so that we can review your PR. This template helps
  reviewers understand your changes as well as the purpose of your pull request.
- Don't forget to `link PR to issue <https://docs.github.com/en/issues/tracking-your-work-with-issues/linking-a-pull-request-to-an-issue>`_
  if you are solving one.
- Enable the checkbox to `allow maintainer edits <https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/allowing-changes-to-a-pull-request-branch-created-from-a-fork>`_
  so the branch can be updated for a merge. Once you submit your PR, a LUE team member will
  review your proposal. We may ask questions or request additional information.
- We may ask for changes to be made before a PR can be merged.
- As you update your PR and apply changes, mark each conversation as
  `resolved <https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/commenting-on-a-pull-request#resolving-conversations`_.


Documentation
-------------
Theme
~~~~~
The LUE documentation is generated using Sphinx. The configuration is
stored in `document/doc/conf.py.in`_.

The theme we use is an updated version of the `Read the Docs Sphinx
Theme`_. We forked that project and based our theme on release 0.5.0.
When LUE is configured and the documentation target is requested, the
CMake script will checkout our fork containing the LUE Sphinx Theme in
the build directory. The Sphinx configuration file will pick up the theme.

If you want to update the LUE Sphinx Theme, then this is more or less
the procedure:

.. code-block:: bash

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

The colours used in the theme are taken from the `Nord theme`_. The goal
of the theme is to use less colours and hover effects than the original
one.

The LUE-specific overrides of the underlying theme are located
in ``src/sass/_lue_theme_*.sass``. These SASS files are included from
``src/sass/theme.sass``.

When committing, skip ``package-lock.json``. It doesn't seem to be necessary
to commit.

.. important::

    Don't forget to generate and commit the theme's CSS
    (``sphinx_rtd_theme/static/css/theme.css``). Otherwise, the changes
    to the theme will not be visible to users.

Updating LUE to pick up the updated Sphinx theme involves these steps:

#. Note the commit hash in the updated ``lue_theme`` branch
#. Update commit hash in ``environment/cmake/LueConfiguration.cmake``
   (search for ``LUE_SPHINX_LUE_THEME_REQUIRED``)
#. If you use a local repository cache, pull in updates in the
   ``LUE_REPOSITORY_CACHE/sphinx_rtd_theme`` directory
#. Remove current theme sources from build directory:

   .. code-block:: bash

      cd $LUE_OBJECTS
      rm -r _deps/sphinx_lue_theme-*

#. Rebuild documentation:

   .. code-block:: bash

      cd $LUE_OBJECTS
      rm -r document/doc/sphinx/
      make sphinx_doc

.. _document/doc/conf.py.in: https://github.com/computationalgeography/lue/blob/master/document/doc/conf.py.in
.. _Read the Docs Sphinx Theme: https://sphinx-rtd-theme.readthedocs.io
.. _Nord theme: https://www.nordtheme.com
