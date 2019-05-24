import sys
import random


TYPE = sys.argv[1]

PAR1 = int(sys.argv[2])
PAR2 = int(sys.argv[3]) if len(sys.argv) > 3 else None


def getMycielskiV(n):
    if n == 2:
        return 2
    return getMycielskiV(n - 1) * 2 + 1


def getMycielskiE(n):
    if n == 2:
        return [[1, 2]]
    prev_e = getMycielskiE(n - 1)
    prev_v = getMycielskiV(n - 1)

    return prev_e \
        + [[v, v + prev_v] for e in prev_e for v in e] \
        + [[v + 1 + prev_v, 2 * prev_v + 1] for v in range(prev_v)]


if TYPE == 'BK':
    print("{} {}".format(PAR1 + PAR2, PAR1 * PAR2))
    for i in range(PAR1):
        for j in range(PAR2):
            print("{} {}".format(i + 1, PAR1 + j + 1))
elif TYPE == 'C':
    print("{} {}".format(PAR1, PAR1))
    for i in range(PAR1):
        print("{} {}".format(i + 1, (i + 1) % PAR1 + 1))
elif TYPE == 'K':
    print("{} {}".format(PAR1, PAR1 * (PAR1 - 1) // 2))
    for i in range(PAR1):
        for j in range(i + 1, PAR1):
            print("{} {}".format(i + 1, j + 1))
elif TYPE == 'L':
    print("{} {}".format(PAR1, PAR1 - 1))
    for i in range(PAR1 - 1):
        print("{} {}".format(i + 1, i + 2))
elif TYPE == 'S':
    print("{} {}".format(PAR1, PAR1 - 1))
    for i in range(1, PAR1):
        print("{} {}".format(1, i + 1))
elif TYPE == 'T':
    print("{} {}".format(PAR1, PAR1 - 1))
    for i in range(1, PAR1):
        print("{} {}".format((i + 1) // 2, i + 1))

elif TYPE == 'M':
    v = getMycielskiV(PAR1)
    e = getMycielskiE(PAR1)

    print("{} {}".format(v, len(e)))
    for edge in e:
        print("{} {}".format(edge[0], edge[1]))

elif TYPE == 'LD':
    print("{} {}".format(PAR1, PAR1 - 1))
    for i in range(PAR1 - 1):
        print("{} {}".format(i + 1, i + 3 if i + 3 <= PAR1 else i + 2))

elif TYPE == 'R':
    print("{} {}".format(PAR1, PAR2))

    random.seed()
    edges = set()

    while len(edges) < PAR2:
        v1 = random.randint(1, PAR1)
        v2 = random.randint(1, PAR1 - 1)

        if v2 >= v1:
            v2 += 1
        else:
            t = v1
            v1 = v2
            v2 = t
        edges.add((v1, v2))

    for e in edges:
        print(*e, sep=' ')