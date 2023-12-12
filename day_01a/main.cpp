#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>

int
main()
{
  std::ifstream file(SRC_DIR"/input.txt");

  // Process input

  int i_result = 0;

  std::string z_line;
  while (std::getline(file, z_line))
  {
    auto iter_first = std::find_if(z_line.cbegin(), z_line.cend(),
      [](char c)
      {
        return std::isdigit(c) != 0;
      });
    auto iter_last = std::find_if(z_line.crbegin(), z_line.crend(),
      [](char c)
      {
        return std::isdigit(c) != 0;
      });

    if (iter_first == z_line.cend() || iter_last == z_line.crend())
      continue;

    i_result += (*iter_first - 48)*10 + (*iter_last - 48);
  }

  std::cout << i_result << '\n';

  return 0;
}
