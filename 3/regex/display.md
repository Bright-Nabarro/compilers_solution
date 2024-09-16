`((a|b)*abb(a|b)*)`
``` graphviz
digraph SyntaxTree0x7fffffffe060 {
node [shape=plaintext]
"cat0" [label = "•"]
"cat0" -> "cat1"
"cat1" [label = "•"]
"cat0" -> "# 0"
"# 0" [label = "# 0"]
"cat1" -> "cat2"
"cat2" [label = "•"]
"cat1" -> "star0"
"star0" [label = "*"]
"cat2" -> "cat3"
"cat3" [label = "•"]
"cat2" -> "b 1"
"b 1" [label = "b 1"]
"cat3" -> "cat4"
"cat4" [label = "•"]
"cat3" -> "b 2"
"b 2" [label = "b 2"]
"cat4" -> "star1"
"star1" [label = "*"]
"cat4" -> "a 3"
"a 3" [label = "a 3"]
"star1" -> "or0"
"or0" [label = "|"]
"or0" -> "a 4"
"a 4" [label = "a 4"]
"or0" -> "b 5"
"b 5" [label = "b 5"]
"star0" -> "or1"
"or1" [label = "|"]
"or1" -> "a 6"
"a 6" [label = "a 6"]
"or1" -> "b 7"
"b 7" [label = "b 7"]
}
```

``
``` graphviz
digraph SyntaxTree0x7fffffffe060 {
node [shape=plaintext]
}
```

`a`
``` graphviz
digraph SyntaxTree0x7fffffffe060 {
node [shape=plaintext]
"cat0" [label = "•"]
"cat0" -> "a 0"
"a 0" [label = "a 0"]
"cat0" -> "# 1"
"# 1" [label = "# 1"]
}
```

`abc`
``` graphviz
digraph SyntaxTree0x7fffffffe060 {
node [shape=plaintext]
"cat0" [label = "•"]
"cat0" -> "cat1"
"cat1" [label = "•"]
"cat0" -> "# 0"
"# 0" [label = "# 0"]
"cat1" -> "cat2"
"cat2" [label = "•"]
"cat1" -> "c 1"
"c 1" [label = "c 1"]
"cat2" -> "a 2"
"a 2" [label = "a 2"]
"cat2" -> "b 3"
"b 3" [label = "b 3"]
}
```

`(a|b)a*`
``` graphviz
digraph SyntaxTree0x7fffffffe060 {
node [shape=plaintext]
"cat0" [label = "•"]
"cat0" -> "cat1"
"cat1" [label = "•"]
"cat0" -> "# 0"
"# 0" [label = "# 0"]
"cat1" -> "or0"
"or0" [label = "|"]
"cat1" -> "star0"
"star0" [label = "*"]
"or0" -> "a 1"
"a 1" [label = "a 1"]
"or0" -> "b 2"
"b 2" [label = "b 2"]
"star0" -> "a 3"
"a 3" [label = "a 3"]
}
```

`(lin)*17*(bright|n)`
``` graphviz
digraph SyntaxTree0x7fffffffe060 {
node [shape=plaintext]
"cat0" [label = "•"]
"cat0" -> "cat1"
"cat1" [label = "•"]
"cat0" -> "# 0"
"# 0" [label = "# 0"]
"cat1" -> "cat2"
"cat2" [label = "•"]
"cat1" -> "or0"
"or0" [label = "|"]
"cat2" -> "cat3"
"cat3" [label = "•"]
"cat2" -> "star0"
"star0" [label = "*"]
"cat3" -> "star1"
"star1" [label = "*"]
"cat3" -> "1 1"
"1 1" [label = "1 1"]
"star1" -> "cat4"
"cat4" [label = "•"]
"cat4" -> "cat5"
"cat5" [label = "•"]
"cat4" -> "n 2"
"n 2" [label = "n 2"]
"cat5" -> "l 3"
"l 3" [label = "l 3"]
"cat5" -> "i 4"
"i 4" [label = "i 4"]
"star0" -> "7 5"
"7 5" [label = "7 5"]
"or0" -> "cat6"
"cat6" [label = "•"]
"or0" -> "n 6"
"n 6" [label = "n 6"]
"cat6" -> "cat7"
"cat7" [label = "•"]
"cat6" -> "t 7"
"t 7" [label = "t 7"]
"cat7" -> "cat8"
"cat8" [label = "•"]
"cat7" -> "h 8"
"h 8" [label = "h 8"]
"cat8" -> "cat9"
"cat9" [label = "•"]
"cat8" -> "g 9"
"g 9" [label = "g 9"]
"cat9" -> "cat10"
"cat10" [label = "•"]
"cat9" -> "i 10"
"i 10" [label = "i 10"]
"cat10" -> "b 11"
"b 11" [label = "b 11"]
"cat10" -> "r 12"
"r 12" [label = "r 12"]
}
```

