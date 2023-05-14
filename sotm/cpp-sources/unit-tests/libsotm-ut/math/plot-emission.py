#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun May 14 16:45:46 2023

@author: dalexies
"""

import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("dipole_samples.csv")
plt.plot(df["t"], df["Ex"], label="Ex")
plt.plot(df["t"], df["Ey"], label="Ey")
plt.plot(df["t"], df["Ez"], label="Ez")
plt.legend()
plt.show()