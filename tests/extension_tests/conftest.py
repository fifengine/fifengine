# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


import importlib.util

import pytest


def pytest_collection_modifyitems(config, items):
    fife_spec = importlib.util.find_spec("fife")
    if fife_spec is None:
        skip_marker = pytest.mark.skip(reason="fife module not available")
        for item in items:
            if "extension_tests" in str(item.fspath):
                item.add_marker(skip_marker)


@pytest.fixture
def temp_settings_dir(tmp_path):
    settings_dir = tmp_path / ".testapp" / "testapp"
    settings_dir.mkdir(parents=True)
    return settings_dir


@pytest.fixture
def sample_xml_string():
    return """<?xml version="1.0" encoding="UTF-8"?>
<root>
    <section key="value">
        <item name="test">Content</item>
    </section>
</root>"""
