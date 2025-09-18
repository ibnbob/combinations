//
//      File     : Main.cc
//      Abstract : Driver
//

#include "Args.h"

#include <Combinations.h>

#include <iostream>
#include <numeric>
#include <string>


//      Struct   : CombinationsArgs
//      Abstract :
struct CombinationsArgs : public argparse::Args {
  size_t &n = kwarg("n,n_size",
                 "size of set.").set_default(16);
  size_t &m = kwarg("m,m_size",
                 "size of subsets.").set_default(4);
  size_t &limit = kwarg("l,limit", "subset limit.").
    set_default(1<<27);
  bool &enumerate = flag("e,enumerate", "use enumerator instead of generator.");
  bool &printp = flag("p,print", "print the combinations.");

  void prolog() override {
    std::cout << "Combination generator." << std::endl;
  } // prolog
}; // CombinationsArgs


//      Function : testEnumerate
//      Abstract : Exercise the enumeration class.
size_t
testEnumerate(size_t n, size_t m, bool printp)
{
  std::vector<int> set;
  set.resize(n, 0);
  std::iota(set.begin(), set.end(), 0);
  combinations::Enumerator<int> enumerator(set);
  size_t cnt2 = 0;
  for (auto comb = enumerator.first(m);
       comb.size();
       comb = enumerator.next()) {
    if (printp) {
      for (auto elem : comb) {
        std::cout << elem << " ";
      } // for each element
      std::cout << std::endl;
    } // if
    ++cnt2;
  } // for

  return cnt2;
} // testEnumerate


//      Function : testGenerate
//      Abstract :
size_t
testGenerate(size_t n, size_t m, bool printp)
{
  std::vector<int> set;
  set.resize(n, 0);
  std::iota(set.begin(), set.end(), 0);
  combinations::Generator<int> generator(set);
  generator.generate(m);

  if (printp) {
    for (auto comb : generator) {
      for (auto elem : comb) {
        std::cout << elem << " ";
      } // for each element
      std::cout << std::endl;
    } // for each combination
  } // if print

  return generator.size();
} // testGenerate


//      Function : main
//      Abstract : Main driver.
int
main(int argc, char *argv[])
{
  auto args = argparse::parse<CombinationsArgs>(argc, argv);
  args.print();

  size_t m = args.m;
  size_t n = args.n;

  try {
    size_t cnt(combinations::Counter().count(n, m));
    std::cout << "Count: " << cnt << std::endl;
    if (cnt <= args.limit) {
      if (args.enumerate) {
        size_t cnt2 = testEnumerate(n, m, args.printp);
        if (cnt != cnt2) {
          std::cout << "Enumerator count is incorrect!" << std::endl;
        } // if
      } else {
        size_t cnt2 = testGenerate(n, m, args.printp);
        if (cnt != cnt2) {
          std::cout << "Generator count is incorrect!" << std::endl;
        } // if
      } // if
    } else {
      std::cout << "Number of subsets exceeds limit." << std::endl;
    } // if
  } catch(std::overflow_error &err) {
    std::cout << err.what() << std::endl;
  } // try/catch

  return 0;
} // main
