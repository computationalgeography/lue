# Release

Releases are automatically archived on Zenodo. After making a release, a DOI is made available by Zenodo which
can be used in publications.

Useful links:

- https://guides.github.com/activities/citable-code/
- [conda-forge maintainer documentation](https://conda-forge.org/docs/maintainer/)
- [LUE feedstock repository](https://github.com/conda-forge/lue-feedstock)
- [LUE feedstock builds](https://dev.azure.com/conda-forge/feedstock-builds/_build?definitionId=10907&_a=summary)


## Prepare

- [ ] Verify that the version number in the main `CMakeLists.txt` is correct
- [ ] Verify that the Releases page in the documentation contains a section for the new version
- [ ] Update list of authors in `CITATION.cff`, if necessary
- [ ] Initiate a Conda-Forge PR:
    - [ ] Create a zip and determine sha256:

        ```bash
        cd $HOME/tmp
        version="<version>"
        branch="master"
        git clone --depth 1 https://github.com/computationalgeography/lue.git --branch ${branch} \
            --single-branch lue-${version}
        tar cjf lue-${version}.tar.bz2 lue-${version}
        openssl sha256 lue-${version}.tar.bz2
        ```

    - [ ] Upload zip to `https://lue.computationalgeography.org/download/lue-<version>.tar.bz2`
- [ ] Wait for PR to appear on https://github.com/conda-forge/lue-feedstock/pulls . This can take hours.
- [ ] Checkout the PR's branch and update the recipe files:
    - [ ] Verify sha256 is the same as the one found above
    - [ ] Verify the build number is correct (zero for new version, bumped if not)
    - [ ] Compare files with the ones in LUE's `environment/conda` directory. Harmonize.
    - [ ] Update project's description if needed (feedstock's `README.md`)
    - [ ] Verify `@conda-forge-admin, please rerender` has been run
    - [ ] Do whatever it takes to make the builds succeed
        - In case the sources must be updated, an update of the hash in the feedstock's `meta.yaml` will
          trigger a rebuild of the packages
    - [ ] Verify all build logs make sense. If not, fix things or create a GH issue to get it fixed before the
          next release.


## Release

- [ ] Draft a release on GitHub. We do not use the letter v to prefix version numbers.
    - [ ] Create a new zip with the same name and determine sha256:

        ```bash
        cd $HOME/tmp
        version="<version>"
        branch="${version}"
        git clone --depth 1 https://github.com/computationalgeography/lue.git --branch ${branch} \
            --single-branch lue-${version}
        tar cjf lue-${version}.tar.bz2 lue-${version}
        openssl sha256 lue-${version}.tar.bz2
        ```

    - [ ] Upload zip to `https://lue.computationalgeography.org/download/lue-<version>.tar.bz2`
    - [ ] Update the recipe files in the PR:
        - [ ] Verify sha256 is the same as the one found above
    - [ ] Wait for all builds to succeed
    - [ ] Add build times to the table
- [ ] Merge PR
    - [ ] Double check build number is correct
    - [ ] Wait for all builds to succeed
- [ ] Revisit any outstanding PRs:
    - [ ] Close the ones handled by the new release
    - [ ] Rebase and merge the others
- [ ] Write blog post about the release
- [ ] Post about the release on [Mastodon](https://scicomm.xyz/@lue)
- [ ] Update [tutorial repo](https://github.com/computationalgeography/lue_tutorial)
    - [ ] Bump LUE version number in `{main,develop}/environment/configuration/conda_environment.yml`
    - [ ] Verify actions still succeed, or fix things until they do
