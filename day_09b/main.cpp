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
  static const std::regex regex_number("-?\\d+");
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

std::vector<int>
get_differences(const std::vector<int>& ai_numbers)
{
  auto ai_differences =
    ai_numbers |
    std::views::adjacent_transform<2>([&](int i_lhs, int i_rhs)
    {
      return i_rhs - i_lhs;
    });
  return {ai_differences.begin(), ai_differences.end()};
}

int
extrapolate(const std::vector<int>& ai_numbers)
{
  std::vector<std::vector<int>> aai_differences;
  aai_differences.push_back(get_differences(ai_numbers));

  while (
    !std::ranges::all_of(
      aai_differences.back(),
      [](int i_difference){return i_difference == 0;}
      ))
  {
    aai_differences.push_back(get_differences(aai_differences.back()));
  }

  auto ai_signs =
    std::views::repeat(std::vector<int>{-1, 1}) |
    std::views::join;

  int i_result = ai_numbers.front();
  for (
    const auto& [i_sign, ai_differences] :
    std::views::zip(ai_signs, aai_differences)
    )
  {
    i_result += i_sign * ai_differences.front();
  }

  return i_result;
}

int
main()
{
  std::ifstream file(SRC_DIR"/input.txt");

  // Process input

  int i_result = 0;

  std::string z_line;
  while (std::getline(file, z_line))
  {
    i_result += extrapolate(get_numbers(z_line));
  }

  std::cout << i_result << '\n';

  return 0;
}
