#!/usr/bin/env python

# ####################################################################
#  Copyright (C) 2005-2019 by the FIFE team
#  http://www.fifengine.net
#  This file is part of FIFE.
#
#  FIFE is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the
#  Free Software Foundation, Inc.,
#  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
# ####################################################################

from __future__ import print_function

import optparse
import os
import re
import shutil
import subprocess
import sys
from builtins import input


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
    return os.path.sep.join(somepath.split("/"))


def print_header(text):
    print("\n")
    print(80 * "=")
    print(text)
    print(80 * "-")


def resolve_test_progs(build_dir):
    """Get core C++ test names from CTest registrations in a configured build directory."""
    ctest_cmd = ["ctest", "--test-dir", build_dir, "-N", "-L", "core"]
    try:
        result = subprocess.run(
            ctest_cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            universal_newlines=True,
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
    pythonfilenames = [p for p in os.listdir(directory) if len(p) > 3 and p[-3:] == ".py"]
    modname = directory.replace(os.path.sep, ".") + "."
    modules = []
    skipped_filenames = ("test_all.py",)
    for p in pythonfilenames:
        skip = False
        for s in skipped_filenames:
            if p.find(s) != -1:
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

    for module_name in ("fife.py", "fifechan.py"):
        src_module = os.path.join(build_dir, module_name)
        if os.path.exists(src_module):
            shutil.copy2(src_module, os.path.join(build_pkg_dir, module_name))

    for extension_name in ("_fife_swig.so", "_fifechan_swig.so"):
        src_extension = os.path.join(build_dir, extension_name)
        if os.path.exists(src_extension):
            shutil.copy2(src_extension, os.path.join(build_pkg_dir, extension_name))

    if build_dir not in sys.path:
        sys.path.insert(0, build_dir)

    ld_path = os.environ.get("LD_LIBRARY_PATH", "")
    ld_parts = [p for p in ld_path.split(":") if p]
    if build_dir not in ld_parts:
        os.environ["LD_LIBRARY_PATH"] = build_dir + (":" + ld_path if ld_path else "")


def run_core_tests(progs):
    build_dir = os.environ.get("FIFE_BUILD_DIR", genpath("build"))
    if not os.path.isdir(build_dir):
        return ["Build directory not found: %s" % build_dir], []

    errors, failures = [], []
    for prog in progs:
        print("\n===== Running %s =====" % prog)
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
            "^%s$" % prog,
        ]
        if subprocess.call(cmd):
            errors.append(prog)
    return errors, failures


def get_dynamic_imports(modules):
    imported = []
    for module in modules:
        m = __import__(module)
        for part in module.split(".")[1:]:
            m = getattr(m, part)
        imported.append(m)
    return imported


def run_test_modules(modules, headless=False):
    build_dir = os.environ.get("FIFE_BUILD_DIR", genpath("build"))
    env = os.environ.copy()
    pythonpath = env.get("PYTHONPATH", "")
    python_parts = [p for p in [build_dir, os.getcwd(), pythonpath] if p]
    env["PYTHONPATH"] = os.pathsep.join(python_parts)
    if headless:
        env.setdefault("SDL_VIDEODRIVER", "dummy")
        env.setdefault("SDL_AUDIODRIVER", "dummy")

    def resolve_module_timeout(module_name):
        normalized_name = re.sub(r"[^A-Za-z0-9]", "_", module_name).upper()

        env_specific = env.get("FIFE_MODULE_TEST_TIMEOUT_" + normalized_name)
        if env_specific:
            return int(env_specific)

        animation_default_timeout = 90
        if module_name == "tests.swig_tests.animation_tests":
            return int(
                env.get(
                    "FIFE_MODULE_TEST_TIMEOUT_TESTS_SWIG_TESTS_ANIMATION_TESTS",
                    animation_default_timeout,
                )
            )

        return int(env.get("FIFE_MODULE_TEST_TIMEOUT", "120"))

    errors = []
    failures = []
    for module in modules:
        module_timeout = resolve_module_timeout(module)
        print("\n===== Running %s =====" % module)
        cmd = [sys.executable, "-m", "unittest", module]
        try:
            proc = subprocess.run(
                cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                universal_newlines=True,
                env=env,
                timeout=module_timeout,
            )
            print(proc.stdout)
            if proc.returncode != 0:
                errors.append(proc.stdout)
        except subprocess.TimeoutExpired as exc:
            timeout_output = (
                exc.stdout or ""
            ) + "\n[timeout] module %s exceeded %ss\n" % (module, module_timeout)
            print(timeout_output)
            errors.append(timeout_output)
    return errors, failures


def run_all(tests, headless=False):
    def print_errors(txt, errs):
        if errs:
            print(txt + ":")
            for msg in errs:
                print("  " + msg)

    core_errors, core_failures = run_core_tests(tests["core"])
    swig_errors, swig_failures = run_test_modules(tests["swig"], headless=headless)
    ext_errors, ext_failures = run_test_modules(tests["ext"], headless=headless)

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


def quit(dummy):
    sys.exit(0)


def run(automatic, selected_cases, headless=None):
    index = 0
    tests = {}
    headless_mode = resolve_headless_mode(headless)
    print("SWIG/extension test mode: %s" % ("headless" if headless_mode else "windowed"))

    build_dir = os.environ.get("FIFE_BUILD_DIR", genpath("build"))
    prepare_python_bindings(build_dir)
    core_tests = resolve_test_progs(build_dir)
    for t in core_tests:
        tests[index] = ("Core tests", t, [t], run_core_tests)
        index += 1
    tests[index] = ("Core tests", "all", core_tests, run_core_tests)
    index += 1

    swig_tests = resolve_test_modules(genpath("tests/swig_tests"))
    for t in swig_tests:
        tests[index] = (
            "SWIG tests",
            t,
            [t],
            lambda modules, _headless=headless_mode: run_test_modules(
                modules, headless=_headless
            ),
        )
        index += 1
    tests[index] = (
        "SWIG tests",
        "all",
        swig_tests,
        lambda modules, _headless=headless_mode: run_test_modules(
            modules, headless=_headless
        ),
    )
    index += 1

    extension_tests = resolve_test_modules(genpath("tests/extension_tests"))
    for t in extension_tests:
        tests[index] = (
            "Extension tests",
            t,
            [t],
            lambda modules, _headless=headless_mode: run_test_modules(
                modules, headless=_headless
            ),
        )
        index += 1
    tests[index] = (
        "Extension tests",
        "all",
        extension_tests,
        lambda modules, _headless=headless_mode: run_test_modules(
            modules, headless=_headless
        ),
    )
    index += 1

    alltests = {"core": core_tests, "swig": swig_tests, "ext": extension_tests}
    tests[index] = (
        "Other",
        "Run all tests",
        alltests,
        lambda modules, _headless=headless_mode: run_all(modules, headless=_headless),
    )
    tests[index + 1] = ("Other", "Cancel and quit", None, quit)

    if (not automatic) and (not selected_cases):
        selection = None
        while True:
            print("Select test module to run:")
            prevheader = ""
            for ind in sorted(tests.keys()):
                header, name, params, fn = tests[ind]
                if header != prevheader:
                    print(header)
                    prevheader = header
                print("  %d) %s" % (ind, name))
            selection = input("-> : ")

            try:
                selection = int(selection)
                if (selection < 0) or (selection > max(tests.keys())):
                    raise ValueError
                break
            except ValueError:
                print("Please enter number between 0-%d\n" % max(tests.keys()))
                continue
        header, name, params, fn = tests[selection]
        fn(params)
    elif selected_cases:
        for case in selected_cases:
            try:
                caseid = int(case)
                if (caseid < 0) or (caseid > max(tests.keys())):
                    raise ValueError
                header, name, params, fn = tests[caseid]
                fn(params)
            except ValueError:
                print("No test case with value %s found" % case)
    else:
        run_all(alltests)


def main():
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
    run(options.automatic, args, options.headless)


if __name__ == "__main__":
    main()
