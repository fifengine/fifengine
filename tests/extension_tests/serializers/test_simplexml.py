# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


import pytest
from fife.extensions.serializers import SerializerError
from fife.extensions.serializers.simplexml import SimpleXMLSerializer


class TestSimpleXMLSerializer:
    def test_simple_serializer_base_class(self):
        from fife.extensions.serializers.simplexml import SimpleSerializer

        s = SimpleSerializer()
        assert s.get("test", "key") is None
        assert s.set("test", "key", "value") is None
        assert s.load() is None
        assert s.save() is None
        assert s.getModuleNameList() is None
        assert s.getAllSettings("test") is None

    def test_simplexml_serializer_init(self):
        s = SimpleXMLSerializer()
        assert s._file is None
        assert s._tree is None
        assert s._initialized is False

    def test_simplexml_serializer_with_filename(self, tmp_path):
        xml_file = tmp_path / "test.xml"
        s = SimpleXMLSerializer(str(xml_file))
        assert s._file == str(xml_file)

    def test_get_value_int(self):
        s = SimpleXMLSerializer()
        assert s.getValue("int", "42") == 42
        assert s.getValue("int", "-10") == -10

    def test_get_value_float(self):
        s = SimpleXMLSerializer()
        assert s.getValue("float", "3.14") == 3.14
        assert s.getValue("float", "-0.5") == -0.5

    def test_get_value_bool(self):
        s = SimpleXMLSerializer()
        assert s.getValue("bool", "true") is True
        assert s.getValue("bool", "false") is False
        assert s.getValue("bool", "yes") is True
        assert s.getValue("bool", "no") is False
        assert s.getValue("bool", "1") is True
        assert s.getValue("bool", "0") is False

    def test_get_value_string(self):
        s = SimpleXMLSerializer()
        assert s.getValue("str", "hello") == "hello"
        assert s.getValue("string", "world") == "world"
        assert s.getValue("unicode", "test") == "test"

    def test_get_value_unknown_returns_unchanged(self):
        s = SimpleXMLSerializer()
        assert s.getValue("unknown", "some_value") == "some_value"

    def test_load_without_filename_raises(self):
        s = SimpleXMLSerializer()
        with pytest.raises(SerializerError):
            s.load()

    def test_save_without_filename_raises(self, tmp_path):
        xml_file = tmp_path / "test.xml"
        s = SimpleXMLSerializer(str(xml_file))
        s.load()
        s._file = None
        with pytest.raises(SerializerError):
            s.save()

    def test_load_creates_file_if_missing(self, tmp_path):
        xml_file = tmp_path / "test.xml"
        s = SimpleXMLSerializer(str(xml_file))
        s.load()
        assert xml_file.exists()

    def test_get_with_invalid_name_type_raises(self, tmp_path):
        xml_file = tmp_path / "test.xml"
        xml_file.write_text("<?xml version='1.0'?><Settings></Settings>")
        s = SimpleXMLSerializer(str(xml_file))
        s.load()
        with pytest.raises(AttributeError):
            s.get("FIFE", 123)

    def test_get_returns_default_when_not_found(self, tmp_path):
        xml_file = tmp_path / "test.xml"
        xml_file.write_text("<?xml version='1.0'?><Settings></Settings>")
        s = SimpleXMLSerializer(str(xml_file))
        s.load()
        result = s.get("FIFE", "NonExistentKey", "default_value")
        assert result == "default_value"
