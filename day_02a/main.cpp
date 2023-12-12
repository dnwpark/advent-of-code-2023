#include <algorithm>
#include <fstream>
#include <iostream>
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

  const std::unordered_map<std::string,int> ai_max_numbers{
    {"red", 12},
    {"green", 13},
    {"blue", 14},
    };
  auto x_number_and_colour_ok =
    [&](const auto& match)
    {
      const int i_number = std::stoi(match.str(1));
      const std::string z_colour = match.str(2);

      auto iter = ai_max_numbers.find(z_colour);
      if (iter == ai_max_numbers.end())
        return false;

      return i_number <= iter->second;
    };

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

    if (std::ranges::all_of(a_colour_matches, x_number_and_colour_ok))
    {
      i_result += i_game_id;
    }
  }

  std::cout << i_result << '\n';

  return 0;
}
