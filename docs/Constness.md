### Handling `const` in `Any++` 

| OP/FN_            | OP/FN_ EXACT        |  Jacket   | Proxy     |  callable?            |
|:-----------------:|:-------------------:|:---------:|:---------:|:---------------------:|
| `mutable`         | *                   | `const`   | *         | no                    |
| `mutable`         | `EXACT`             | `mutable` | `mutable` | yes                   |
| `mutable`         | _                   | `mutable` | `mutable` | yes                   |
| `mutable`         | *                   | *         | `const`   | no                    |
| `const`           | _                   | *         | *         | yes                   |
| `const`           | `EXACT`             | *         | `mutable` | no                    |
| `const`           | `EXACT`             | *         | `const`   | yes                   |
