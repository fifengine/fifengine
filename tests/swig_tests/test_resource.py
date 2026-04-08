# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import pytest
from fife import fife


def test_image_img_mgr(engine):
    img_mgr = engine.getImageManager()
    assert img_mgr
    assert img_mgr.getTotalResources() == 0
    img = img_mgr.create("tests/data/beach_e1.png")
    assert img_mgr.getTotalResourcesLoaded() == 0
    assert img_mgr.getTotalResourcesCreated() == 1
    img = img_mgr.get(img.getHandle())
    assert img_mgr.getTotalResourcesLoaded() == 1
    assert img_mgr.getTotalResourcesCreated() == 0


def test_image_img_mgr_fail(engine):
    img_mgr = engine.getImageManager()
    with pytest.raises(fife.NotFound):
        img_mgr.load("does_not_exist.png")
