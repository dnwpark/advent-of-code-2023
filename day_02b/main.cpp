#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <regex>
#include <string>
#include <unordered_map>

int
main()
{
  std::ifstream file(SRC_DIR"/input.txt");

  // Setup

  std::regex regex_colour("(\\d+) (red|green|blue)");

  // Process input

  int i_result = 0;

  int i_game_id = 0;
  std::string z_line;
  while (std::getline(file, z_line))
  {
    i_game_id += 1;

    auto a_colour_matches = std::ranges::subrange(
      std::sregex_iterator(z_line.begin(), z_line.end(), regex_colour),
      std::sregex_iterator()
      );

    std::unordered_map<std::string,int> ai_max_numbers;
    for (const auto& match : a_colour_matches)
    {
      const int i_number = std::stoi(match.str(1));
      const std::string z_colour = match.str(2);

      int& i_old_number = ai_max_numbers[z_colour];
      i_old_number = std::max(i_old_number, i_number);
    }

    i_result += std::accumulate(ai_max_numbers.begin(), ai_max_numbers.end(), 1,
      [](int lhs, const auto& rhs)
      {
        return lhs * rhs.second;
      });
  }

  std::cout << i_result << '\n';

  return 0;
}
