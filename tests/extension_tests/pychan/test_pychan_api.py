# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestPychanLoadXML:
    def test_load_xml_exists(self):
        from fife.extensions.pychan import loadXML

        assert loadXML is not None


class TestPychanInit:
    def test_init_exists(self):
        from fife.extensions.pychan import init

        assert init is not None


class TestPychanSetupModalExecution:
    def test_setup_modal_execution_exists(self):
        from fife.extensions.pychan import setupModalExecution

        assert setupModalExecution is not None
