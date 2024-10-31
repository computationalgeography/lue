# Release

Releases are automatically archived on Zenodo. After making a release, a DOI is made available by Zenodo which
can be used in publications.

- https://guides.github.com/activities/citable-code/


Checklist:

- [ ] Before the release:

    - [ ] Make sure the version number in the main CMakeLists.txt is correct
    - [ ] Make sure the releases page in the documentation is correct
    - [ ] Draft a release on Github. Currently we do not use the letter v to prefix version numbers.

- [ ] After the release:

    - [ ] Bump the version number in the main CMakeLists.txt
    - [ ] Update the releases page in the documentation



## Conda package

Conda-Forge detects new LUE releases automatically. In our case, Conda-Forge looks in a specific location for
a zipped tar file containing the LUE source files
(`https://pcraster.geo.uu.nl/pcraster/packages/src/lue-<version>.tar.bz2`). The first step in creating a
Conda package for a new release of LUE, is to upload a new zipped tar file to this location. Example workflow:

```bash
# Create a directory containing (only) the sources associated with the new release. Currently
# it is relevant that this directory contains the .git subdirectory.
mkdir -p tmp  # Whatever
cd tmp
# Update for actual tag associated with release
git clone --depth 1 https://github.com/computationalgeography/lue.git --branch 0.3.4 --single-branch
tar cjf lue-0.3.4.tar.bz2 lue
# Determine hash, to store in meta.yaml
openssl sha256 lue-0.3.4.tar.bz2
# ... ftp zip to ftp server ...
```

It may take a few hours for Conda-Forge to detect the new release. Once it has, a pull request is created
automatically, and all kinds of tests and builds are started automatically. These may or may not succeed.
Information about this can be found on the [lue-feedstock page](https://github.com/conda-forge/lue-feedstock)
at Github.

The pull request is based on a new branch in a clone of the `lue-feedstock` repository, located in the
`regro-cf-autotick-bot` Github organisation. To make changes to this branch, the following workflow can be
used:

```bash
# Clone the repository and checkout the branch associated with the PR
mkdir -p development/project/github/regro-cf-autotick-bot  # Whatever
cd development/project/github/regro-cf-autotick-bot
git clone git@github.com:regro-cf-autotick-bot/lue-feedstock.git
git checkout 0.3.4_h813c5f  # Update for actual branch mentioned in PR
# ... make changes ...
git push
```

```{important}
Before merging the changes in the pull request's branch the build number in `meta.yaml` must be correct:

- In case LUE's version was bumped, the build number must be 0. This is done by the bot that created the pull
  request in the first place.
- In case LUE's version was not bumped, the build number must be bumped by 1. In this case there was some
  other reason for rebuilding a Conda package.
```

Once the PR is fine, merge it into the main branch.
