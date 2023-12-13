#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <ranges>
#include <string>
#include <vector>

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

  // Read input

  std::vector<std::string> az_grid;
  while (std::getline(file, az_grid.emplace_back()));

  while (az_grid.back().empty())
  {
    az_grid.pop_back();
  }

  std::vector<std::size_t> ai_empty_rows = range_to_vector<std::size_t>(
    std::views::iota(0ull, az_grid.size()) |
    std::views::filter(
      [&](std::size_t i_row)
      {
        const std::string& z_row = az_grid[i_row];
        return z_row.find('#') == std::string::npos;
      })
    );

  std::vector<std::size_t> ai_empty_cols = range_to_vector<std::size_t>(
    std::views::iota(0ull, az_grid[0].size()) |
    std::views::filter(
      [&](std::size_t i_col)
      {
        return std::ranges::all_of(
          az_grid,
          [&](const std::string& z_row)
          {
            return z_row[i_col] != '#';
          });
      })
    );

  std::map<std::size_t, std::size_t> ai_row_counts;
  std::map<std::size_t, std::size_t> ai_col_counts;
  for (const auto& [i_row, z_row] : az_grid | std::views::enumerate)
  {
    for (const auto& [i_col, c_space] : z_row | std::views::enumerate)
    {
      if (c_space == '#')
      {
        ++ai_row_counts[i_row];
        ++ai_col_counts[i_col];
      }
    }
  }

  // Process data

  auto x_get_distances =
    [](
      const std::map<std::size_t, std::size_t>& ai_counts,
      const std::vector<std::size_t>& ai_empty_indexes
      )
    {
      std::size_t i_result = 0;

      for (
        auto iter_first = ai_counts.begin();
        iter_first != ai_counts.end();
        ++iter_first
        )
      {
        const auto& [i_first_index, i_first_count] = *iter_first;

        for (
          auto iter_second = std::next(iter_first);
          iter_second != ai_counts.end();
          ++iter_second
          )
        {
          const auto& [i_second_index, i_second_count] = *iter_second;

          const std::size_t i_distance = i_second_index - i_first_index +
            (
              std::ranges::lower_bound(ai_empty_indexes, i_second_index) -
              std::ranges::lower_bound(ai_empty_indexes, i_first_index)
              ) * (1000000-1);
           i_result += i_distance * (i_first_count * i_second_count);
        }
      }

      return i_result;
    };

  std::size_t i_total_distance =
    x_get_distances(ai_row_counts, ai_empty_rows) +
    x_get_distances(ai_col_counts, ai_empty_cols);

  std::cout << i_total_distance << '\n';

  return 0;
}
