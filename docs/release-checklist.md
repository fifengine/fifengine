# Fifengine Release Steps

This is a checklist documenting the steps required to perform a Fifengine release.

We need this document, because maintainers might change and we need to transfer the knowledge
of "how to do a release properly" to the next person having the maintainer role.
In the end, it should form a checklist guide for the maintainer.

We'll refine the checklist over time, so that it becomes more complete.

## 1. Prerequisites

- [ ] You need to be a maintainer (with push access to the repository).
- [ ] You need to have access to GitHub Actions
  - so that you can delete and re-trigger builds manually when things go wrong during automated builds and releases.
- [ ] Your local Git repository must be synced with the main branch.

## 2. Release Preparation & Versioning

- [ ] Pick a new version number (e.g., 1.2.3).
- [ ] Update to the new version number.
  - [ ] Update vcpgk.json.
  - [ ] Then run `./build-tools/set-version.sh` to update the version in all relevant files.
- [ ] Update AUTHORS.
- [ ] Update CHANGELOG.
  - [ ] Roll changelog data from the "Unreleased" section to the new version section.
  - [ ] Link the new header (add a new reference at the end of the changelog).
  - [ ] Fix the link to Unreleased (update the old version number with the new version number).
- [ ] git commit + git push — add changes until everything is updated and ready.

### Pick Release Commit

   - this is the golden commit = the base for the release tag
   - [ ] check commit message: "Release x.y.z" or "Updating Changelog for x.y.z"
   - [ ] check that the commit message doesn't contain `[skip ci]`
   - [ ] check that the builds of this commit are all green on CI services, before you tag!

## 3. Release Execution (Tagging)

The release is performed automatically by tagging the commit. The build artifacts of the tag are then released to GitHub Releases.

- [ ] Identify the hash of the verified Golden Commit.
- [ ] `git tag -a <tagname=version> <git-hash of release commit>`
   - e.g. `git tag -a 1.2.3 9fceb02`: you are tagging the commit 9fceb02 as version 1.2.3.
- [ ] `git push origin <tagname=version>`
   - e.g. `git push origin 1.2.3`: this pushes the single tag "0.1.3" to "origin"
- [ ]  Wait for CI services to complete the tag build.
- [ ]  Wait for CI services to deploy the build artifacts to GitHub Releases.

Note: If things go bust, delete the tags locally and then manually via the Github Interface, before you try to release the same tag.

## 4. Post-Release

- [ ] Verify the release page: check that all build artifacts for the release are attached.
- [ ] Announce the release in the Discord channel.
- [ ] Update the Overview channel in the Unknown Horizons Discord server.
      - (Optional) Update the website: update download links.
- [ ] update Wikipedia entries
    - https://fr.wikipedia.org/wiki/Flexible_Isometric_Free_Engine

## 5. Emergency Rollback (If Build Fails)

- [ ] Delete local tag: `git tag -d <version>`
- [ ] Delete remote tag via CLI: `git push --delete origin <version>` or delete it via the GitHub UI.
- [ ] Fix issues on the branch.
- [ ] Create a new commit.
- [ ] Repeat the process starting from Step 2.
