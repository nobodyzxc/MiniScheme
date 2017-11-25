free = set()
freeTwice = set()
aloc = set()
tp = dict()
with open('leak.txt' , 'r') as f:
    data = list(f)
    for r in data:
        r = r.strip().split()
        if r[0] == 'free':
            if r[1] in free:
                freeTwice.add(r[1])
            else:
                free.add(r[1])
        else:
            aloc.add(r[1])
        tp[r[1]] = r[2]
    print('tot aloc: ' + str(len(aloc)))
    print('tot free: ' + str(len(free)))
    print('leak:')
    for p in aloc:
        if p not in free:
            print('   :' + p + ' ' + tp[p])
    print('unsafe free:')
    for p in free:
        if p not in aloc:
            print('   :' + p + ' ' + tp[p])
    print('free  twice:')
    for p in freeTwice:
        print('   :' + p + ' ' + tp[p])
