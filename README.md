[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
# Combinations.h

[Combinations.h](src/Combinations.h) is a header-only implementation 
of classes for counting or generating all _m_-element subsets of an
_n_-element set. It provides two classes, `Counter` and `Generator`, 
for counting and generating combinations. Both classes exist within 
the `combinations` namespace.

## `Counter` Class
The `Counter` class has a member function 
`Counter::size_t count(size_t n, size_t m);`
that computes the number of _m_-element subsets of an _n_-element set
using the recurrence
```math
\left( \begin{array}{c} n \\ m \end{array} \right) =
\left( \begin{array}{c} n-1 \\ m-1 \end{array} \right) +
\left( \begin{array}{c} n-1 \\ m \end{array} \right)
```
with the terminating conditions 
```math
\left( \begin{array}{c} n \\ 1 \end{array} \right) = n
\text{\; and }
\left( \begin{array}{c} n \\ 0 \end{array} \right) = 1
```
along with the identity
```math
\left( \begin{array}{c} n \\ m \end{array} \right) =
\left( \begin{array}{c} n \\ n-m \end{array} \right) \text{.}
```
The class memoizes intermediate results so that the running time of `count` 
is $O(n \cdot m)$. This is worse than the straightforward computation using 
factorials but has the advantage of not overflowing during the computation 
if the result can be represent by `size_t`. Also, subsequent calls can reuse 
the results of previous calls. The functiont throws `std::overflow_error` if
an overflow is detected.

## `Generator` Class
The `Generator` class is a template class:
```
template <class T = int> class Generator;
```
It generates all _m_-element subsets of an _n_-element set. The original set is
specified as `std::vector<T>`. The result is  
`std::vector< std::vector<T> >`.
Type `T` must be copyable.

## Usage
See [Main.cc](src/Main.cc) for an example of the useage of both `Counter` 
and `Generator`.




