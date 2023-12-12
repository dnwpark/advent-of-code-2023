#include <algorithm>
#include <cmath>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <ranges>
#include <regex>
#include <string>
#include <vector>

int
main()
{
  std::ifstream file(SRC_DIR"/input.txt");

  // Setup

  auto x_get_numbers =
    [](const std::string& z_text)
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

      std::vector<int> ai_sorted_numbers(ai_numbers.begin(), ai_numbers.end());
      std::ranges::sort(ai_sorted_numbers);

      return ai_sorted_numbers;
    };

  // Process input

  int i_result = 0;

  std::deque<int> ai_upcoming_bonuses;
  std::string z_line;
  while (std::getline(file, z_line))
  {
    const std::size_t i_colon = z_line.find(':');
    const std::size_t i_bar = z_line.find('|');

    const std::vector<int> ai_winning_numbers =
      x_get_numbers(z_line.substr(i_colon+1, i_bar-i_colon));
    const std::vector<int> ai_my_numbers =
      x_get_numbers(z_line.substr(i_bar));
    std::vector<int> ai_matched_numbers;

    std::ranges::set_intersection(
      ai_winning_numbers,
      ai_my_numbers,
      std::back_inserter(ai_matched_numbers)
      );

    const int i_current_card_count = 1 +
      (ai_upcoming_bonuses.empty() ? 0 : ai_upcoming_bonuses.front());

    if (!ai_upcoming_bonuses.empty())
      ai_upcoming_bonuses.pop_front();

    const int i_bonus_count = ai_matched_numbers.size();

    i_result += i_current_card_count;

    for (
      int i_round_index = 0;
      i_round_index < i_bonus_count;
      ++i_round_index
      )
    {
      if (ai_upcoming_bonuses.size() <= std::size_t(i_round_index))
      {
        ai_upcoming_bonuses.push_back(i_current_card_count);
      }
      else
      {
        ai_upcoming_bonuses[i_round_index] += i_current_card_count;
      }
    }
  }

  std::cout << i_result << '\n';

  return 0;
}
