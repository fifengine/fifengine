# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestXmlmapsaver:
    def test_module_docstring(self):
        from fife.extensions.serializers import xmlmapsaver

        assert "map" in xmlmapsaver.__doc__.lower()

    def test_mapformat_defined(self):
        from fife.extensions.serializers import xmlmapsaver

        assert xmlmapsaver.MAPFORMAT == "1.0"

    def test_file_extensions_defined(self):
        from fife.extensions.serializers import xmlmapsaver

        assert "xml" in xmlmapsaver.fileExtensions

    def test_xmlmapsaver_class_exists(self):
        from fife.extensions.serializers.xmlmapsaver import XMLMapSaver

        assert XMLMapSaver is not None
