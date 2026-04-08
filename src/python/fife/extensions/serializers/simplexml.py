# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Simple XML serializer for settings."""

import os
from io import BytesIO

from fife.extensions.serializers import ET, InvalidFormat, SerializerError

EMPTY_XML_FILE = """\
<?xml version='1.0' encoding='UTF-8'?>
<Settings>

</Settings>
"""


class SimpleSerializer:
    """
    Base class for custom setting loaders/savers.

    Use this as a base class for custom setting loaders/savers to use with the
    Setting class.
    """

    def __init__(self, filename=None):
        """Initialize the serializer."""
        pass

    def get(self, module, name, defaultValue=None):
        """Get a setting value."""
        pass

    def set(self, module, name, value, extra_attrs=None):
        """Set a setting value."""
        pass

    def load(self, filename=None):
        """Load settings from ``filename``.

        Notes
        -----
        If the filename specified is empty this function must initialize an
        empty settings file in the desired format.
        """
        pass

    def save(self, filename=None):
        """Save settings to file."""
        pass

    def getModuleNameList(self):
        """Return a list of module names present in the settings file.

        Returns
        -------
        list[str]
            Module names found in the settings XML file.
        """
        pass

    def getAllSettings(self, module):
        """Return all setting names and values under a module as a dict.

        Parameters
        ----------
        module : str
            Module name to retrieve settings for.

        Returns
        -------
        dict
            Mapping of setting names to values.
        """
        pass


class SimpleXMLSerializer(SimpleSerializer):
    """Provide a simple interface to get and store data in XML files."""

    def __init__(self, filename=None):
        """Initialize the serializer."""
        self._file = filename
        self._tree = None
        self._root_element = None
        self._initialized = False

    def load(self, filename=None):
        """Load settings from a file."""
        if filename:
            self._file = filename

        if not self._file:
            raise SerializerError(
                "Cannot load file or create file. No filename specified!"
            )

        if not os.path.exists(self._file):
            # In Python 3, ET.parse works well with BytesIO for encoded XML strings
            self._tree = ET.parse(BytesIO(EMPTY_XML_FILE.encode("utf-8")))
            self._tree.write(self._file, encoding="UTF-8", xml_declaration=True)
        else:
            self._tree = ET.parse(self._file)

        self._root_element = self._tree.getroot()
        self._validateTree()
        self._initialized = True

    def save(self, filename=None):
        """Save settings to a file."""
        if not self._initialized:
            self.load()

        savefile = filename if filename else self._file

        if not savefile:
            raise SerializerError("Cannot save file. No filename specified!")

        self._indent(self._root_element)
        self._tree.write(savefile, encoding="UTF-8", xml_declaration=True)

    def getValue(self, e_type, e_value):
        """Convert a value from string to the appropriate type."""
        if e_type == "int":
            return int(e_value)
        elif e_type == "float":
            return float(e_value)
        elif e_type == "bool":
            e_value = e_value.lower()
            return e_value not in ("", "false", "no", "0")
        elif e_type in ("str", "string", "unicode"):
            return str(e_value)
        elif e_type == "list":
            return self._deserializeList(e_value)
        elif e_type == "dict":
            return self._deserializeDict(e_value)
        return e_value

    def get(self, module, name, defaultValue=None):
        """Get a setting value from the XML file."""
        if not self._initialized:
            self.load()

        if not isinstance(name, str):
            raise AttributeError(
                "SimpleXMLSerializer.get(): Invalid type for name argument."
            )

        moduleTree = self._getModuleTree(module)
        element = None
        for e in moduleTree:
            if e.tag == "Setting" and e.get("name", "") == name:
                element = e
                break
        else:
            return defaultValue

        e_value = element.text
        e_strip_raw = element.get("strip", "1").strip().lower()
        e_type = str(element.get("type", "str")).strip()

        if e_value is None:
            return defaultValue

        e_strip = e_strip_raw not in ("", "false", "no", "0")

        if e_type in ("str", "unicode"):
            if e_strip:
                e_value = e_value.strip()
        else:
            e_value = e_value.strip()

        return self.getValue(e_type, e_value)

    def set(self, module, name, value, extra_attrs=None):
        """Set a setting value in the XML file."""
        if extra_attrs is None:
            extra_attrs = {}

        if not self._initialized:
            self.load()

        if not isinstance(name, str):
            raise AttributeError(
                "SimpleXMLSerializer.set(): Invalid type for name argument."
            )

        moduleTree = self._getModuleTree(module)

        # Determine type string
        if isinstance(value, bool):
            e_type = "bool"
        elif isinstance(value, int):
            e_type = "int"
        elif isinstance(value, float):
            e_type = "float"
        elif isinstance(value, list):
            e_type = "list"
            value = self._serializeList(value)
        elif isinstance(value, dict):
            e_type = "dict"
            value = self._serializeDict(value)
        else:
            e_type = "str"

        str_value = str(value)

        for e in moduleTree:
            if e.tag == "Setting" and e.get("name", "") == name:
                e.text = str_value
                e.set("type", e_type)
                break
        else:
            attrs = {"name": name, "type": e_type}
            for k, v in extra_attrs.items():
                if k not in attrs:
                    attrs[k] = str(v)
            elm = ET.SubElement(moduleTree, "Setting", attrs)
            elm.text = str_value

    def remove(self, module, name):
        """Remove a setting from the XML file."""
        if not self._initialized:
            self.load()

        if not isinstance(name, str):
            raise AttributeError(
                "SimpleXMLSerializer.remove(): Invalid type for name argument."
            )

        moduleTree = self._getModuleTree(module)
        for e in list(moduleTree):  # Use list to allow removal while iterating
            if e.tag == "Setting" and e.get("name", "") == name:
                moduleTree.remove(e)

    def getModuleNameList(self):
        """Get a list of module names in the file."""
        if not self._initialized:
            self.load()

        moduleNames = []
        for c in self._root_element:
            if c.tag == "Module":
                name = c.get("name", "")
                moduleNames.append(name)
        return moduleNames

    def getAllSettings(self, module):
        """Get all settings for a module."""
        settingsFromFile = {}
        if not self._initialized:
            self.load()

        moduleTree = self._getModuleTree(module)

        for e in moduleTree:
            if e.tag == "Setting":
                name = e.get("name", "")
                e_value = e.text
                e_strip_raw = e.get("strip", "1").strip().lower()
                e_type = str(e.get("type", "str")).strip()

                e_strip = e_strip_raw not in ("", "false", "no", "0")

                if e_value:
                    if e_type in ("str", "unicode"):
                        if e_strip:
                            e_value = e_value.strip()
                    else:
                        e_value = e_value.strip()

                settingsFromFile[name] = self.getValue(e_type, e_value)

        return settingsFromFile

    def _validateTree(self):
        for c in self._root_element:
            if c.tag != "Module":
                raise InvalidFormat(
                    f"Invalid tag in {self._file}. Expected Module, got: {c.tag}"
                )
            if not c.get("name"):
                raise InvalidFormat(f"Invalid tag in {self._file}. Module name is empty.")

            for e in c:
                if e.tag != "Setting":
                    raise InvalidFormat(
                        f"Invalid tag in {self._file} in module {c.get('name')}. Expected Setting, got: {e.tag}"
                    )

    def _getModuleTree(self, module):
        if not isinstance(module, str):
            raise AttributeError(
                "Settings:_getModuleTree: Invalid type for module argument."
            )

        for c in self._root_element:
            if c.tag == "Module" and c.get("name", "") == module:
                return c

        return ET.SubElement(self._root_element, "Module", {"name": module})

    def _indent(self, elem, level=0):
        i = "\n" + level * "  "
        if len(elem):
            if not elem.text or not elem.text.strip():
                elem.text = i + "  "
            if not elem.tail or not elem.tail.strip():
                elem.tail = i
            for subelem in elem:
                self._indent(subelem, level + 1)
            if not elem.tail or not elem.tail.strip():
                elem.tail = i
        else:
            if level and (not elem.tail or not elem.tail.strip()):
                elem.tail = i

    def _serializeList(self, items):
        return " ; ".join(map(str, items))

    def _deserializeList(self, string):
        if not string:
            return []
        return string.split(" ; ")

    def _serializeDict(self, data):
        return " ; ".join([f"{k} : {v}" for k, v in data.items()])

    def _deserializeDict(self, serial):
        if not serial:
            return {}
        result = {}
        items = serial.split(" ; ")
        for i in items:
            if " : " in i:
                kv_pair = i.split(" : ", 1)
                result[kv_pair[0]] = kv_pair[1]
        return result
