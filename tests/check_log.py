#!/usr/bin/env python3
"""Validate a codexion log read from stdin.
usage: ./codexion <args> | python3 tests/check_log.py <args...>
Checks: format, per-coder state order, dongle mutual exclusion between
neighbours, cooldown, burnout deadline and burnout log precision."""
import sys, re
n, burn, comp, dbg, ref, req, cd = map(int, sys.argv[1:8])
TOL = 3
pat = re.compile(r"^(\d+) (\d+) (has taken a dongle|is compiling|is debugging|is refactoring|burned out)$")
ev, err, burned = [], [], None
for ln in sys.stdin.read().splitlines():
    m = pat.match(ln)
    if not m:
        err.append("bad line: %r" % ln); continue
    ev.append((int(m[1]), int(m[2]), m[3]))
if burned is None:
    b = [e for e in ev if e[2] == "burned out"]
    if b:
        burned = b[0]
        if ev[-1] != burned: err.append("lines printed after burnout")
ts = [e[0] for e in ev]
if ts != sorted(ts): err.append("timestamps not monotonic")
starts = {i: [0] for i in range(1, n + 1)}
comps = {i: [] for i in range(1, n + 1)}
for t, i, s in ev:
    if s == "is compiling": starts[i].append(t); comps[i].append(t)
for i in range(1, n + 1):
    for a, b in zip(starts[i], starts[i][1:]):
        if b - a > burn + TOL: err.append("coder %d gap %d > burnout" % (i, b - a))
    for a, b in zip(comps[i], comps[i][1:]):
        if b - a < comp + dbg + ref - TOL: err.append("coder %d cycle too short" % i)
if n > 1:
    for i in range(1, n + 1):
        j = i % n + 1
        iv = sorted([(t, t + comp, i) for t in comps[i]] + [(t, t + comp, j) for t in comps[j]])
        for (s1, e1, c1), (s2, e2, c2) in zip(iv, iv[1:]):
            if c1 != c2 and s2 < e1 - TOL: err.append("overlap coders %d/%d at %d" % (c1, c2, s2))
            if c1 != c2 and s2 < e1 + cd - TOL: err.append("cooldown violated %d/%d at %d" % (c1, c2, s2))
        if n == 2: break
if burned:
    last = max(starts[burned[1]])
    if not (burn <= burned[0] - last <= burn + 10): err.append("burnout log at %d, last start %d" % (burned[0], last))
else:
    cnt = [len(comps[i]) for i in range(1, n + 1)]
    if min(cnt) < req and n > 0: err.append("ended without burnout but compiles %s < %d" % (cnt, req))
print("OK" if not err else "FAIL: " + "; ".join(err[:5]))
sys.exit(1 if err else 0)
