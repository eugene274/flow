#include <iostream>
#include <iomanip>
#include <chrono>

#include "CorrelationTask.h"

int main(int argc, char **argv) {
  std::cout << "go" << std::endl;
  auto start = std::chrono::system_clock::now();
  CorrelationTask st(argv[1], "tree");
  st.Run();
  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "elapsed time: " << elapsed_seconds.count() << " s\n";
  return 0;
}
