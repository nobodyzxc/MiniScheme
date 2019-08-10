# Zekin [![Build Status](https://travis-ci.org/nobodyzxc/Zekin.svg?branch=master)](https://travis-ci.org/nobodyzxc/Zekin)

Zekin is a naive scheme interpreter wrote in C.

To learn how to make an interpreter, I started the project.

## To Fix:

consider the expression below

```scheme
(define (f) (if 1 2 3))
(define if (lambda (a b c) #f))
(f)
; => ?
; racket => 2
; zekin  => #f
; => env in zekin is list of trees, not list

(define (func x) x)
(define (f) (func 0))
(define (func x) 'error)
(f)
```


## Feature :

   1. Tail Call Optimization
   2. Mark-and-Sweep Garbage Collection
   3. Simple macro (may improve it after I finishing SICP)
   4. Multi-line comment (`#| multi-line |#`)
   5. Interesting functions about environment
   6. A simple library manage mechanism
   7. Friendly interactive mode with tab completion
      * (You can refer the [Key binding list](http://readline.kablamo.org/emacs.html))

## Usage :
```shell
   usage: zekin [-e exprs] [-h] [-i] [-v] [-l] [file ...]

   Yet another scheme interpreter

   positional arguments:
      file   scheme script to run

   optional arguments:
      -i     run file interactively

      -e     eval expressions and exit

      -v     information about version

      -l     show library path defined

      -h     show this help message and exit
```

## Requirements :

   1. GCC
   2. [GNU Readline Library](https://tiswww.case.edu/php/chet/readline/rltop.html) (optional)

## Install :

If you don't want to use GNU RL, uncomment it in Makefile

All you need is just :
```bash
   make
```

If you want to install readline

Cygwin
```bash
   apt-cyg install libreadline-devel && make
```

Ubuntu
```bash
   sudo apt-get install libreadline6 libreadline6-dev && make
```

Arch
```bash
   sudo pacman -S readline && make
```

## Goals :

#### General :

> 1. Macro implementation
>
> 2. Provide some interesting functions to observe the ecology
>
> 3. Do some interesting optimizations
      (symbol, gc, TCO, any opt about speed and space)


#### Optional :

> 1. escape char in string (numberical, like \x \o ...)
>
> 2. more types, i.e. char, big number (8 byte current), rational ...
>
> 3. string operations
>
> 4. support printf function
>
> 5. implement continuation

#### Final :

> Run repl.ss (scheme interpreter wrote in scheme) by using zekin

## Todo :

1. consider the expressions below
```scheme
   (define f (lambda (x) (if (= x 0) 0 (+ x (f (- x 1))))))
   (f 100000) ; Can I opt it to tail call? (Do I need CPS?)
```

2. add let family, `let*`, `letrec` ...

3. figureout `syntax-rules` to improve ( rewrite ) macro

4. consider tco, when apply a closure,
    with non-lambda args, can I just update without new a env?
    (How about Closure Passing Style)

5. Implement [more functions](https://srfi.schemers.org/srfi-1/srfi-1.html)
