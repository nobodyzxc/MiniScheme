#### Primary
```
; operators

'%               : <procedure:%>
'*               : <procedure:*>
'+               : <procedure:+>
'-               : <procedure:->
'/               : <procedure:/>
'mod             : <procedure:mod>

; boolean functions, predictors

'<               : <procedure:<>
'<=              : <procedure:<=>
'=               : <procedure:=>
'>               : <procedure:>>
'>=              : <procedure:>=>
'eq?             : <procedure:eq?>
'eqv?            : <procedure:eqv?>
'equal?          : <procedure:equal?>

'not             : <procedure:not>
'env?            : <procedure:env?>
'void?           : <procedure:void?>
'null?           : <procedure:null?>
'pair?           : <procedure:pair?>
'list?           : <procedure:list?>
'exact?          : <procedure:exact?>
'number?         : <procedure:number?>
'integer?        : <procedure:integer?>
'symbol?         : <procedure:symbol?>
'string?         : <procedure:string?>
'boolean?        : <procedure:boolean?>
'closure?        : <procedure:closure?>
'procedure?      : <procedure:procedure?>

'port?           : <procedure:port?>
'in-port?        : <procedure:in-port?>
'out-port?       : <procedure:out-port?>

; ctor, list

'car             : <procedure:car>
'cdr             : <procedure:cdr>
'cons            : <procedure:cons>
'void            : <procedure:void>
'length          : <procedure:length>


; environment

'eval            : <procedure:eval>
'apply           : <procedure:apply>
'global          : <environment>
'get-env         : <procedure:get-env>
'get-curenv      : <procedure:get-curenv>
'lookup-symbol   : <procedure:lookup-symbol>

; syntax

'if              : <syntax:if>
'quote           : <syntax:quote>
'define          : <syntax:define>
'lambda          : <syntax:lambda>
'set!            : <syntax:set!>
'set-car!        : <syntax:set-car!>
'set-cdr!        : <syntax:set-cdr!>
'syntax-rules    : <syntax:syntax-rules>

; io, system

'gc              : <procedure:gc>
'exit            : <procedure:exit>
'read            : <procedure:read>
'source          : <procedure:source>
'system          : <procedure:system>
'display         : <procedure:display>
'flush-output    : <procedure:flush-output>

'open-in-port    : <procedure:open-in-port>
'open-out-port   : <procedure:open-out-port>
'fclose          : <procedure:fclose>

```

#### Library
```
'or              : <macro>
'and             : <macro>
'cond            : <macro>
'case            : <macro>
'let             : <macro>
'begin           : <macro>

'caar            : <closure>
'cadr            : <closure>
'cdar            : <closure>
'cddr            : <closure>
'caaar           : <closure>
'caadr           : <closure>
'cadar           : <closure>
'caddr           : <closure>
'cdaar           : <closure>
'cdadr           : <closure>
'cddar           : <closure>
'cdddr           : <closure>
'caaaar          : <closure>
'caaadr          : <closure>
'caadar          : <closure>
'caaddr          : <closure>
'cadaar          : <closure>
'cadadr          : <closure>
'caddar          : <closure>
'cadddr          : <closure>
'cdaaar          : <closure>
'cdaadr          : <closure>
'cdadar          : <closure>
'cdaddr          : <closure>
'cddaar          : <closure>
'cddadr          : <closure>
'cdddar          : <closure>
'cddddr          : <closure>

'map             : <closure>
'fold            : <closure>
'filter          : <closure>

'list            : <closure>
'range           : <closure>
'assoc           : <closure>
'append          : <closure>
'indexof         : <closure>

'odd?            : <closure>
'even?           : <closure>

'show            : <closure>
'print           : <closure>
'newline         : <closure>
```
