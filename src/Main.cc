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
  int &n = kwarg("n,n_size",
                 "size of set.").set_default(16);
  int &m = kwarg("m,m_size",
                 "size of subsets.").set_default(4);
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

  std::vector<int> set;
  set.resize(args.n, 0);
  std::iota(set.begin(), set.end(), 0);

  combinations::Generator cgen(set);
  cgen.enumerate(args.m);
  std::cout << "Number of cominations: "
            << cgen.size()
            << std::endl;

  if (args.printp) {
    for (auto comb : cgen) {
      for (auto elem : comb) {
        std::cout << elem << " ";
      } // for
      std::cout << std::endl;
    } // for
  } // if

  return 0;
} // main
