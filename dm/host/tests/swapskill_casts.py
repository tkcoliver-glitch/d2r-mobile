#!/usr/bin/env python3
"""Check a native-game off-hand charged-skill tap regression run.

Use an isolated save with enough charges, outside town, and DM_KEYS to tap
the bound skill repeatedly (with gaps longer than the cast/swap animation).
Pass the resulting log and the number of taps. No game/save data is bundled.
Example: python3 swapskill_casts.py run.log --taps 16 --skill 54

This catches premature READY: the swap animation was mistaken for a cast,
so the combo switched back without spending a charge. Each single tap must
spend exactly one charge, finish, and return to its original weapon set.
"""
import argparse
import re
from pathlib import Path


def check(log, taps, skill):
    current = None
    completed = 0
    failures = []
    for line in log.splitlines():
        request = re.search(r"槽 \d+ 技能 (\d+)（物品 \d+）在另一组", line)
        if request:
            if current is not None:
                failures.append("previous tap did not return to its weapon set")
            current = {} if int(request[1]) == skill else None
        if current is None:
            continue
        ready = re.search(r"技能可放了（换武器 → 可放 \d+ 拍，剩余 (\d+) 次）", line)
        if ready:
            current["before"] = int(ready[1])
        returned = re.search(r"切回（当前组 \d+，技能 (\d+) 剩余 (-?\d+) 次）", line)
        if returned and int(returned[1]) == skill:
            current["after"] = int(returned[2])
        if "已切回第" in line:
            completed += 1
            before, after = current.get("before"), current.get("after")
            if before is None or after is None or before - after != 1:
                failures.append(f"tap {completed}: charges {before} -> {after}, expected one cast")
            current = None
    if current is not None or completed != taps:
        failures.append(f"completed {completed} of {taps} taps")
    return failures


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("log", type=Path)
    parser.add_argument("--taps", type=int, required=True)
    parser.add_argument("--skill", type=int, default=54)
    args = parser.parse_args()
    errors = check(args.log.read_text(), args.taps, args.skill)
    print("\n".join(errors) if errors else f"PASS: {args.taps} taps cast once and returned")
    raise SystemExit(bool(errors))
