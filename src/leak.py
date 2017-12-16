#!/usr/bin/python3.6
showed = set()
tab = set()
refree = []
erfree = []
tp = dict()
with open('.leak' , 'r') as f:
    data = list(f)
    for r in data:
        r = r.strip().split()
        if r[0] == 'free':
            if r[1] in tab:
                tab.remove(r[1])
            elif r[1] in showed:
                refree.append([r[1] , r[2]])
            else:
                erfree.append([r[1] , r[2]])
        else:
            tab.add(r[1])
            showed.add(r[1])
        tp[r[1]] = r[2]
    print('leak:')
    for p in tab:
        print('    ' + p + ' ' + tp[p])
    print('refree:')
    for p in refree:
        print('    ' + ' '.join(p))
    print('erfree:')
    for p in erfree:
        print('    ' + ' '.join(p))
