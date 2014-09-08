#!/usr/bin/python
import re

thresholds = [0.02, 0.20, 0.50, 1.00, 3000]


def fixTime(m):
    timeamt = 1000*float(m.group('time'))
    lvl = 0
    while timeamt > thresholds[lvl]:
        #print("{} , {}".format(timeamt, thresholds[lvl]))
        if lvl +1 == len(thresholds):
            break
        lvl += 1
    result = "{:.3f} lv{} ".format(timeamt, lvl)
    #print(result)
    #    print(newtime + "\t|\t" + m.group("time"))
    return m.group('before') + result + m.group('after')

def main():
    f_in = open("tmp/profiny.out", "r")
    f_out = open("tmp/profiny.graphed.txt", "w")
    for line in f_in:
        linefixed = line
        linefixed = re.sub("(?P<before>T:)(?P<time>[0-9\.\-e]+)(?P<after> )", fixTime, linefixed)
        linefixed = re.sub(":[0-9]+\s+T", ":  T:", linefixed)
        linefixed = re.sub("[a-zA-Z_]+.cpp:", "", linefixed)
        linefixed = re.sub("%:[0-9\.]+", "", linefixed)
        f_out.write(linefixed)
    f_out.flush()
    f_out.close()

if __name__ == '__main__':
    main()
