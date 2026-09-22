# Release

Releases are automatically archived on Zenodo. After making a release, a DOI is made available by Zenodo which
can be used in publications.

Useful links:

- <https://guides.github.com/activities/citable-code/>
- [conda-forge maintainer documentation](https://conda-forge.org/docs/maintainer/)
- [LUE feedstock repository](https://github.com/conda-forge/lue-feedstock)
- [LUE feedstock builds](https://dev.azure.com/conda-forge/feedstock-builds/_build?definitionId=10907&_a=summary)

## Prepare

- [ ] Create an issue + branch for preparing the release
- [ ] Update list of authors in `CITATION.cff`, if necessary
- [ ] Update the Releases page in the documentation to contain a section for the new version

### Trigger a PR in the [conda-forge/lue-feedstock](https://github.com/conda-forge/lue-feedstock) repository

````{tab-set}
```{tab-item} Let Conda-Forge create the PR
- [ ] Initiate (no need to create one yourself) a Conda-Forge PR:
  - [ ] Create a zip and determine sha256:

    ```bash
    cd $HOME/tmp
    # This command currently only results in a good zip on Linux. On macOS the zip doesn't seem to be
    # correct.
    prepare_lue_release.sh kordejong 0.3.10 gh861
    ```

  - [ ] Upload zip to `https://lue.computationalgeography.org/download/lue-<version>.tar.bz2`
  - [ ] Wait for PR to appear on <https://github.com/conda-forge/lue-feedstock/pulls> . This can take hours.
- [ ] Checkout the PR's branch and update the recipe files:
  - [ ] Verify the sha256 is the same as the one found above
  - [ ] Update the feestock (see items below this block)
  - [ ] In case the sources must be updated, an update of the hash in the feedstock's `meta.yaml` will
    trigger a rebuild of the packages
```

```{tab-item} Create the PR yourself (faster)
- [ ] Create a branch in a fork of the lue-feedstock repository
- [ ] Update the recipe files:
  - [ ] Point `source/git_url` to the LUE repository
  - [ ] Update the feestock (see items below this block)
  - [ ] In case the sources must be updated, a push of an empty commit will trigger a rebuild of the
    packages:

    ```bash
    git commit --allow-empty -m "LUE master changed, trigger rebuild"
    git push
    ```
```
````

- [ ] Update the feedstock:

  - [ ] Verify the build number is correct (zero for new version, bumped if not)
  - [ ] Compare files with the ones in LUE's `environment/conda` directory. Harmonize.
  - [ ] Update project's description if needed (feedstock's `README.md`)
  - [ ] If needed, push the branch and create a PR
  - [ ] Verify `@conda-forge-admin, please rerender` has been run
  - [ ] Do whatever it takes to make the builds succeed.

## Release

- [ ] Draft a release on GitHub. We do not use the letter v to prefix version numbers.
  - [ ] Create a new zip with the same name and determine sha256:

    ```bash
    cd $HOME/tmp
    prepare_lue_release.sh computationalgeography 0.3.10 master
    ```

  - [ ] Upload zip to `https://lue.computationalgeography.org/download/lue-<version>.tar.bz2`

  - [ ] Update the recipe files in the PR:
    - [ ] Replace the sha256 with one just found
  - [ ] Wait for all builds to succeed
- [ ] Merge Conda-Forge PR
- [ ] Postprocess release:
  - [ ] Bump version number in:
    - [ ] The main `CMakeLists.txt`
    - [ ] `environment/conda/meta.yaml`
  - [ ] Write blog post about the release
  - [ ] Post about the release on [Mastodon](https://scicomm.xyz/@lue)
  - [ ] Update [tutorial repo](https://github.com/computationalgeography/lue_tutorial)
    - [ ] Bump LUE version number in `{main,develop}/environment/configuration/conda_environment.yml`
    - [ ] Verify actions still succeed, or fix things until they do
