"""Sphinx configuration for the FIFE project documentation."""

import sys

project = "FIFE"
copyright = "2005-2025, FIFE Team"
author = "FIFE contributors"
release = "0.5.0"

extensions = [
    "sphinx.ext.autodoc",  # pull docstrings from code
    "sphinx.ext.viewcode",  # add source code links
    "sphinx.ext.napoleon",  # parse NumPy-style docstrings
    "sphinx_autodoc_typehints",  # integrate type hints into docs
    "myst_parser",  # enable Markdown support
]

templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

html_theme = "furo"
html_static_path = ["_static"]

html_theme_options = {
    "source_repository": "https://github.com/fifengine/fifengine",
    "source_branch": "master",
    "source_directory": "docs/sphinx/",
}

html_context = {
    "github_user": "fifengine",
    "github_repo": "fifengine",
    "github_version": "master",
    "conf_py_path": "/docs/",
}

pygments_style = "sphinx"
pygments_dark_style = "monokai"

sys.path.insert(0, "/workspaces/fifengine/src/python")

# Options for autodoc

autodoc_default_options = {
    "members": True,
    "undoc-members": True,
    "show-inheritance": True,
}

autodoc_mock_imports = [
    "_fife_swig",
    "_fifechan_swig",
    "fife._fife_swig",
    "fife._fifechan_swig",
]

autodoc_typehints = "description"

# Options for Napoleon

napoleon_numpy_docstring = True
napoleon_google_docstring = False
