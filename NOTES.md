# Notes

## TODO

- [ ] enhance CMake install() export
   - see https://cmake.org/cmake/help/latest/command/install.html#export
   - [ ] use COMPONENT definitions
- [ ] add development export?
- [ ] try use these COMPONENT definitions in pyproject.toml for wheel building?
   - https://scikit-build-core.readthedocs.io/en/stable/configuration/index.html
- [ ] upload wheels to PyPI
   - see https://packaging.python.org/en/latest/guides/publishing-package-distribution-releases-using-github-actions-ci-cd-workflows/
   - accounts needed:
     - [ ] testing, see https://packaging.python.org/en/latest/guides/using-testpypi/
     - [ ] production

- [ ] reintroduce `m_lateReverbDelay` field to `audio/effects/soundeffect.h`|cpp:
  - this was flagged as unused, probably half-baked feature
  - [ ] Implement the actual reverb delay logic in the audio processing
  - [ ] Add getter/setter methods for the reverb delay field

## Unknown Horizons

- Open Issues with "fife" label:
  - https://github.com/unknown-horizons/unknown-horizons/issues?q=is%3Aissue%20state%3Aopen%20label%3Afife
- Focus on:
  - SDL3 migration
  - transparency and alpha blending issues
  - HDPI + window + scaling + fullscreen + multi-monitor support
  - Swig "builtin"

## Clang Tidy

- [ ] reduce number of clang-tidy warnings
- [ ] work order: low prio to high prio

- Baseline, 10.05.2026, L9149
  - https://github.com/fifengine/fifengine/actions/runs/25628339279/job/75227331476#step:21:9149
- 12.05.2026: L8023
  - https://github.com/fifengine/fifengine/actions/runs/25735020103/job/75569611392#step:22:8024
