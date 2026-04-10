# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestXmlobject:
    def test_xmlobjectsaver_class_exists(self):
        from fife.extensions.serializers.xmlobject import XMLObjectSaver

        assert XMLObjectSaver is not None

    def test_xmlobjectloader_class_exists(self):
        from fife.extensions.serializers.xmlobject import XMLObjectLoader

        assert XMLObjectLoader is not None

    def test_processing_instruction_defined(self):
        from fife.extensions.serializers.xmlobject import XMLObjectSaver

        assert hasattr(XMLObjectSaver, "PROCESSING_INSTRUCTION")
        assert "fife" in XMLObjectSaver.PROCESSING_INSTRUCTION
