#!/usr/bin/env python

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Test runner for FIFE engine core, extensions, and SWIG bindings."""

import optparse
import os
import re
import shutil
import subprocess
import sys
from glob import glob

DEFAULT_MODULE_TIMEOUT = 120
DEFAULT_ANIMATION_MODULE_TIMEOUT = 90


def _env_truthy(value):
    if value is None:
        return False
    return str(value).strip().lower() in ("1", "true", "yes", "on")


def resolve_headless_mode(cli_headless):
    """Resolve headless behavior.

    Priority:
    1) Explicit CLI flags (--headless/--windowed)
    2) FIFE_TEST_HEADLESS env var
    3) CI env defaults to headless
    4) default windowed
    """
    if cli_headless is not None:
        return cli_headless

    if "FIFE_TEST_HEADLESS" in os.environ:
        return _env_truthy(os.environ.get("FIFE_TEST_HEADLESS"))

    if _env_truthy(os.environ.get("CI")):
        return True

    return False


def genpath(somepath):
    """Convert Unix-style path separators to OS-specific separators."""
    return os.path.sep.join(somepath.split("/"))


def has_python_bindings(build_dir):
    """Check if Python bindings are available in the build directory."""
    if not os.path.isdir(build_dir):
        return False
    if not os.path.exists(os.path.join(build_dir, "fife.py")):
        return False
    return bool(glob(os.path.join(build_dir, "_fife_swig*")))


def resolve_build_dir():
    """Resolve the build directory containing Python bindings."""
    env_build_dir = os.environ.get("FIFE_BUILD_DIR")
    if env_build_dir:
        return env_build_dir

    legacy_build_dir = genpath("build")
    if has_python_bindings(legacy_build_dir):
        return legacy_build_dir

    preset_candidates = []
    for candidate in sorted(glob(genpath("out/build/*"))):
        if has_python_bindings(candidate):
            preset_candidates.append(candidate)

    if len(preset_candidates) == 1:
        return preset_candidates[0]

    if preset_candidates:
        return max(preset_candidates, key=os.path.getmtime)

    # Also consider the top-level out/build directory (not only out/build/*)
    out_build_dir = genpath("out/build")
    if has_python_bindings(out_build_dir):
        return out_build_dir

    return legacy_build_dir


def _unique_existing_paths(paths):
    unique = []
    seen = set()
    for path in paths:
        if not path or path in seen:
            continue
        if os.path.isdir(path):
            unique.append(path)
            seen.add(path)
    return unique


def _prepend_unique_env_paths(env, key, paths, sep):
    current = [p for p in env.get(key, "").split(sep) if p]
    merged = [p for p in (paths or []) if p and p not in current] + current
    if merged:
        env[key] = sep.join(merged)
    return merged


def candidate_install_python_roots(install_dir):
    """Get candidate Python package roots from an install directory."""
    roots = [install_dir]
    patterns = (
        os.path.join(install_dir, "lib", "python*", "site-packages"),
        os.path.join(install_dir, "lib", "python*", "dist-packages"),
        os.path.join(install_dir, "lib64", "python*", "site-packages"),
        os.path.join(install_dir, "Lib", "site-packages"),
    )
    for pattern in patterns:
        roots.extend(sorted(glob(pattern)))
    return _unique_existing_paths(roots)


def has_installed_fife(install_dir):
    """Check if FIFE is installed in the given directory."""
    for root in candidate_install_python_roots(install_dir):
        pkg_dir = os.path.join(root, "fife")
        if os.path.exists(os.path.join(pkg_dir, "__init__.py")) and glob(
            os.path.join(pkg_dir, "_fife_swig*")
        ):
            return True
    return False


def resolve_install_dir():
    """Resolve the install directory containing FIFE."""
    env_install_dir = os.environ.get("FIFE_INSTALL_DIR")
    if env_install_dir:
        return env_install_dir

    out_install_dir = genpath("out/install")
    if not os.path.isdir(out_install_dir):
        return None

    if has_installed_fife(out_install_dir):
        return out_install_dir

    install_candidates = [
        p for p in sorted(glob(os.path.join(out_install_dir, "*"))) if os.path.isdir(p)
    ]
    matching = [p for p in install_candidates if has_installed_fife(p)]
    if len(matching) == 1:
        return matching[0]
    if matching:
        return max(matching, key=os.path.getmtime)
    return None


def prepare_install_bindings(install_dir):
    """Prepare Python bindings from an install directory."""
    python_roots = []
    for root in candidate_install_python_roots(install_dir):
        if os.path.exists(os.path.join(root, "fife", "__init__.py")):
            python_roots.append(root)

    if not python_roots:
        return False

    for root in reversed(python_roots):
        if root not in sys.path:
            sys.path.insert(0, root)

    lib_candidates = [
        install_dir,
        os.path.join(install_dir, "lib"),
        os.path.join(install_dir, "lib64"),
        os.path.join(install_dir, "bin"),
    ]
    for root in python_roots:
        lib_candidates.append(os.path.join(root, "fife"))

    library_paths = _unique_existing_paths(lib_candidates)

    _prepend_unique_env_paths(os.environ, "LD_LIBRARY_PATH", library_paths, ":")

    if os.name == "nt":
        _prepend_unique_env_paths(os.environ, "PATH", library_paths, os.pathsep)

    return {"pythonpath": python_roots, "library_paths": library_paths}


def resolve_test_progs(build_dir):
    """Get core C++ test names from CTest registrations in a configured build directory."""
    ctest_cmd = ["ctest", "--test-dir", build_dir, "-N", "-L", "core"]
    try:
        result = subprocess.run(
            ctest_cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            check=False,
        )
    except OSError:
        return []

    reprg = re.compile(r"""^\s*Test\s+#\d+:\s+(.*?)\s*$""")
    progs = []
    for line in result.stdout.splitlines():
        m = reprg.match(line)
        if m:
            progs.append(m.group(1))
    return progs


def resolve_test_modules(directory):
    """Resolve test modules from a directory."""
    pythonfilenames = [p for p in os.listdir(directory) if len(p) > 3 and p[-3:] == ".py"]
    modname = directory.replace(os.path.sep, ".") + "."
    modules = []
    skipped_filenames = ("test_all.py",)
    for p in pythonfilenames:
        skip = False
        for s in skipped_filenames:
            if p.find(s) != -1:
                skip = True
        if p.endswith("_utils.py"):
            skip = True
        if p[0] == "_":
            skip = True
        if not skip:
            modules.append(modname + p[:-3])
    return modules


def prepare_python_bindings(build_dir):
    """Prepare a local Python package layout for SWIG tests.

    This mirrors src/python/fife into build/fife and overlays generated
    SWIG stubs (fife.py, fifechan.py), so imports like `from fife import fife`
    work without installing the package system-wide.
    """
    project_root = os.path.dirname(os.path.abspath(__file__))
    src_pkg_dir = os.path.join(project_root, "src", "python", "fife")
    build_pkg_dir = os.path.join(build_dir, "fife")

    os.makedirs(build_pkg_dir, exist_ok=True)
    if os.path.isdir(src_pkg_dir):
        shutil.copytree(src_pkg_dir, build_pkg_dir, dirs_exist_ok=True)

    # Copy generated SWIG modules into the package dir if present.
    # On Windows the extension filename may be e.g. "_fife_swig.pyd" or
    # contain ABI tags. Search for any file that starts with the expected
    # prefix (e.g. "_fife_swig"). Only copy when we find both the Python
    # stub and a matching compiled extension file.
    generated_modules = {
        "fife.py": "_fife_swig",
        "fifechan.py": "_fifechan_swig",
    }
    for module_name, ext_prefix in generated_modules.items():
        src_module = os.path.join(build_dir, module_name)
        if not os.path.exists(src_module):
            continue

        # Find a compiled extension file that begins with the expected prefix.
        found_ext = None
        try:
            for fname in os.listdir(build_dir):
                if fname.startswith(ext_prefix):
                    candidate = os.path.join(build_dir, fname)
                    if os.path.isfile(candidate):
                        found_ext = candidate
                        break
        except OSError:
            found_ext = None

        if found_ext:
            # copy the Python stub and the compiled extension into the
            # package directory so `from fife import fife` works.
            shutil.copy2(src_module, os.path.join(build_pkg_dir, module_name))
            dst_ext = os.path.join(build_pkg_dir, os.path.basename(found_ext))
            shutil.copy2(found_ext, dst_ext)

            # On Windows also copy DLL dependencies into the package folder
            # so the extension can find them when imported from the package.
            if os.name == "nt":
                # copy DLLs from the build root
                try:
                    for fname in os.listdir(build_dir):
                        if fname.lower().endswith(".dll"):
                            srcdll = os.path.join(build_dir, fname)
                            dstdll = os.path.join(build_pkg_dir, fname)
                            if os.path.isfile(srcdll) and not os.path.exists(dstdll):
                                shutil.copy2(srcdll, dstdll)
                except OSError:
                    pass

                # copy DLLs from dependencies install bin (vcpkg/deps)
                deps_bin = os.path.join(
                    project_root, "out", "fife-dependencies", "install", "bin"
                )
                try:
                    if os.path.isdir(deps_bin):
                        for fname in os.listdir(deps_bin):
                            if fname.lower().endswith(".dll"):
                                srcdll = os.path.join(deps_bin, fname)
                                dstdll = os.path.join(build_pkg_dir, fname)
                                if os.path.isfile(srcdll) and not os.path.exists(dstdll):
                                    shutil.copy2(srcdll, dstdll)
                except OSError:
                    pass

            # Try to preload the compiled extension as a submodule (e.g.
            # 'fife._fife_swig') to avoid circular import problems when
            # the package __init__ imports the Python stub which in turn
            # imports the extension. Loading the extension directly into
            # sys.modules under the package submodule name ensures the
            # import in the stub finds the compiled module.
            try:
                import importlib.util

                pkg_name = os.path.basename(build_pkg_dir)
                submod_name = (
                    pkg_name + "." + os.path.splitext(os.path.basename(dst_ext))[0]
                )
                if submod_name not in sys.modules:
                    spec = importlib.util.spec_from_file_location(submod_name, dst_ext)
                    if spec and spec.loader:
                        module = importlib.util.module_from_spec(spec)
                        sys.modules[submod_name] = module
                        spec.loader.exec_module(module)
            except Exception:
                # Non-fatal: if preloading fails we'll rely on normal import
                # behavior when tests are run.
                pass

    if build_dir not in sys.path:
        sys.path.insert(0, build_dir)

    ld_path = os.environ.get("LD_LIBRARY_PATH", "")
    ld_parts = [p for p in ld_path.split(":") if p]
    if build_dir not in ld_parts:
        os.environ["LD_LIBRARY_PATH"] = build_dir + (":" + ld_path if ld_path else "")
    # On Windows the DLL search path is controlled via PATH
    if os.name == "nt":
        path_env = os.environ.get("PATH", "")
        path_parts = [p for p in path_env.split(os.pathsep) if p]
        if build_dir not in path_parts:
            os.environ["PATH"] = build_dir + (os.pathsep + path_env if path_env else "")
        # Also add known dependency bins (vcpkg / dependencies install) to PATH
        project_root = os.path.dirname(os.path.abspath(__file__))
        dep_bins = [
            os.path.join(project_root, "out", "fife-dependencies", "install", "bin"),
            os.path.join(
                project_root, "out", "build", "vcpkg_installed", "x64-windows", "bin"
            ),
            os.path.join(
                project_root,
                "out",
                "build",
                "vcpkg_installed",
                "x64-windows",
                "debug",
                "bin",
            ),
            os.path.join(project_root, "vcpkg_installed", "x64-windows", "bin"),
            "f:\\tools\\vcpkg\\installed\\x64-windows\\bin",
            "f:\\tools\\vcpkg\\installed\\x64-windows\\debug\\bin",
        ]
        for d in dep_bins:
            if os.path.isdir(d):
                current = os.environ.get("PATH", "")
                parts = [p for p in current.split(os.pathsep) if p]
                if d not in parts:
                    os.environ["PATH"] = d + os.pathsep + os.environ.get("PATH", "")


def run_core_tests(progs):
    """Run core C++ tests."""
    build_dir = resolve_build_dir()
    if not os.path.isdir(build_dir):
        return [f"Build directory not found: {build_dir}"], []

    errors, failures = [], []
    for prog in progs:
        print(f"\n===== Running {prog} =====")
        build_cmd = ["cmake", "--build", build_dir, "--target", prog]
        if subprocess.call(build_cmd):
            errors.append(prog)
            continue
        cmd = [
            "ctest",
            "--test-dir",
            build_dir,
            "--output-on-failure",
            "-R",
            f"^{prog}$",
        ]
        if subprocess.call(cmd):
            errors.append(prog)
    return errors, failures


def _parse_timeout(value, fallback, key_name):
    try:
        parsed = int(value)
        if parsed <= 0:
            raise ValueError
        return parsed
    except (TypeError, ValueError):
        print(
            f"Warning: invalid timeout for {key_name}={value!r}. Using {fallback} seconds."
        )
        return fallback


def _build_test_subprocess_env(
    build_dir, extra_pythonpath, extra_library_path, headless=False
):
    env = os.environ.copy()
    pythonpath = env.get("PYTHONPATH", "")
    python_parts = []
    for part in (extra_pythonpath or []) + [build_dir, os.getcwd(), pythonpath]:
        if part and part not in python_parts:
            python_parts.append(part)
    env["PYTHONPATH"] = os.pathsep.join(python_parts)

    if extra_library_path:
        _prepend_unique_env_paths(env, "LD_LIBRARY_PATH", extra_library_path, ":")

        if os.name == "nt":
            _prepend_unique_env_paths(env, "PATH", extra_library_path, os.pathsep)

    if headless:
        env.setdefault("SDL_VIDEODRIVER", "dummy")
        env.setdefault("SDL_AUDIODRIVER", "dummy")
    return env


def run_test_modules(
    modules, build_dir, extra_pythonpath=None, extra_library_path=None, headless=False
):
    """Run Python test modules."""
    env = _build_test_subprocess_env(
        build_dir,
        extra_pythonpath=extra_pythonpath,
        extra_library_path=extra_library_path,
        headless=headless,
    )

    def resolve_module_timeout(module_name):
        normalized_name = re.sub(r"[^A-Za-z0-9]", "_", module_name).upper()

        env_specific = env.get("FIFE_MODULE_TEST_TIMEOUT_" + normalized_name)
        if env_specific:
            return _parse_timeout(
                env_specific,
                DEFAULT_MODULE_TIMEOUT,
                "FIFE_MODULE_TEST_TIMEOUT_" + normalized_name,
            )

        if module_name == "tests.swig_tests.animation_tests":
            return _parse_timeout(
                env.get(
                    "FIFE_MODULE_TEST_TIMEOUT_TESTS_SWIG_TESTS_ANIMATION_TESTS",
                    DEFAULT_ANIMATION_MODULE_TIMEOUT,
                ),
                DEFAULT_ANIMATION_MODULE_TIMEOUT,
                "FIFE_MODULE_TEST_TIMEOUT_TESTS_SWIG_TESTS_ANIMATION_TESTS",
            )

        return _parse_timeout(
            env.get("FIFE_MODULE_TEST_TIMEOUT", str(DEFAULT_MODULE_TIMEOUT)),
            DEFAULT_MODULE_TIMEOUT,
            "FIFE_MODULE_TEST_TIMEOUT",
        )

    errors = []
    failures = []
    for module in modules:
        module_timeout = resolve_module_timeout(module)
        print(f"\n===== Running {module} =====")
        cmd = [sys.executable, "-m", "unittest", module]
        try:
            proc = subprocess.run(
                cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                env=env,
                timeout=module_timeout,
            )
            print(proc.stdout)
            if proc.returncode != 0:
                errors.append(proc.stdout)
        except subprocess.TimeoutExpired as exc:
            timeout_output = (
                exc.stdout or ""
            ) + f"\n[timeout] module {module} exceeded {module_timeout}s\n"
            print(timeout_output)
            errors.append(timeout_output)
    return errors, failures


def run_all(
    tests, build_dir, extra_pythonpath=None, extra_library_path=None, headless=False
):
    """Run all tests."""

    def print_errors(txt, errs):
        if errs:
            print(txt + ":")
            for msg in errs:
                print("  " + msg)

    core_errors, core_failures = run_core_tests(tests["core"])
    swig_errors, swig_failures = run_test_modules(
        tests["swig"],
        build_dir,
        extra_pythonpath=extra_pythonpath,
        extra_library_path=extra_library_path,
        headless=headless,
    )
    ext_errors, ext_failures = run_test_modules(
        tests["ext"],
        build_dir,
        extra_pythonpath=extra_pythonpath,
        extra_library_path=extra_library_path,
        headless=headless,
    )

    print(80 * "=")
    errorsfound = False

    if core_errors or core_failures:
        print_errors("Errors in core tests", core_errors)
        print_errors("Failures in core tests", core_failures)
        errorsfound = True
    else:
        print("No Core errors found")

    if swig_errors or swig_failures:
        print_errors("Errors in SWIG tests", swig_errors)
        print_errors("Failures in SWIG tests", swig_failures)
        errorsfound = True
    else:
        print("No SWIG errors found")

    if ext_errors or ext_failures:
        print_errors("Errors in extensions tests", ext_errors)
        print_errors("Failures in extensions tests", ext_failures)
        errorsfound = True
    else:
        print("No Extensions errors found")

    print(80 * "=")
    if errorsfound:
        print("ERROR. One or more tests failed!")
    else:
        print("OK. All tests ran succesfully!")
    print("")
    return not errorsfound


def quit(dummy):
    """Quit the test runner."""
    sys.exit(0)


def _run_case_and_report(fn, params):
    result = fn(params)
    if isinstance(result, tuple) and len(result) == 2:
        errors, failures = result
        return not (errors or failures)
    return bool(result)


def _bind_runner(fn, build_dir, headless_mode, extras):
    return lambda params: fn(
        params,
        build_dir,
        extra_pythonpath=extras["pythonpath"],
        extra_library_path=extras["library_paths"],
        headless=headless_mode,
    )


def _append_test_group(tests, index, header, test_names, runner):
    for test_name in test_names:
        tests[index] = (header, test_name, [test_name], runner)
        index += 1
    tests[index] = (header, "all", test_names, runner)
    return index + 1


def _build_test_menu(
    core_tests, swig_tests, extension_tests, build_dir, headless_mode, extras
):
    index = 0
    tests = {}
    module_runner = _bind_runner(run_test_modules, build_dir, headless_mode, extras)
    all_runner = _bind_runner(run_all, build_dir, headless_mode, extras)

    index = _append_test_group(tests, index, "Core tests", core_tests, run_core_tests)
    index = _append_test_group(tests, index, "SWIG tests", swig_tests, module_runner)
    index = _append_test_group(
        tests, index, "Extension tests", extension_tests, module_runner
    )

    alltests = {"core": core_tests, "swig": swig_tests, "ext": extension_tests}
    tests[index] = ("Other", "Run all tests", alltests, all_runner)
    tests[index + 1] = ("Other", "Cancel and quit", None, quit)
    return tests, alltests


def run(automatic, selected_cases, headless=None):
    """Run the test suite."""
    headless_mode = resolve_headless_mode(headless)
    print("SWIG/extension test mode: %s" % ("headless" if headless_mode else "windowed"))

    build_dir = resolve_build_dir()
    install_dir = resolve_install_dir()
    extras = {"pythonpath": [build_dir], "library_paths": [build_dir]}
    if install_dir:
        install_extras = prepare_install_bindings(install_dir)
        if install_extras:
            extras = install_extras
            print(f"Using installed FIFE python package from: {install_dir}")
        else:
            prepare_python_bindings(build_dir)
            print(f"Using build-directory FIFE python package from: {build_dir}")
    else:
        prepare_python_bindings(build_dir)
        print(f"Using build-directory FIFE python package from: {build_dir}")

    core_tests = resolve_test_progs(build_dir)
    swig_tests = resolve_test_modules(genpath("tests/swig_tests"))
    extension_tests = resolve_test_modules(genpath("tests/extension_tests"))
    tests, alltests = _build_test_menu(
        core_tests,
        swig_tests,
        extension_tests,
        build_dir,
        headless_mode,
        extras,
    )

    if (not automatic) and (not selected_cases):
        selection = None
        while True:
            print("Select test target to run:")
            prevheader = ""
            for ind in sorted(tests.keys()):
                header, name, params, fn = tests[ind]
                if header != prevheader:
                    print(header)
                    prevheader = header
                print(f"  {ind}) {name}")
            selection = input("-> : ")

            try:
                selection = int(selection)
                if (selection < 0) or (selection > max(tests.keys())):
                    raise ValueError
                break
            except ValueError:
                print(f"Please enter number between 0-{max(tests.keys())}\n")
                continue
        header, name, params, fn = tests[selection]
        return _run_case_and_report(fn, params)
    elif selected_cases:
        success = True
        for case in selected_cases:
            try:
                caseid = int(case)
                if (caseid < 0) or (caseid > max(tests.keys())):
                    raise ValueError
                header, name, params, fn = tests[caseid]
                success = _run_case_and_report(fn, params) and success
            except ValueError:
                print(f"No test case with value {case} found")
                success = False
        return success
    else:
        return run_all(
            alltests,
            build_dir,
            extra_pythonpath=extras["pythonpath"],
            extra_library_path=extras["library_paths"],
            headless=headless_mode,
        )


def main():
    """Run the test runner."""
    usage = (
        "usage: %prog [options] [args]\n"
        + "This is a test runner.\n"
        + "It enables you to test functionalities of fifengine core, extensions and the generated swig interface.\n"
        + "You can give a list of test ids as arguments to the script.\n"
        + "This is useful when running a test over and over again with little changes.\n"
        + "Available test ids can be seen from interactive menu (run script without any parameters).\n"
        + 'You can also use "-a" to run all tests from the CLI.'
    )
    parser = optparse.OptionParser(usage)
    parser.add_option(
        "-a",
        "--automatic",
        action="store_true",
        dest="automatic",
        default=False,
        help="In case selected, runs all the tests automatically",
    )
    parser.add_option(
        "--headless",
        action="store_true",
        dest="headless",
        default=None,
        help="Run SWIG/extension tests in headless mode (dummy SDL drivers)",
    )
    parser.add_option(
        "--windowed",
        action="store_false",
        dest="headless",
        help="Run SWIG/extension tests with normal windowing (overrides CI default)",
    )
    options, args = parser.parse_args()
    return 0 if run(options.automatic, args, options.headless) else 1


if __name__ == "__main__":
    sys.exit(main())
