# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestXmlmap:
    def test_module_docstring(self):
        from fife.extensions.serializers import xmlmap

        assert "map" in xmlmap.__doc__.lower()

    def test_format_defined(self):
        from fife.extensions.serializers import xmlmap

        assert xmlmap.FORMAT == "1.0"

    def test_xmlmaploader_class_exists(self):
        from fife.extensions.serializers.xmlmap import XMLMapLoader

        assert XMLMapLoader is not None
