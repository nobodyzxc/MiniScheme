
car    = lambda l: l[0]
cdr    = lambda l: l[1:]
cadr    = lambda l: car(cdr(l))
cddr    = lambda l: cdr(cdr(l))
caddr    = lambda l: car(cdr(cdr(l)))
isList = lambda l: isinstance(l , list)
isBlank = lambda c: c in (' ' , '\n' , '\t' , '\r')
isSymbol = lambda s: isinstance(s , str) and s and s[0] == "'"
isString = lambda s: isinstance(s , str) and s and s[0] == '"'

glenv  = dict()

def read_file():
    #return a list of str
    buff = ""
    text = []
    parth_cnt = 0
    dbquo_cnt = False
    while True:
        try:
            t = input()
            t = '\n' + t.strip()
            if t.strip() and t.strip()[0] == '#':
                continue
            for i , v in enumerate(t):
                if v == '(':
                    parth_cnt += 1
                elif v == ')':
                    parth_cnt -= 1
                elif v == '"':
                    dbquo_cnt = not dbquo_cnt
                elif v == ';':
                    break #discard semicolon(comment)
                buff += v
                if parth_cnt == 0 and dbquo_cnt == False \
                        and buff.strip() != "'":
                    if buff.strip():
                        text.append(buff)
                    buff = ""
        except:
            return text

def error(msg):
    print("error : " + msg)
    exit(0)

def next_symbol(expr , p):
    # a str and p is start with '
    count = 0
    buff = ""
    while p < len(expr):
        if buff and buff.strip() != "'" \
                and count == 0 and isBlank(expr[p]):
            return buff , p
        if expr[p] == '(': count += 1
        elif expr[p] == ')': count -= 1
        if count < 0:
            break
        if not isBlank(expr[p]) or buff.strip() != "'":
            buff += expr[p]
        p += 1
    return ['quote' , parse(tokenize(buff[1:]) , 0)[0]] , p

def next_str(expr , p):
    # a str and p is start with "
    quote = False
    buff = ""
    while p < len(expr):
        if buff and not quote:
            return buff , p
        if expr[p] == '"': quote = not quote
        buff += expr[p]
        p += 1
    return buff , p

def tokenize(txt):
    # str -> [str] = tokens
    token = [] ; buff = "" ; p = 0
    while p < len(txt):
        if txt[p] == '"':
            strg , p = next_str(txt , p)
            token.append(strg)
        elif isBlank(txt[p]) or txt[p] in ('(' , ')'):
            if buff:
                token.append(buff)
                buff = ""
            if not isBlank(txt[p]):
                token.append(txt[p])
            p += 1
        elif txt[p] == "'":
            sym , p = next_symbol(txt , p)
            token.append(sym)
        else:
            buff += txt[p]
            p += 1
    if buff: token.append(buff)
    return token

def parse(expr , p):
    # [str] = tokens -> [e , [e ... ] , ... , e] | e
    # discard '(' and build into nested list struct
    sexpr = []
    if expr[p] == '(': # list
        p += 1
    else: # atom
        return expr[p] , p + 1
    while p < len(expr):
        if expr[p] == '(':
            # nested list
            nexpr , p = parse(expr , p)
            sexpr.append(nexpr)
        elif expr[p] == ')':
            return sexpr , p + 1
        else:
            # atom
            sexpr.append(expr[p])
            p += 1

def find_func(func):
    return {
            'list' : lambda *args: list(args) ,
            'display' : print ,
            }[func]

def eval_atom(expr):
    if isinstance(expr , str):
        if isSymbol(expr): return eval_symbol(expr)
        if isString(expr): return expr
    else:
        return expr

def interp_eval(expr , env):
    if not isList(expr):
        return eval_atom(expr)
    else:
        if expr:
            call = car(expr)
        if call == 'define':
            define(cadr(expr) , caddr(expr))
        elif call == 'if':
            if_expr(cadr(expr) , caddr(expr) , cadddr(expr))
        else:
            return find_func(car(expr))(*cdr(expr))

if __name__ == '__main__':
    #sexpr = [parse(expr , 0)[0] for expr in tokenize(read_file())]
    exprs = [parse(expr , 0)[0] for expr in  \
            [tokenize(e) for e in read_file()]]
    for expr in exprs:
        print(expr)
    #    rtn = interp_eval(expr , glenv)
    #    if rtn: print(rtn)
    #for expr in sexpr: print(expr)
    #for expr in sexpr:
    #    interp_eval(expr , glenv)
