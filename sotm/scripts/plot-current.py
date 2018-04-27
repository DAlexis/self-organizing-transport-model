#!/usr/bin/env python3

import matplotlib.pyplot as plt
import matplotlib.ticker as mticker
import argparse

parser = argparse.ArgumentParser("Plotting tool")
parser.add_argument("--input", "-i", type=str, default="currents.txt", help="Input file name")
args = parser.parse_args()

times = []
currents = []

plot_from = 0

with open(args.input) as f:
    index = 0
    for line in f:
        vals = line.split(",")
        t = float(vals[0])
        i = float(vals[1])
        times.append(t*1e6)
        currents.append(i)
        if i > 1e-4 and plot_from == 0:
            plot_from = index
        index += 1

plt.rc('text', usetex=True)
plt.rc('text.latex',unicode=True)
plt.rc('text.latex', preamble='\\usepackage[utf8]{inputenc}')
plt.rc('text.latex',preamble='\\usepackage[russian]{babel}')
plt.rc('font', family='serif')

##plt.title(r"\TeX\ is Number "
#          r"$\displaystyle\sum_{n=1}^\infty\frac{-e^{i\pi}}{2^n}$!",
#          fontsize=16, color='gray')

fig, ax = plt.subplots()
ax.set_yscale("log", nonposy='clip')

plt.xlabel('Время $t$, мкс')
plt.ylabel('Суммарный ток $I$, А')

#plt.subplots_adjust(top=0.8)
plt.plot(times[1000:], currents[1000:])

#f = mticker.ScalarFormatter(useOffset=False, useMathText=True)
#g = lambda x,pos : "${}$".format(f._formatSciNotation('%1.10e' % x))
#plt.gca().xaxis.set_major_formatter(mticker.FuncFormatter(g))

plt.savefig("currents.png")
plt.show()
#plt.xlabel(r'Ток , сек')