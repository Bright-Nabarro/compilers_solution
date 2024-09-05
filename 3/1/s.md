```cpp
float limitedSquare(x)
{
    float x;
    return ( x <= -10.0 || x >= 10.0 ) ? 100 : x * x;
}
```

```token
<float> <id, limitedSquare> <(> <id, x> <)>
<{>
    <float> <id, x> <;>
    <return> <(> <id, x> <comparison, `<=`> <number, -10.0> <link_op, ||>
            <id, x> <comparison, `>=`> <)> <?> <number, 100> <:> <id, x> <op, *> <id, x> <;>
<}>
```
