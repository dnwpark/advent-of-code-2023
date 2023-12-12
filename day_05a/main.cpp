#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ranges>
#include <regex>
#include <string>
#include <unordered_map>

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

std::vector<std::int64_t>
get_numbers(const std::string& z_text)
{
  static const std::regex regex_number("\\d+");
  auto ai_numbers =
    std::ranges::subrange(
      std::sregex_iterator(z_text.begin(), z_text.end(), regex_number),
      std::sregex_iterator()
      ) |
    std::views::transform(
      [&](const auto& match){return std::stoll(match.str(0));}
      );

  return {ai_numbers.begin(), ai_numbers.end()};
}

struct MapEntry
{
  std::int64_t i_in_begin;
  std::int64_t i_out_begin;
  std::int64_t i_length;
};

bool
matches_map_entry(const MapEntry& map_entry, std::int64_t i_value)
{
  return
    map_entry.i_in_begin <= i_value &&
    i_value < map_entry.i_in_begin + map_entry.i_length;
};

std::int64_t
apply_map_entry(const MapEntry& map_entry, std::int64_t i_value)
{
  return i_value - map_entry.i_in_begin + map_entry.i_out_begin;
};

std::int64_t
apply_map(const std::vector<MapEntry>& a_map, std::int64_t i_value)
{
  for (const MapEntry& map_entry : a_map)
  {
    if (matches_map_entry(map_entry, i_value))
    {
      return apply_map_entry(map_entry, i_value);
    }
  }
  return i_value;
};

int
main()
{
  std::ifstream file(SRC_DIR"/input.txt");

  // Process input

  std::string z_line;

  std::getline(file, z_line);
  const std::vector<std::int64_t> ai_seeds = get_numbers(z_line);
  std::getline(file, z_line);

  std::vector<std::vector<MapEntry>> aai_maps(1);
  while (std::getline(file, z_line))
  {
    if (z_line.empty())
    {
      aai_maps.emplace_back();
    }
    else
    {
      const std::vector<std::int64_t> ai_line = get_numbers(z_line);
      if (!ai_line.empty())
      {
        aai_maps.back().push_back({ai_line[1], ai_line[0], ai_line[2]});
      }
    }
  }

  auto ai_locations_range =
    ai_seeds |
    std::views::transform(
      [&](std::int64_t i_value)
      {
        for (const auto& ai_map : aai_maps)
        {
          i_value = apply_map(ai_map, i_value);
        }
        return i_value;
      });
  const std::vector<std::int64_t> ai_locations =
    range_to_vector<std::int64_t>(ai_locations_range);

  std::cout << *std::ranges::min_element(ai_locations) << '\n';

  return 0;
}
