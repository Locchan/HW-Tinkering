# Plots memory stats exported by meteo in debug mode

import time
import matplotlib.pyplot

data = {}

def tailf(afile):
    line = afile.readline()
    while line:
        parse_line(line)
        line = afile.readline()


def parse_line(aline):
    split_line = aline.split(":")
    if not split_line[0] in data:
        data[split_line[0]] = ([], [])
    data[split_line[0]][0].append(int(split_line[2][:-1]))
    data[split_line[0]][1].append(int(split_line[1])) 

def draw():
    i = 0
    plt = matplotlib.pyplot

    for anitem in data:
        plt.figure(i)
        i+=1
        plt.plot(data[anitem][0], data[anitem][1])
        plt.title(label=anitem)
    plt.show()

mem_data_fp = open("/tmp/meteo_debug_meminfo","r")

tailf(mem_data_fp)
draw()