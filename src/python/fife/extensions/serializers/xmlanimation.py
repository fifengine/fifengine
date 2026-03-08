# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

from builtins import str

from fife import fife
from fife.extensions.serializers import ET, InvalidFormat


def loadXMLAnimation(engine, filename):
    f = engine.getVFS().open(filename)
    f.thisown = 1

    imgMgr = engine.getImageManager()
    aniMgr = engine.getAnimationManager()

    tree = ET.parse(f)
    node = tree.getroot()

    ani_id = node.get("id")
    if not ani_id:
        ani_id = filename

    if aniMgr.exists(ani_id):
        animation = aniMgr.getPtr(str(ani_id))
    else:
        animation = aniMgr.create(str(ani_id))

    common_width = int(node.get("width", 0))
    common_height = int(node.get("height", 0))
    common_frame_delay = int(node.get("delay", 0))
    common_x_offset = int(node.get("x_offset", 0))
    common_y_offset = int(node.get("y_offset", 0))
    animation.setActionFrame(int(node.get("action_frame", 0)))

    frames = node.findall("frame")
    if not frames:
        raise InvalidFormat("animation without <frame>s")

    atlas = node.get("atlas")

    if atlas:
        path = filename.split("/")
        path.pop()
        path.append(str(atlas))
        atlas_file = "/".join(path)
        if imgMgr.exists(atlas_file):
            atlas_img = imgMgr.get(str(atlas_file))
        else:
            atlas_img = imgMgr.create(str(atlas_file))
        # parse atlas animation format 2 (e.g. cursor)
        for frame in frames:
            source = frame.get("source")
            if not source:
                raise InvalidFormat("animation without <frame>s")

            frame_x_pos = int(frame.get("xpos", 0))
            frame_y_pos = int(frame.get("ypos", 0))
            frame_x_offset = int(frame.get("x_offset", common_x_offset))
            frame_y_offset = int(frame.get("y_offset", common_y_offset))
            frame_delay = int(frame.get("delay", common_frame_delay))
            frame_width = int(frame.get("width", common_width))
            frame_height = int(frame.get("height", common_height))

            # xml paths are relative to the directory of the file they're used in.
            path = filename.split("/")
            path.pop()
            path.append(str(source))

            frame_file = "/".join(path)
            frame_file = atlas, ":", frame_file

            if imgMgr.exists(str(frame_file)):
                frame_img = imgMgr.get(str(frame_file))
            else:
                frame_img = imgMgr.create(str(frame_file))
                region = fife.Rect(frame_x_pos, frame_y_pos, frame_width, frame_height)
                frame_img.useSharedImage(atlas_img, region)
                frame_img.setXShift(frame_x_offset)
                frame_img.setYShift(frame_y_offset)

            animation.addFrame(frame_img, frame_delay)
    else:
        # parse single images
        for frame in frames:
            source = frame.get("source")
            if not source:
                raise InvalidFormat("animation without <frame>s")

            frame_x_offset = int(frame.get("x_offset", common_x_offset))
            frame_y_offset = int(frame.get("y_offset", common_y_offset))
            frame_delay = int(frame.get("delay", common_frame_delay))

            # xml paths are relative to the directory of the file they're used in.
            path = filename.split("/")
            path.pop()
            path.append(str(source))

            image_file = "/".join(path)

            img = imgMgr.create(image_file)
            img.setXShift(frame_x_offset)
            img.setYShift(frame_y_offset)

            animation.addFrame(img, frame_delay)

    # 	animation.thisown = 0
    return animation