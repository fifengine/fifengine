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
- [ ] verify that wheels contain all dependencies
- [ ] switch to uv everywhere
   - [ ] build-tools
   - [ ] CI

- [ ] reintroduce audio/effects/soundeffect.h|cpp:
  - [ ] m_lateReverbDelay field
  - [ ] Implement the actual reverb delay logic in the audio processing
  - [ ] Add getter/setter methods for the reverb delay field
