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

  const std::regex regex_node("(\\w+)\\W+(\\w+)\\W+(\\w+)\\W+");

  // Parse input

  std::string z_line;

  std::getline(file, z_line);
  std::vector<std::size_t> ai_turns;
  std::ranges::transform(
    z_line,
    std::back_inserter(ai_turns),
    [](char c_turn)
    {
      return c_turn == 'R';
    });

  std::unordered_map<std::string, std::vector<std::string>> aaz_map;
  while (std::getline(file, z_line))
  {
    std::smatch node_match;
    if (!std::regex_match(z_line, node_match, regex_node))
      continue;

    aaz_map.insert({
      node_match.str(1),
      {node_match.str(2), node_match.str(3)}
      });
  }

  // Process input

  auto ai_repeating_turns = std::views::repeat(ai_turns) | std::views::join;

  auto iter_position = aaz_map.find("AAA");
  auto iter_turn = ai_repeating_turns.begin();

  int i_result = 0;
  while (iter_position->first != "ZZZ")
  {
    iter_position = aaz_map.find(iter_position->second.at(*iter_turn));
    ++iter_turn;
    ++i_result;
  }

  std::cout << i_result << '\n';

  return 0;
}
