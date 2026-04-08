# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors



def test_animated_cursor(engine):
    engine.getModel()
    img_mgr = engine.getImageManager()
    anim_mgr = engine.getAnimationManager()

    frame_delay = 100

    cursor_anim = anim_mgr.create("cursor_anim")

    imgs = []
    imgs.append(img_mgr.load("tests/data/crate/full_s_000.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0001.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0002.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0003.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0004.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0005.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0006.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0007.png"))
    imgs.append(img_mgr.load("tests/data/crate/full_s_0008.png"))

    for img in imgs:
        cursor_anim.addFrame(img, frame_delay)

    cursor_drag_anim = anim_mgr.create("cursor_drag_anim")

    imgs = []
    imgs.append(img_mgr.load("tests/data/wolf_walk/gfx/run_e_1.png"))
    imgs.append(img_mgr.load("tests/data/wolf_walk/gfx/run_e_2.png"))
    imgs.append(img_mgr.load("tests/data/wolf_walk/gfx/run_e_3.png"))
    imgs.append(img_mgr.load("tests/data/wolf_walk/gfx/run_e_4.png"))
    imgs.append(img_mgr.load("tests/data/wolf_walk/gfx/run_e_5.png"))

    for img in imgs:
        cursor_drag_anim.addFrame(img, frame_delay)

    cursor = engine.getCursor()
    cursor.set(cursor_anim)
    cursor.setDrag(cursor_drag_anim, 50, 50)

    engine.initializePumping()

    for _ in range(250):
        engine.pump()

    engine.finalizePumping()
