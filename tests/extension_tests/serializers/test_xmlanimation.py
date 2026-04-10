# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


class TestXmlanimation:
    def test_load_xml_animation_exists(self):
        from fife.extensions.serializers import xmlanimation

        assert xmlanimation.loadXMLAnimation is not None

    def test_module_docstring(self):
        from fife.extensions.serializers import xmlanimation

        assert "animation" in xmlanimation.__doc__.lower()
