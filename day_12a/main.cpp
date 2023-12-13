#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <regex>
#include <string>
#include <vector>

std::vector<int>
get_numbers(const std::string& z_text)
{
  static const std::regex regex_number("\\d+");
  auto ai_numbers =
    std::ranges::subrange(
      std::sregex_iterator(z_text.begin(), z_text.end(), regex_number),
      std::sregex_iterator()
      ) |
    std::views::transform(
      [&](const auto& match){return std::stoi(match.str(0));}
      );

  return {ai_numbers.begin(), ai_numbers.end()};
}

template<typename T>
auto
range_to_vector(auto range)
{
  // std::ranges::to not supported yet
  std::vector<T> result;
  for (const auto& entry : range)
  {
      result.push_back(entry);
  }
  return result;
}

int
main()
{
  std::ifstream file(SRC_DIR"/input.txt");

  // Setup

  std::regex regex_broken("#+");

  // Process input

  int i_result = 0;

  std::string z_line;
  while (std::getline(file, z_line))
  {
    if (z_line.empty())
      continue;

    // Get gear counts
    const std::string z_gears = z_line.substr(0, z_line.find(' '));
    const std::vector<int> ai_numbers = get_numbers(z_line);

    const int i_broken_count = std::ranges::count(z_line, '#');

    auto a_unknown_indexes =
      std::views::iota(0ull, z_line.size()) |
      std::views::filter(
        [&](std::size_t i_line_index){return z_line[i_line_index] == '?';}
        ) |
      std::views::enumerate;
    const int i_unknown_count =
      std::ranges::count_if(a_unknown_indexes, [](const auto&){return true;});

    const int i_total_number =
      std::accumulate(ai_numbers.begin(), ai_numbers.end(), 0);

    const int i_unknown_broken = i_total_number - i_broken_count;
    const int i_unknown_fixed = i_unknown_count - i_unknown_broken;

    // Get gear permutations

    std::vector<std::string> az_unknowns;
    std::string z_unknowns =
      std::string(i_unknown_broken, '#') + std::string(i_unknown_fixed, '.');
    do
    {
      az_unknowns.push_back(z_unknowns);
    } while (std::next_permutation(z_unknowns.begin(), z_unknowns.end()));

    for (const std::string& z_unknowns : az_unknowns)
    {
      std::string z_effective_line = z_line;
      for (const auto& [i_unknown_index, i_line_index] : a_unknown_indexes)
      {
        z_effective_line[i_line_index] = z_unknowns[i_unknown_index];
      }

      std::vector<int> ai_broken_groups = range_to_vector<int>(
        std::ranges::subrange(
          std::sregex_iterator(
            z_effective_line.begin(),
            z_effective_line.end(),
            regex_broken
            ),
          std::sregex_iterator()
          ) |
        std::views::transform(
          [&](const auto& match)
          {
            return match.str(0).size();
          })
        );

      if (ai_broken_groups == ai_numbers)
      {
        ++i_result;
      }
    }
  }

  std::cout << i_result << '\n';

  return 0;
}
