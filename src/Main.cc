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
    set_default(1<<24);
  bool &iterative = flag("i,iterative", "use iterative generator.");
  bool &printp = flag("p,print", "print the combinations.");

  void prolog() override {
    std::cout << "Combination generator." << std::endl;
  } // prolog
}; // CombinationsArgs


//      Function : main
//      Abstract : Main driver.
int
main(int argc, char *argv[])
{
  auto args = argparse::parse<CombinationsArgs>(argc, argv);
  // args.print();
  size_t m = args.m;
  size_t n = args.n;

  try {
    size_t cnt(combinations::Counter().count(n, m));
    std::cout << "Count: " << cnt << std::endl;
    if (cnt <= args.limit) {
      std::vector<int> set;
      set.resize(n, 0);
      std::iota(set.begin(), set.end(), 0);
      combinations::Generator<int> generator(set);
      if (args.iterative) {
        generator.generateIter(m);
      } else {
        generator.generate(m);
      } // if

      if (args.printp) {
        for (auto comb : generator) {
          for (auto elem : comb) {
            std::cout << elem << " ";
          } // for each element
          std::cout << std::endl;
        } // for each combination
      } // if print
    } else {
      std::cout << "Number of subsets exceeds limit." << std::endl;
    } // if
  } catch(std::overflow_error &err) {
    std::cout << err.what() << std::endl;
  } // try/catch

  return 0;
} // main
