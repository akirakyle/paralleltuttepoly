#!/usr/bin/env python
from __future__ import print_function
import sys
import re

def termRegexp(x,y):
    def fmtVar(s,v):
        if v == 0: return r''
        if v == 1: return r'\*{0}'.format(s)
        return r'\*{0}\^{1}'.format(s,v)
    return r' [0-9]+{0}{1} '.format(fmtVar('x',x), fmtVar('y',y))

def getCoeff(s,x,y):
    term = re.findall(termRegexp(x,y), s)
    if len(term) > 1: print('regexp found multiple terms')
    if len(term) == 0: return 0
    return int(term[0].split('*')[0])


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print('usage: {} deg fname1 fname2'.format(sys.argv[0]))
        sys.exit(1)
    deg = int(sys.argv[1])
    fname1 = sys.argv[2]
    fname2 = sys.argv[3]
    f1 = open(fname1, 'r').read()
    f2 = open(fname2, 'r').read()

    retval = 0
    for x in range(deg):
        for y in range(deg):
            if getCoeff(f1,x,y) != getCoeff(f2,x,y):
                print('x^{0}*y^{1} : {2} != {3}'.format(
                    x, y, getCoeff(f1,x,y), getCoeff(f2,x,y)))
                retval = 1
            #print('1: {}*x^{}*y^{}'.format(getCoeff(f1,x,y), x, y))
            #print('2: {}*x^{}*y^{}'.format(getCoeff(f2,x,y), x, y))
    #sys.exit(retval)
