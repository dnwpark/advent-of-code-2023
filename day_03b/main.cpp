#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <regex>
#include <string>
#include <unordered_map>
#include <set>

using NumberEntry = std::tuple<std::int64_t, int>;
using NumberMap = std::unordered_multimap<std::int64_t, NumberEntry>;

template<typename T_Key, typename T_Mapped>
auto
range_to_multimap(auto& range)
{
  // std::ranges::to not supported yet
  std::unordered_multimap<T_Key, T_Mapped> result;
  for (const auto& entry : range)
  {
      result.insert(entry);
  }
  return result;
}

template<typename T>
auto
range_to_vector(auto& range)
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

  std::regex regex_number("\\d+");
  std::regex regex_gear("[\\*]");

  // Parse input

  std::vector<NumberMap> aai_numbers;
  std::vector<std::vector<std::int64_t>> aai_gear_indexes;

  aai_numbers.emplace_back(); // pad numbers with blank line

  std::string z_line;
  while (std::getline(file, z_line))
  {
    // Collect the numbers in each row adjacent to each index
    // Uniquely identify numbers by combining with start index
    //   (In case a line contains duplicate numbers)
    auto ai_numbers =
      std::ranges::subrange(
        std::sregex_iterator(z_line.begin(), z_line.end(), regex_number),
        std::sregex_iterator()
        ) |
      std::views::transform(
        [&](const auto& match)
        {
          const int i_value = std::stoi(match.str(0));
          const std::int64_t i_begin = match[0].first - z_line.begin();
          const std::int64_t i_end = match[0].second - z_line.begin();

          return std::views::zip(
            // key = every possible connected symbol index
            std::views::iota(i_begin-1, i_end+1),
            std::views::zip(
              // mapped = tuple of start index and number value
              std::ranges::repeat_view(i_begin),
              std::ranges::repeat_view(i_value)
              )
            );
        }) |
      std::views::join;
    aai_numbers.push_back(
      range_to_multimap<std::int64_t, NumberEntry>(ai_numbers)
      );

    // Collect indexes of gears
    auto ai_gear_indexes =
      std::ranges::subrange(
        std::sregex_iterator(z_line.begin(), z_line.end(), regex_gear),
        std::sregex_iterator()
        ) |
      std::views::transform(
        [&](const auto& match){return match[0].first - z_line.begin();}
        );
    aai_gear_indexes.emplace_back(
      ai_gear_indexes.begin(),
      ai_gear_indexes.end()
      );
  }

  aai_numbers.emplace_back(); // pad numbers with blank line

  // Calculate result

  int i_result = 0;

  for (const auto& [ai_gear_indexes, aai_adjacent_number_rows] : std::views::zip(
      aai_gear_indexes,
      aai_numbers | std::views::slide(3)
      )
    )
  {
    auto ai_gear_ratios =
      // For each gear index
      ai_gear_indexes |
      std::views::transform(
        [&](const int i_gear_indexes)
        {
          // For the prev, curr, and next rows
          auto ai_adjacent_numbers = aai_adjacent_number_rows |
            std::views::transform(
              [&](const auto& ai_number_row)
              {
                // Gather all numbers adjacent to the gear
                auto [first, last] = ai_number_row.equal_range(i_gear_indexes);
                return
                  std::ranges::subrange(first, last) |
                  std::views::transform(
                    [](const NumberMap::value_type& entry)
                    {
                      return std::get<1>(entry.second);
                    })
                  ;
              }) |
            std::views::join;
          return range_to_vector<int>(ai_adjacent_numbers);
        }) |
        std::views::filter(
          // Only count gears with 2 adjacent numbers
          [](const std::vector<int>& ai_adjacent_numbers)
          {
            return ai_adjacent_numbers.size() == 2;
          }) |
        std::views::transform(
          // Calculate gear ratio
          [](const std::vector<int>& ai_adjacent_numbers)
          {
            return ai_adjacent_numbers[0] * ai_adjacent_numbers[1];
          });

    i_result += std::accumulate(
      ai_gear_ratios.begin(),
      ai_gear_ratios.end(),
      0
      );
  }

  std::cout << i_result << '\n';

  return 0;
}
