# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestXmlmapLoader:
    def test_class_exists(self):
        from fife.extensions.serializers.xmlmap import XMLMapLoader

        assert XMLMapLoader is not None

    def test_format_defined(self):
        from fife.extensions.serializers.xmlmap import FORMAT

        assert FORMAT == "1.0"


class TestXmlmapsaverClass:
    def test_class_exists(self):
        from fife.extensions.serializers.xmlmapsaver import XMLMapSaver

        assert XMLMapSaver is not None

    def test_file_extensions_defined(self):
        from fife.extensions.serializers.xmlmapsaver import fileExtensions

        assert "xml" in fileExtensions


class TestXmlobjectLoader:
    def test_class_exists(self):
        from fife.extensions.serializers.xmlobject import XMLObjectLoader

        assert XMLObjectLoader is not None

    def test_class_exists2(self):
        from fife.extensions.serializers.xmlobject import XMLObjectSaver

        assert XMLObjectSaver is not None


class TestXmlanimationLoader:
    def test_function_exists(self):
        from fife.extensions.serializers.xmlanimation import loadXMLAnimation

        assert loadXMLAnimation is not None
