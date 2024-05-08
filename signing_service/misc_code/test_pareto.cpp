// #include "startup.hpp"

// int main(int argc, char** argv) {
//   create_and_fill_database("test.db", 2);

// }

#include <cmath> //std::exp
#include <random>
#include <algorithm>
#include <iostream>

#include "common.hpp"
#include "utils.hpp"

int main() {
  auto pareto = [](auto shape) {
    return [shape]() {
      //see https://en.wikipedia.org/wiki/Pareto_distribution#Relation_to_the_exponential_distribution
      static auto gen = std::mt19937(std::random_device()());
      static auto exp_dist = std::exponential_distribution<>(shape);
      return std::exp(exp_dist(gen));
    };
  };

  auto const count = 100000;
  //reality is fat-tailed and does not owe you even an expected value
  //const auto shape = 1.0;
  const auto shape = std::log(5) / std::log(4); //rate parameter for 80:20
  auto samples = make_vector(count, pareto(shape));

  std::sort(std::begin(samples), std::end(samples));
  // for (auto & sample : samples)
  //   std::cout << sample << "\n";
  // std::cout.flush();
  
  auto const cutoff = std::begin(samples) + count*4/5;
  auto const bottom = std::reduce(std::begin(samples), cutoff);
  auto const top = std::reduce(cutoff, std::end(samples));

  for (size_t i = 0; i < 3; ++i)
    std::cout << samples[i] << "\n";
  std::cout << "...\n";
  for (size_t i = 3; i != 0; --i)
    std::cout << samples[count-i] << "\n";
  std::cout.flush();

  auto total = top + bottom;
  std::cout <<
      "sum bottom 80 %: " << bottom <<
    "\nsum    top 20 %: " << top <<
    "\n          total: " << total <<
    "\n    top / total: " << 100*top/(top+bottom) << " %" << std::endl;
}