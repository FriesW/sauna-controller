import itertools

# RC = 2.2 uF * 500 ohm = 1.1 ms
# 5RC = 99.3% = 5.5 ms

states = [-1, 0, 1]
resistors = [330, 1000, 1000]

res = dict()

for c in itertools.combinations_with_replacement(states, len(resistors)):
    l = [32000]
    h = []
    for r, s in zip(resistors, c):
        if s < 0: l.append( r )
        if s > 0: h.append( r )
    if len(l) == 0 or len(h) == 0: continue
    rl = 1 / sum([1/x for x in l])
    rh = 1 / sum([1/x for x in h])
    r_ratio = int( rl / (rh + rl) * 1000000 )
    res[r_ratio] = c

for k, v in sorted(res.items()):
    k = k / 1000000
    print(f'{k:.6f}')

for k, v in sorted(res.items()):
    print(f'{v}')
