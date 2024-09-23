``` graphviz
digraph DFA0x7fffffffdd18 {
BEGIN_MARK -> 2 [label = "first"]
BEGIN_MARK [shape = none, label = ""]
1 [shape = doublecircle]
2 [shape = circle]
2 -> 1 [ label = a ]
}
```
``` graphviz
digraph DFA0x7fffffffda88 {
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
digraph DFA0x7fffffffd7f8 {
BEGIN_MARK -> 2 [label = "first"]
BEGIN_MARK [shape = none, label = ""]
1 [shape = doublecircle]
2 [shape = doublecircle]
1 -> 1 [ label = a ]
1 -> 1 [ label = b ]
2 -> 1 [ label = a ]
2 -> 1 [ label = b ]
}
```
``` graphviz
digraph DFA0x7fffffffd568 {
BEGIN_MARK -> 13 [label = "first"]
BEGIN_MARK [shape = none, label = ""]
1 [shape = doublecircle]
2 [shape = doublecircle]
3 [shape = doublecircle]
4 [shape = doublecircle]
5 [shape = doublecircle]
6 [shape = doublecircle]
7 [shape = doublecircle]
8 [shape = doublecircle]
9 [shape = doublecircle]
10 [shape = doublecircle]
11 [shape = doublecircle]
12 [shape = doublecircle]
13 [shape = circle]
2 -> 5 [ label = a ]
3 -> 5 [ label = a ]
3 -> 10 [ label = b ]
5 -> 8 [ label = a ]
2 -> 10 [ label = b ]
8 -> 8 [ label = a ]
10 -> 8 [ label = a ]
8 -> 2 [ label = b ]
10 -> 3 [ label = b ]
4 -> 5 [ label = a ]
5 -> 1 [ label = b ]
13 -> 12 [ label = b ]
12 -> 12 [ label = b ]
12 -> 9 [ label = a ]
11 -> 9 [ label = a ]
13 -> 11 [ label = a ]
11 -> 7 [ label = b ]
1 -> 5 [ label = a ]
7 -> 9 [ label = a ]
9 -> 6 [ label = b ]
6 -> 9 [ label = a ]
4 -> 10 [ label = b ]
9 -> 9 [ label = a ]
1 -> 10 [ label = b ]
7 -> 4 [ label = b ]
6 -> 10 [ label = b ]
}
```
