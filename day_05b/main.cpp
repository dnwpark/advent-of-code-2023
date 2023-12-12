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

struct NumberRange
{
  std::int64_t i_begin;
  std::int64_t i_length;

  auto operator<=>(const NumberRange&) const = default;
};

std::vector<NumberRange>
get_number_ranges(const std::string& z_text)
{
  static const std::regex regex_number("(\\d+)\\s+(\\d+)");
  auto a_number_ranges =
    std::ranges::subrange(
      std::sregex_iterator(z_text.begin(), z_text.end(), regex_number),
      std::sregex_iterator()
      ) |
    std::views::transform(
      [&](const auto& match)
      {
        return NumberRange{std::stoll(match.str(1)), std::stoll(match.str(2))};
      }
      );

  return {a_number_ranges.begin(), a_number_ranges.end()};
}

struct MapEntry
{
  std::int64_t i_in_begin;
  std::int64_t i_out_begin;
  std::int64_t i_length;
};

std::int64_t
apply_map_entry(const MapEntry& map_entry, std::int64_t i_value)
{
  return i_value - map_entry.i_in_begin + map_entry.i_out_begin;
};

std::tuple<std::vector<NumberRange>, std::vector<NumberRange>>
apply_map_entry(const MapEntry& map_entry, const NumberRange& number_range)
{

  const std::int64_t i_map_entry_in_end =
    map_entry.i_in_begin + map_entry.i_length;
  const std::int64_t i_number_range_end =
    number_range.i_begin + number_range.i_length;

  if (
    i_number_range_end <= map_entry.i_in_begin ||
    i_map_entry_in_end <= number_range.i_begin
    )
  {
    // no changes
    return {{}, {number_range}};
  }

  const std::int64_t i_changed_begin = std::max(
    number_range.i_begin,
    map_entry.i_in_begin
    );
  const std::int64_t i_changed_end = std::min(
    i_number_range_end,
    i_map_entry_in_end
    );

  std::vector<NumberRange> a_changed;
  std::vector<NumberRange> a_unchanged;

  if (number_range.i_begin < i_changed_begin)
  {
    a_unchanged.push_back({
      number_range.i_begin,
      i_changed_begin - number_range.i_begin
      });
  }

  a_changed.push_back({
    apply_map_entry(map_entry, i_changed_begin),
    i_changed_end - i_changed_begin
    });

  if (i_changed_end < i_number_range_end)
  {
    a_unchanged.push_back({
      i_changed_end,
      i_number_range_end - i_changed_end
      });
  }

  return {a_changed, a_unchanged};
};

std::vector<NumberRange>
apply_map(
  const std::vector<MapEntry>& a_map,
  std::vector<NumberRange> a_number_ranges
  )
{
  std::vector<NumberRange> a_result;
  std::vector<NumberRange> a_unchanged_number_ranges;

  for (const MapEntry& map_entry : a_map)
  {
    for (const NumberRange& number_range : a_number_ranges)
    {
      auto [a_current_changed, a_current_unchanged] =
        apply_map_entry(map_entry, number_range);

      // store any changed number ranges to return
      a_result.insert(
        a_result.end(),
        a_current_changed.begin(),
        a_current_changed.end()
        );

      // keep any unchanged ranges to see if another map entry applies
      a_unchanged_number_ranges.insert(
        a_unchanged_number_ranges.end(),
        a_current_unchanged.begin(),
        a_current_unchanged.end()
        );
    }
    a_number_ranges = std::move(a_unchanged_number_ranges);
  }

  // return any remaining unchanged number ranges
  a_result.insert(
    a_result.end(),
    a_number_ranges.begin(),
    a_number_ranges.end()
    );

  return a_result;
};

int
main()
{
  std::ifstream file(SRC_DIR"/input.txt");

  // Process input

  std::string z_line;

  std::getline(file, z_line);
  const std::vector<NumberRange> a_seed_ranges = get_number_ranges(z_line);
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

  // Calculate location number ranges

  std::vector<NumberRange> a_result_number_ranges = a_seed_ranges;
  for (const std::vector<MapEntry>& ai_map : aai_maps)
  {
    a_result_number_ranges = apply_map(ai_map, a_result_number_ranges);
  }

  std::cout << std::ranges::min_element(a_result_number_ranges)->i_begin << '\n';

  return 0;
}
