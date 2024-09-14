``` graphviz
digraph DFA0x7ffd22340530 {
BEGIN_MARK -> 2 [label = "first"]
BEGIN_MARK [shape = none, label = ""]
1 [shape = doublecircle]
2 [shape = circle]
2 -> 1 [ label = a ]
}
```
``` graphviz
digraph DFA0x7ffd22340530 {
BEGIN_MARK -> 3 [label = "first"]
BEGIN_MARK [shape = none, label = ""]
1 [shape = doublecircle]
2 [shape = circle]
3 [shape = circle]
2 -> 1 [ label = b ]
3 -> 2 [ label = a ]
}
```
``` graphviz
digraph DFA0x7ffd22340530 {
BEGIN_MARK -> 1 [label = "first"]
BEGIN_MARK [shape = none, label = ""]
1 [shape = doublecircle]
1 -> 1 [ label = a ]
1 -> 1 [ label = b ]
}
```
``` graphviz
digraph DFA0x7ffd22340530 {
BEGIN_MARK -> 7 [label = "first"]
BEGIN_MARK [shape = none, label = ""]
1 [shape = doublecircle]
2 [shape = doublecircle]
3 [shape = doublecircle]
4 [shape = doublecircle]
5 [shape = doublecircle]
6 [shape = doublecircle]
7 [shape = circle]
7 -> 5 [ label = a ]
5 -> 4 [ label = b ]
1 -> 2 [ label = a ]
6 -> 5 [ label = a ]
7 -> 6 [ label = b ]
5 -> 5 [ label = a ]
4 -> 3 [ label = b ]
3 -> 3 [ label = b ]
4 -> 5 [ label = a ]
3 -> 2 [ label = a ]
2 -> 1 [ label = b ]
2 -> 2 [ label = a ]
6 -> 6 [ label = b ]
1 -> 3 [ label = b ]
}
```
