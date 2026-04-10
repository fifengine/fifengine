#!/usr/bin/env python3

# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors


"""Lightweight serialization helpers for RPG demo objects."""


class Serializer:
    """Base mixin providing serialize/deserialize hooks for demo objects."""

    def __init__(self):
        """Create a serializer instance (no-op base implementation)."""
        return

    def serialize(self):
        """Return a serializable representation (override in subclasses)."""
        pass

    def deserialize(self, valuedict=None):
        """Restore object state from a dict (override in subclasses)."""
        pass
