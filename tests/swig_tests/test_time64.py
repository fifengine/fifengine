# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

import json
import pickle
import random
import time
from unittest import mock

from fife import fife
import pytest


def _to_signed32(value):
    wrapped = value & 0xFFFFFFFF
    return wrapped if wrapped < 0x80000000 else wrapped - 0x100000000


def test_event_timestamp64_roundtrip_large_value(engine_minimized):
    del engine_minimized

    cmd = fife.Command()
    value64 = 0x100000123

    cmd.setTimeStamp64(value64)

    assert isinstance(cmd.getTimeStamp64(), int)
    assert cmd.getTimeStamp64() == value64


@pytest.mark.parametrize(
    "value64",
    [
        0,
        1,
        0xFFFFFFFF,
        0x100000000,
        0xFFFFFFFFFFFFFFFF,
    ],
)
def test_event_timestamp64_boundary_roundtrip(engine_minimized, value64):
    del engine_minimized

    cmd = fife.Command()
    cmd.setTimeStamp64(value64)

    assert cmd.getTimeStamp64() == value64


@pytest.mark.parametrize("value", [-1, 0x10000000000000000])
def test_event_timestamp64_rejects_out_of_range_values(engine_minimized, value):
    del engine_minimized

    cmd = fife.Command()
    with pytest.raises(OverflowError):
        cmd.setTimeStamp64(value)


@pytest.mark.parametrize(
    ("value64", "expected_legacy32"),
    [
        (0xFFFFFFFE, -2),
        (0xFFFFFFFF, -1),
    ],
)
def test_event_timestamp32_near_rollover_boundary(engine_minimized, value64, expected_legacy32):
    del engine_minimized

    cmd = fife.Command()
    cmd.setTimeStamp64(value64)

    assert cmd.getTimeStamp64() == value64
    assert cmd.getTimeStamp() == expected_legacy32


@pytest.mark.parametrize(
    ("value64", "expected_legacy32"),
    [
        (0x100000000, 0),
        (0x100000001, 1),
    ],
)
def test_event_timestamp32_crosses_rollover_boundary(engine_minimized, value64, expected_legacy32):
    del engine_minimized

    cmd = fife.Command()
    cmd.setTimeStamp64(value64)

    assert cmd.getTimeStamp64() == value64
    assert cmd.getTimeStamp() == expected_legacy32


def test_event_timestamp64_accelerated_plus_30_days(engine_minimized):
    del engine_minimized

    ms_30_days = 30 * 24 * 60 * 60 * 1000
    cmd = fife.Command()
    cmd.setTimeStamp64(ms_30_days)

    assert cmd.getTimeStamp64() == ms_30_days
    assert cmd.getTimeStamp() == _to_signed32(ms_30_days)


def test_event_timestamp64_accelerated_plus_1_year(engine_minimized):
    del engine_minimized

    ms_1_year = 365 * 24 * 60 * 60 * 1000
    cmd = fife.Command()
    cmd.setTimeStamp64(ms_1_year)

    assert cmd.getTimeStamp64() == ms_1_year
    assert cmd.getTimeStamp() == _to_signed32(ms_1_year)


def test_event_timestamp64_accelerated_large_step_jumps(engine_minimized):
    del engine_minimized

    steps = [
        0,
        1_000,
        2_000_000_000,
        30 * 24 * 60 * 60 * 1000,
        365 * 24 * 60 * 60 * 1000,
        10 * 365 * 24 * 60 * 60 * 1000,
        1 << 40,
    ]

    cmd = fife.Command()
    observed_64 = []
    observed_32 = []
    for value64 in steps:
        cmd.setTimeStamp64(value64)
        observed_64.append(cmd.getTimeStamp64())
        observed_32.append(cmd.getTimeStamp())

    assert observed_64 == steps
    assert observed_32 == [_to_signed32(v) for v in steps]
    assert [observed_64[i + 1] - observed_64[i] for i in range(len(observed_64) - 1)] == [
        steps[i + 1] - steps[i] for i in range(len(steps) - 1)
    ]


def test_timemanager_now64_monotonic_non_decreasing(engine_minimized):
    tm = engine_minimized.getTimeManager()

    values = []
    for _ in range(8):
        tm.update()
        values.append(tm.now64())

    assert values == sorted(values)


def test_timemanager_now64_suspend_resume_like_pause(engine_minimized):
    tm = engine_minimized.getTimeManager()

    tm.update()
    before = tm.now64()

    # sleep64 simulates a paused period where process does not pump frames.
    tm.sleep64(40)
    tm.update()
    after = tm.now64()

    assert after >= before


def test_timemanager_now64_independent_from_python_wall_clock(engine_minimized):
    tm = engine_minimized.getTimeManager()

    tm.update()
    before = tm.now64()

    # SDL ticks are monotonic and must not depend on Python's wall-clock APIs.
    with mock.patch.object(time, "time", return_value=0.0):
        tm.sleep64(20)
        tm.update()
        patched = tm.now64()

    assert patched >= before


def test_timemanager_now64_legacy_wrapper(engine_minimized):
    tm = engine_minimized.getTimeManager()
    tm.update()

    now64 = tm.now64()
    now32 = tm.getTime()

    assert isinstance(now64, int)
    assert isinstance(now32, int)
    assert now32 == (now64 & 0xFFFFFFFF)


def test_timeprovider_game_time64_legacy_wrapper(engine_minimized):
    del engine_minimized

    tp = fife.TimeProvider(None)
    time64 = tp.getGameTime64()
    time32 = tp.getGameTime()

    assert isinstance(time64, int)
    assert isinstance(time32, int)
    assert time32 == (time64 & 0xFFFFFFFF)


def test_instance_runtime64_legacy_wrapper(engine_minimized):
    model = engine_minimized.getModel()
    map_obj = model.createMap("map_time64")
    grid = model.getCellGrid("square")
    layer = map_obj.createLayer("layer_time64", grid)
    obj = model.createObject("object_time64", "test_nspace")

    inst = layer.createInstance(obj, fife.ModelCoordinate(1, 1))

    runtime64 = inst.getRuntime64()
    runtime32 = inst.getRuntime()

    assert isinstance(runtime64, int)
    assert isinstance(runtime32, int)
    assert runtime32 == (runtime64 & 0xFFFFFFFF)


def test_soundemitter_play_timestamp64_available(engine_minimized):
    sm = engine_minimized.getSoundManager()
    sm.init()
    emitter = sm.createEmitter()

    timestamp64 = emitter.getPlayTimestamp64()

    assert isinstance(timestamp64, int)
    assert timestamp64 >= 0


@pytest.mark.parametrize(
    "value64",
    [
        0,
        1,
        0xFFFFFFFF,
        0x100000000,
        0xFFFFFFFFFFFFFFFF,
    ],
)
def test_time64_python_arithmetic_happy_and_edge_cases(engine_minimized, value64):
    del engine_minimized

    cmd = fife.Command()
    cmd.setTimeStamp64(value64)
    value = cmd.getTimeStamp64()

    assert isinstance(value, int)
    assert value + 1 == value64 + 1
    assert (value & 0xFFFFFFFF) == (value64 & 0xFFFFFFFF)


@pytest.mark.parametrize(
    "value64",
    [
        0,
        1,
        0xFFFFFFFF,
        0x100000000,
        0xFFFFFFFFFFFFFFFF,
    ],
)
def test_time64_json_roundtrip_happy_and_edge_cases(engine_minimized, value64):
    del engine_minimized

    cmd = fife.Command()
    cmd.setTimeStamp64(value64)
    value = cmd.getTimeStamp64()

    payload = {"timestamp64": value}
    encoded = json.dumps(payload)
    decoded = json.loads(encoded)

    assert decoded["timestamp64"] == value64


@pytest.mark.parametrize(
    "value64",
    [
        0,
        1,
        0xFFFFFFFF,
        0x100000000,
        0xFFFFFFFFFFFFFFFF,
    ],
)
def test_time64_pickle_roundtrip_happy_and_edge_cases(engine_minimized, value64):
    del engine_minimized

    cmd = fife.Command()
    cmd.setTimeStamp64(value64)
    value = cmd.getTimeStamp64()

    payload = {"timestamp64": value}
    restored = pickle.loads(pickle.dumps(payload))

    assert restored["timestamp64"] == value64


def test_time64_fuzz_timestamp_arithmetic(engine_minimized):
    del engine_minimized

    rng = random.Random(1337)
    samples = [
        0,
        1,
        0xFFFFFFFF,
        0x100000000,
        0xFFFFFFFFFFFFFFFF,
        *(rng.getrandbits(64) for _ in range(300)),
    ]

    cmd = fife.Command()
    for value64 in samples:
        cmd.setTimeStamp64(value64)
        observed64 = cmd.getTimeStamp64()
        observed32 = cmd.getTimeStamp()

        assert observed64 == value64
        assert observed32 == _to_signed32(value64)

        # Happy path: arithmetic on returned Python ints must stay exact.
        assert observed64 + 123 == value64 + 123
        # Edge case: legacy wrapped 32-bit view must match explicit masking.
        assert (observed64 & 0xFFFFFFFF) == (value64 & 0xFFFFFFFF)


def test_time64_serialization_roundtrip_bulk(engine_minimized):
    del engine_minimized

    rng = random.Random(2026)
    values = [
        0,
        1,
        0xFFFFFFFF,
        0x100000000,
        0xFFFFFFFFFFFFFFFF,
        *(rng.getrandbits(64) for _ in range(200)),
    ]

    cmd = fife.Command()
    observed = []
    for value64 in values:
        cmd.setTimeStamp64(value64)
        observed.append(cmd.getTimeStamp64())

    # JSON round-trip (text serialization)
    json_blob = json.dumps({"timestamps": observed})
    json_restored = json.loads(json_blob)["timestamps"]
    assert json_restored == values

    # Pickle round-trip (binary serialization)
    pickle_restored = pickle.loads(pickle.dumps({"timestamps": observed}))["timestamps"]
    assert pickle_restored == values
