import datetime
import os


PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), os.path.pardir)
RESULTS_PATH = os.path.join(PATH, "out\\results")

INPUTS_PATH = os.path.join(PATH, "data")

algs = os.listdir(RESULTS_PATH)
inputs = os.listdir(INPUTS_PATH)

print(*(["name"] + ["{0}.result\t{0}.maxc\t{0}.time".format(alg) for alg in algs] + ["n", "m", "max.deg"]), sep='\t')

for inp in inputs:
    results = [inp.split('.')[0]]
    for alg in algs:
        fname = os.path.join(RESULTS_PATH, alg, inp)
        if os.path.exists(fname):
            with open(fname, "r") as file:
                first = None
                res = "ERR"
                maxc = "ERR"
                time = "ERR"
                last = None
                for i, line in enumerate(file):
                    line = line.strip()
                    if i == 0:
                        first = line
                    if i == 1:
                        if "error" not in line and "bad" not in line:
                            maxc, res = line.split(" ")
                        else:
                            break
                    if len(line) > 0:
                        last = line

                if res != "ERR":
                    start = datetime.datetime.strptime(first + "000", "%d/%m/%Y %H:%M:%S.%f")
                    stop = datetime.datetime.strptime(last + "000", "%d/%m/%Y %H:%M:%S.%f")
                    td = (stop - start)
                    time = str(td.total_seconds() + td.microseconds / 1000000)
                results.append("{}\t{}\t{}".format(res, maxc, time))
        else:
            results.append("NA\tNA\tNA")

    with open(os.path.join(INPUTS_PATH, inp)) as file:
        n = None
        e = []

        for line in file:
            if n is None:
                n = int(line.split(' ')[0])
            else:
                e.append([int(v) for v in line.strip().split(' ')])

        v_in_e = [v for edge in e for v in edge]
        max_deg_v = max(set(v_in_e), key=v_in_e.count)

        results.extend([n, len(e), v_in_e.count(max_deg_v)])

    print(*results, sep="\t")
