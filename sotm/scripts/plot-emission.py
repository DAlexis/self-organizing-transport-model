#!/usr/bin/env python3

import matplotlib.pyplot as plt
import pandas as pd
import argparse

parser = argparse.ArgumentParser("Electric field plotting tool")
parser.add_argument("--input", "-i", type=str, default="/home/dalexies/Projects/self-organizing-transport-model/sotm/cpp-sources/build/release/lightning-modeller/emission.csv", help="Input file name")
args = parser.parse_args()

df = pd.read_csv(args.input)

plt.rc('text', usetex=True)
# plt.rc('text.latex',unicode=True)
plt.rc('text.latex', preamble='\\usepackage[utf8]{inputenc}')
plt.rc('text.latex',preamble='\\usepackage[russian]{babel}')
plt.rc('font', family='serif')

plt.xlabel('Время $t$, мкс')
plt.ylabel('Электрическое поле $E$, В/м')

t = df["time"] * 1e6

#plt.plot(t, df["Ex"], label="Ex")
#plt.plot(t, df["Ey"], label="Ey")
plt.plot(t, df["Ez"], label="Ez")
plt.grid(True)
plt.legend()
plt.show()