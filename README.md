[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
# Combinations.h

[Combinations.h](src/Combinations.h) is a header-only implementation of
classes for counting, enumerating or generating _m_-element subsets of an
_n_-element set. It provides four classes, `Counter`, `Enumerator`, `Lexor`
and `Generator`. All classes exist within the `combinations` namespace.

## `Counter` Class
The `Counter` class has a single member function 
`Counter::size_t count(size_t n, size_t m);`
that computes the number of _m_-element subsets of an _n_-element set.
It uses the recurrence
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
as long as the result can be represent by `size_t`. Also, subsequent calls
reuse the results of previous calls which can make the amortized running time
approach $O(1)$. The function throws `std::overflow_error` if it detects an
overflow.

## `Enumerator` Class
The `Enumerator` class is a template class:
```
template <class T = int> class Enumerator;
```
It enumerates all _m_-element subsets of an _n_-element set in lexicographical
order. One specifies original set as `std::vector<T>`. Type `T` must be
copyable. The method `Enumerator::start(size_t m)` starts the enumeration and
returns the lexicographically first combination also as `std::vector<T>`. The
method `Enumerator::next()` returns the next combination in lexicographical
order or an empty vector when we reach the end of the enumeration.

## `Lexor` Class
The `Lexor` class is a template class
```
template <class T = int> class Lexor;
```
It provides random access to _m_-element subsets of _n_-element sets based
on lexicographical order using the method `Lexor::get(size_t )`. We assume
the n-element set to be the set of natural numbers ${0, 1, ...,  n-1}$.
Random access is by the _ith_ _m_-element subset based on lexicographical
ordering. The first subset, with index 0, is ${0, 1, ..., m-1}$. The last
subset is ${n-m, n-m+1, ..., n-1}$. The result is returned as
`std::vector<T>`.

If one intends to process all subsets in order, then the `Enumerator` class
(_v.s._) is slightly more efficient.

## `Generator` Class
The `Generator` class is a template class:
```
template <class T = int> class Generator;
```
It generates all _m_-element subsets of an _n_-element set and keeps them in
memory. The original set is specified as `std::vector<T>`. The result is  
`std::vector< std::vector<T> >`. Type `T` must be copyable. Since all
combinations are present in memory at the same time, this can be a
memory-intensive class. This class should only be used for small sets when
fast random access of the combinations is required.

## Usage
See [Main.cc](src/Main.cc) for an example of the usage of all classes.
