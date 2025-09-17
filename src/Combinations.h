//
//      File     : Combinations.h
//      Abstract : Classes for counting, generating or enumerating all
//      m-element subsets of an n-element set.
//

#ifndef COMBINATIONS_H
#define COMBINATIONS_H

#include <cassert>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace combinations {


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


//      Class    : Generator
//      Abstract : Template class for generating all m-element subsets
//      of an n-element set. The original set is specified as a
//      standard vector of type T. The result is a vector of vectors
//      of type T. This is memory intensive, but allows random access
//      to the combinations if needed. Type T must be copyable.
template <class T = int>
class Generator {
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


//      Class    : Enumerator
//      Abstract : Template class for enumerating m-element subsets
//      of an n-element set one at a time. This would normally be used
//      in a loop when random access to all of the combinations is
//      unnecessary. The original set is specified as a standard
//      vector of type T. Type T must be copyable.
template <class T = int>
class Enumerator {
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


//      Function : Generator<T>::generate
//      Abstract : Generate all m-element subsets of the set.
template <class T>
void
Generator<T>::generate(size_t m)
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
Generator<T>::generateRec(size_t curIdx, Set &curSet)
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


//      Function : Enumerator<T>::first
//      Abstract : Starts the enumerator and returns the first
//      combination. If m is zero, the null set is returned.
template <class T>
auto Enumerator<T>::first(size_t m) -> Set
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


} // namespace combinations

#endif // COMBINATIONS_H
