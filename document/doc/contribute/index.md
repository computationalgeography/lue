(doc-contribute)=

# Contribute

Thank you for investing your time in contributing to this project!

```{note}
If you are a model or software developer interested in *using* LUE, then you can safely skip everything in
this part of the manual. You will likely not need to know about the contents.
```


## Copyright and licensing

- Copyright of LUE work that is part of the LUE source repository stays with the individual contributor
- New code files should include a [short-form SPDX ID](https://spdx.dev/learn/handling-license-info/) at the
  top, indicating the project license for code, which is MIT. This should look like the following:

    ```
    // SPDX-License-Identifier: MIT
    ```

- A contributor must agree to the [Developer Certificate of Origin](developer_certificate_of_origin.txt)
- All contributions must include a "Signed-off-by" line in the commit message, with the contributors' name and
  email address. See also Git's [--sign-off
  argument](https://git-scm.com/docs/git-commit#Documentation/git-commit.txt---signoff) which makes signing
  trivial to do.


## Issues

### Create a new issue

If you spot a problem with LUE software or documentation, and a related issue doesn't already exist, you can
open a new issue using a relevant [issue
form](https://github.com/computationalgeography/lue/issues/new/choose).


### Solve an issue

Scan through our [existing issues](https://github.com/computationalgeography/lue/issues) to find one that
interests you. If you find an issue to work on, you are welcome to open a PR with a fix.


## Make changes

1. Fork the repository.
1. Setup the development environment
1. Create a working branch and start with your changes!

   We name branches according to the issue number, e.g.: gh5 for the branch on Github in which
   the issue with number 5 gets solved. You may want to do the same, but this is not a requirement.


## Commit your update

Commit the changes once you are happy with them.


## Create a pull request

When you're finished with the changes, create a pull request.

- Fill the "Ready for review" template so that we can review your PR. This template helps reviewers understand
  your changes as well as the purpose of your pull request.
- Don't forget to [link PR to
  issue](https://docs.github.com/en/issues/tracking-your-work-with-issues/linking-a-pull-request-to-an-issue)
  if you are solving one.
- Enable the checkbox to [allow maintainer
  edits](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/allowing-changes-to-a-pull-request-branch-created-from-a-fork)
  so the branch can be updated for a merge. Once you submit your PR, a LUE team member will review your
  proposal. We may ask questions or request additional information.
- We may ask for changes to be made before a PR can be merged.
- As you update your PR and apply changes, mark each conversation as
  [resolved](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/commenting-on-a-pull-request#resolving-conversations).
