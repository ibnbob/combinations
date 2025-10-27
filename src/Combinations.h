//
//      File     : Combinations.h
//      Abstract : Classes for counting, generating or enumerating all
//      m-element subsets of an n-element set.
//

#ifndef COMBINATIONS_H
#define COMBINATIONS_H

#include <cassert>
#include <concepts>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace combinations {

//      Class    : Counter
//      Abstract : Class for counting all m-element sets of
class Counter {
public:
  Counter() :
    _counts(16) {}; // CTOR
  ~Counter() = default; // DTOR

  size_t count(size_t n, size_t m);

  Counter(const Counter &) = delete; // Copy CTOR
  Counter &operator=(const Counter &) = delete; // Copy assignment
  Counter(Counter &&) = delete; // Move CTOR
  Counter &operator=(Counter &&) = delete; // Move assignment
private:
  // Storing and hashing an (n,m) pair.
  using CombPair = std::pair<size_t, size_t>;
  struct CombPairHash {
    size_t operator()(const CombPair &pair) const {
      return pair.first ^ (pair.second<<1); };
  }; // CombPairHash

  size_t countRec(size_t n, size_t m);

  std::unordered_map<CombPair, size_t, CombPairHash> _counts;
}; // Counter


//      Class    : Enumerator
//      Abstract : Template class for enumerating m-element subsets
//      of an n-element set one at a time. This would normally be used
//      in a loop when random access to all of the combinations is
//      unnecessary. The original set is specified as a standard
//      vector of type T. Type T must be copy-constructible.
template <class T = int>
class Enumerator {
#if __cplusplus >= 202002L
  static_assert(std::copy_constructible<T>);
#endif
public:
  using Set = std::vector<T>;

  Enumerator(const Set &set) :
    _set(set), _m(0) {}; // CTOR
  ~Enumerator() = default; // DTOR

  Set first(size_t m);
  Set next();

  Enumerator(const Enumerator &) =
    delete; // Copy CTOR
  Enumerator &operator=(const Enumerator &) =
    delete; // Copy assignment
  Enumerator(Enumerator &&) =
    delete; // Move CTOR
  Enumerator &operator=(Enumerator &&) =
    delete; // Move assignment

 private:
  const Set &_set;
  size_t _m;
  Set _curSet;
  std::vector<size_t> _idxStack;
  std::vector<int> _stateStack;
}; // Enumerator


//      Class    : Lexor
//      Abstract : Template class for providing random access to
//      m-element subsets of n-element sets. The original set is
//      specified as a standard vector of type T. Type T must be
//      copy-constructible. Random access is by the ith m-element
//      subset based on lexicographical ordering. The first subset is
//      {0, 1, ..., m-1}. The last subset is {n-m, n-m+1, ..., n-1}.
template <class T = int>
class Lexor {
#if __cplusplus >= 202002L
  static_assert(std::copy_constructible<T>);
#endif
public:
  using Set = std::vector<T>;

  Lexor(const Set &set, const size_t m) :
    _set(set), _n(set.size()), _m(m) {}; // CTOR
  ~Lexor() = default; // DTOR

  void setM(size_t m);
  std::vector<T> get(size_t i, size_t m); // Sets m as side effect.
  std::vector<T> get(size_t i);

  Lexor(const Lexor &) = delete; // Copy CTOR
  Lexor &operator=(const Lexor &) = delete; // Copy assignment
  Lexor(Lexor &&) = delete; // Move CTOR
  Lexor &operator=(Lexor &&) = delete; // Move assignment
private:
  void get(size_t n,
           size_t m,
           size_t i,
           size_t nel,
           std::vector<T> &r);

  const Set &_set;
  size_t _n;
  size_t _m;
  Counter _counter;
}; // Lexor


//      Class    : Generator
//      Abstract : Template class for generating all m-element subsets
//      of an n-element set. The original set is specified as a
//      standard vector of type T. The result is a vector of vectors
//      of type T. This is memory intensive, but allows random access
//      to the combinations if needed. Type T must be copy-constructible.
template <class T = int>
class Generator {
#if __cplusplus >= 202002L
  static_assert(std::copy_constructible<T>);
#endif
public:
  using Set = std::vector<T>;

  Generator(const Set &set) :
    _set(set) {}; // CTOR
  ~Generator() = default; // DTOR

  void generate(size_t m);

  std::vector<Set> &getCombinations() { return _combinations; };
  auto size() { return _combinations.size(); };
  auto begin() { return _combinations.begin(); };
  auto end() { return _combinations.end(); };

  Generator(const Generator &) =
    delete; // Copy CTOR
  Generator &operator=(const Generator &) =
    delete; // Copy assignment
  Generator(Generator &&) =
    delete; // Move CTOR
  Generator &operator=(Generator &&) =
    delete; // Move assignment

 private:
  void generateRec(size_t curIdx, Set &curset);

  const Set &_set;
  std::vector<Set> _combinations;
  size_t _m;
}; // Generator


// Function definitions.


//      Function : Counter::count
//      Abstract : Return the number of combinations of m elements
//      from an n-element set. Throws an overflow error if an overflow
//      is detected.
inline size_t
Counter::count(const size_t n, const size_t m)
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


//      Function : Enumerator<T>::first
//      Abstract : Starts the enumerator and returns the first
//      combination. If m is zero, the null set is returned.
template <class T>
auto Enumerator<T>::first(const size_t m) -> Set
{
  _m = m;
  _curSet.clear();
  _idxStack.clear();
  _idxStack.push_back(0);
  _stateStack.clear();
  _stateStack.resize(_set.size()+1, 0);

  if (_m) {
    return next();
  } else {
    return _curSet;
  } // if
} // Enumerator<T>::first


//      Function : Enumerator<T>::next
//      Abstract : Returns the next combination. If there are no more
//      combinations, the null set is returned.
template <class T>
auto Enumerator<T>::next() -> Set
{
  while (_idxStack.size()) {
    size_t curIdx = _idxStack.back();
    int state = _stateStack[curIdx];
    if (state == 0) {
      if (_curSet.size() < _m) {
        _curSet.push_back(_set[curIdx]);
        _idxStack.push_back(curIdx+1);
        _stateStack[curIdx] = 1;
      } else {
        assert(state == 0);
        _idxStack.pop_back();
        return _curSet;
      } // if
    } else if (state == 1) {
      _curSet.pop_back();
      if (curIdx + (_m-_curSet.size()) < _set.size()) {
        _idxStack.push_back(curIdx+1);
        _stateStack[curIdx] = 2;
      } else {
        _idxStack.pop_back();
        _stateStack[curIdx] = 0;
      } // if
    } else {
      assert(state == 2);
      _idxStack.pop_back();
      _stateStack[curIdx] = 0;
    } // if
  } // while

  assert(_curSet.size() == 0);
  return _curSet;
} // Enumerator<T>::next


//      Function : Lexor::setM
//      Abstract : Sets the size of the subset for subsequent get() calls.
template <class T>
void
Lexor<T>::setM(const size_t m)
{
  _m = m;
} // Lexor::setM


//      Function : Lexor::get
//      Abstract : Get the i-th m-element subset of the n-element set
//      {0,...,n-1}. The first subset in the order is indexed by 0;
//      the last is C(n, m)-1. If i is out of range, then we return an
//      empty vector. This sets m for subsequent calls as a side effect.
template <class T>
std::vector<T>
Lexor<T>::get(const size_t i, const size_t m)
{
  _m = m;
  return get(i);
} // Lexor::get


//      Function : Lexor::get
//      Abstract : Get the i-th m-element subset of the n-element set
//      {0,...,n-1}. The first subset in the order is indexed by 0;
//      the last is C(n, m)-1. If i is out of range, then we return an
//      empty vector.
template <class T>
std::vector<T>
Lexor<T>::get(const size_t i)
{
  std::vector<T> result;
  if (i < _counter.count(_n, _m)) {
    get(_n, _m, i, 0, result);
  } // if
  return result;
} // Lexor::get


//      Function : Lexor::get
//      Abstract : Get the i-th m-element subset of the n-element set
//      {0,...,n-1}. Parameter i is then next element we are
//      considering adding to the returned subset. Parameter r is the
//      returned subset.
template <class T>
void
Lexor<T>::get(const size_t n,
              const size_t m,
              const size_t i,
              const size_t nel,
              std::vector<T> &r)
{
  if (m > 0) {
    assert(nel < _n);
    assert(i < _counter.count(n, m));
    auto elCnt = _counter.count(n-1, m-1);
    if (i < elCnt) {
      r.push_back(_set[nel]);
      get(n-1, m-1, i, nel+1, r);
    } else {
      get(n-1, m, i-elCnt, nel+1, r);
    } // if
  } // if
} // Lexor::get


//      Function : Generator<T>::generate
//      Abstract : Generate all m-element subsets of the set.
template <class T>
void
Generator<T>::generate(const size_t m)
{
  _combinations.clear();
  _combinations.reserve(Counter().count(_set.size(), m));
  _m = m;
  Set curSet;
  curSet.reserve(m);
  generateRec(0, curSet);
} // Generator<T>::generate


//      Function : Generator<T>::generateRec
//      Abstract : Recursive enumeration.
template <class T>
void
Generator<T>::generateRec(const size_t curIdx, Set &curSet)
{
  if (curSet.size() < _m) {
    curSet.push_back(_set[curIdx]);
    generateRec(curIdx+1, curSet);
    curSet.pop_back();
    if (curIdx + (_m-curSet.size()) < _set.size()) {
      generateRec(curIdx+1, curSet);
    } // if
  } else {
    assert(curSet.size() == _m);
    _combinations.push_back(curSet);
  } // if
} // Generator<T>::generateRec


} // namespace combinations

#endif // COMBINATIONS_H
