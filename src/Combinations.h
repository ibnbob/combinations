//
//      File     : Combinations.h
//      Abstract : Classes for enumerating or counting all m-element
//      subsets of an n-element set.
//

#ifndef COMBINATIONS_H
#define COMBINATIONS_H

#include <cassert>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace combinations {

//      Class    : Generator
//      Abstract : Class for enumerating all m-element subsets of an
//      n-element set. The original set is specified as a standard
//      vector of type T. The result is a vector of vectors of type
//      T. Type T must be copyable.
template <class T = int>
class Generator {
public:
  using Set = std::vector<T>;

  Generator(Set &set) :
    _set(set) {}; // CTOR
  ~Generator() = default; // DTOR

  void reserve(size_t sz) { _enumeration.reserve(sz); };
  void enumerate(size_t m);
  void enumerateIter(size_t m);

  std::vector<Set> &getEnumeration() { return _enumeration; };
  auto size() { return _enumeration.size(); };
  auto begin() { return _enumeration.begin(); };
  auto end() { return _enumeration.end(); };

  Generator(const Generator &) =
    delete; // Copy CTOR
  Generator &operator=(const Generator &) =
    delete; // Copy assignment
  Generator(Generator &&) =
    delete; // Move CTOR
  Generator &operator=(Generator &&) =
    delete; // Move assignment

 private:
  void enumerateRec(size_t curIdx, Set &curset);

  Set &_set;
  std::vector<Set> _enumeration;
  size_t _m;
}; // Generator


// Storing and hashing an (n,m) pair.
using CombPair = std::pair<size_t, size_t>;
auto CombPairHash = [](const CombPair &pair) {
  return pair.first ^ (pair.second<<1); };


//      Class    : Counter
//      Abstract : Class for counting all m-element sets of
class Counter {
public:
  Counter() :
    _counts(16, CombPairHash) {}; // CTOR
  ~Counter() = default; // DTOR

  size_t count(size_t n, size_t m);

  Counter(const Counter &) = delete; // Copy CTOR
  Counter &operator=(const Counter &) = delete; // Copy assignment
  Counter(Counter &&) = delete; // Move CTOR
  Counter &operator=(Counter &&) = delete; // Move assignment
private:
  size_t countRec(size_t n, size_t m);

  std::unordered_map<CombPair, size_t, decltype(CombPairHash)> _counts;
}; // Counter


//      Function : Generator<T>::enumerate
//      Abstract : Enumerate all m-element subsets of the set.
template <class T>
void
Generator<T>::enumerate(size_t m)
{
  _enumeration.clear();
  _enumeration.reserve(Counter().count(_set.size(), m));
  _m = m;
  Set curSet;
  curSet.reserve(m);
  enumerateRec(0, curSet);
} // Generator<T>::enumerate


//      Function : Generator<T>::enumerateRec
//      Abstract : Recursive enumeration.
template <class T>
void
Generator<T>::enumerateRec(size_t curIdx, Set &curSet)
{
  if (curSet.size() < _m) {
    curSet.push_back(_set[curIdx]);
    enumerateRec(curIdx+1, curSet);
    curSet.pop_back();
    if (curIdx + (_m-curSet.size()) < _set.size()) {
      enumerateRec(curIdx+1, curSet);
    } // if
  } else {
    assert(curSet.size() == _m);
    _enumeration.push_back(curSet);
  } // if
} // Generator<T>::enumerateRec


//      Function : Generator<T>::enumerateIter
//      Abstract : Iterative enumerator.
template <class T>
void
Generator<T>::enumerateIter(size_t m)
{
  _enumeration.clear();
  Set curSet;
  std::vector<size_t> idxStack;
  idxStack.push_back(0);
  std::vector<int> stateStack;
  stateStack.resize(_set.size(), 0);

  while (idxStack.size()) {
    size_t curIdx = idxStack.back();
    int state = stateStack[curIdx];
    if (state == 0) {
      if (curSet.size() < m) {
        curSet.push_back(_set[curIdx]);
        idxStack.push_back(curIdx+1);
        stateStack[curIdx] = 1;
      } else {
        assert(state == 0);
        _enumeration.push_back(curSet);
        idxStack.pop_back();
      } // if
    } else if (state == 1) {
      curSet.pop_back();
      if (curIdx + (m-curSet.size()) < _set.size()) {
        idxStack.push_back(curIdx+1);
        stateStack[curIdx] = 2;
      } else {
        idxStack.pop_back();
        stateStack[curIdx] = 0;
      } // if
    } else {
      assert(state == 2);
      idxStack.pop_back();
      stateStack[curIdx] = 0;
    } // if
  } // while
} // Generator<T>::enumerateIter


//      Function : Counter::count
//      Abstract : Return the number of combinations of m elements
//      from an n-element set. Throws an overflow error if an overflow
//      is detected.
inline size_t
Counter::count(const size_t n, size_t m)
{
  return countRec(n, m);
} // Counter::count


//      Function : Counter::countRec
//      Abstract : Return the number of combinations of m elements
//      from an n-element set. Computation is done using the recursive
//      formula C(n,m) = C(n-1,m) + C(n-1,m-1) so no factorials are
//      directly computed. Throws an overflow error if an overflow
//      is detected.
inline size_t
Counter::countRec(const size_t n, size_t m)
{
  m = std::min(m, n-m);
  if (m == 0) {
    return 1;
  } else if (m == 1) {
    return n;
  } else {
    CombPair pair(n,m);
    if (auto result = _counts.find(pair);
        result != _counts.end()) {
      return result->second;
    } else {
      size_t cnt0 = countRec(n-1, m);
      size_t cnt1 = countRec(n-1, m-1);
      size_t cnt = cnt0 + cnt1;
      if (cnt < (cnt0 | cnt1)) {
        throw std::overflow_error("Combination size overflowed.");
      } // if
      _counts[pair] = cnt;
      return cnt;
    } // if
  } // if
} // Counter::countRec


} // namespace combinations

#endif // COMBINATIONS_H
