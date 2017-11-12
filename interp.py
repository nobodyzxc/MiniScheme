import sys
glenv = dict()
count = 0
text = []
buff = ""
for t in sys.stdin:
    t = ' ' + t.strip()
    if t and t[0] == '#':
        continue
    for i , v in enumerate(t):
        if v == '(':
            count += 1
        elif v == ')':
            count -= 1
        elif v == ';':
            break #discard semicolon(comment)
        buff += v
        if count == 0:
            text.append(buff)
            buff = ""
in_str = False
for i , l in enumerate(text):
    token = [] ; buff = ""
    for t in l:
        if t == '"':
            in_str = not in_str
        if not in_str and t in ('(' , ' ' , ')'):
            if buff:
                token.append(buff)
                buff = ""
            if t is not ' ':
                token.append(t)
        else:
            buff += t
    text[i] = token
print(text)
