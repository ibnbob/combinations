[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
# Combinations.h

[Combinations.h](src/Cominations.h) is a header-only implementation 
of classes for counting or generating all _m_-element subsets of an
_n_-element set. It provides two classes, `Counter` and `Generator`, 
for counting and generating combinations. Both classes exist within 
the `combinations` namespace.

## `Counter` Class
The _Counter_ class uses the recurrence
```math
\left( \begin{array}{c} n \\ m \end{array} \right) =
\left( \begin{array}{c} n-1 \\ m-1 \end{array} \right) +
\left( \begin{array}{c} n-1 \\ m \end{array} \right)
```
with the terminating conditions 
```math
\left( \begin{array}{c} n \\ 1 \end{array} \right) = n
,
\left( \begin{array}{c} n \\ 0 \end{array} \right) = 1.
```
The class memoizes intermediate results so that the running time is $O(n \cdot m)$.
This is worse than the straightforward computation using factorials but has the 
advantage of not overflowing during the computation if the result does not overflow.
The result is returned as `size_t`. 
