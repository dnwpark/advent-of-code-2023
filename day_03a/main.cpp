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
  std::regex regex_symbol("[^0-9\\.]");

  // Parse input

  std::vector<NumberMap> aai_numbers;
  std::vector<std::vector<std::int64_t>> aai_symbol_indexes;

  aai_symbol_indexes.emplace_back(); // pad symbols with blank line

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

    // Collect indexes of symbols
    auto ai_symbol_indexes =
      std::ranges::subrange(
        std::sregex_iterator(z_line.begin(), z_line.end(), regex_symbol),
        std::sregex_iterator()
        ) |
      std::views::transform(
        [&](const auto& match){return match[0].first - z_line.begin();}
        );
    aai_symbol_indexes.emplace_back(
      ai_symbol_indexes.begin(),
      ai_symbol_indexes.end()
      );
  }

  aai_symbol_indexes.emplace_back(); // pad symbols with blank line

  // Calculate result

  int i_result = 0;

  for (const auto& [ai_numbers, aai_adjacent_symbol_rows] : std::views::zip(
      aai_numbers,
      aai_symbol_indexes | std::views::slide(3)
      )
    )
  {
    auto ai_numbers_adjacent_to_symbols =
      // For the prev, curr, and next rows
      aai_adjacent_symbol_rows |
      std::views::transform(
        [&](const auto& ai_symbols)
        {
          // For each symbol index
          return ai_symbols |
            std::views::transform([&](std::int64_t i_symbol_index)
            {
              // Gather all numbers adjacent to symbols
              auto [first, last] = ai_numbers.equal_range(i_symbol_index);
              return
                std::ranges::subrange(first, last) |
                std::views::transform(
                  [](const NumberMap::value_type& entry){return entry.second;}
                  )
                ;
            }) |
            std::views::join;
        }) |
      std::views::join;

    // Remove duplicates
    std::vector<NumberEntry> ai_part_numbers =
      range_to_vector<NumberEntry>(ai_numbers_adjacent_to_symbols);
    std::ranges::sort(ai_part_numbers);
    ai_part_numbers.erase(
      std::unique(ai_part_numbers.begin(), ai_part_numbers.end()),
      ai_part_numbers.end()
      );

    auto ai_number_values =
      ai_part_numbers |
      std::views::transform(
        [](const NumberEntry& entry){return std::get<1>(entry);}
        )
      ;
    i_result += std::accumulate(
      ai_number_values.begin(),
      ai_number_values.end(),
      0
      );
  }

  std::cout << i_result << '\n';

  return 0;
}
