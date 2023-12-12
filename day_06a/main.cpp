#include <cmath>
#include <fstream>
#include <iostream>
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

struct Race
{
  int i_time;
  int i_distance;
};

int
main()
{
  std::ifstream file(SRC_DIR"/input.txt");

  // Process input

  std::string z_times;
  std::string z_distances;
  std::getline(file, z_times);
  std::getline(file, z_distances);

  std::vector<int> ai_times = get_numbers(z_times);
  std::vector<int> ai_distances = get_numbers(z_distances);

  auto a_races =
    std::views::zip(ai_times, ai_distances) |
    std::views::transform(
      [&](const auto& entry)
      {
        const auto& [i_time, i_distance] = entry;
        return Race{i_time, i_distance};
      });

  int i_result = 1;

  for (const Race& race : a_races)
  {
    // distance > hold * (time - hold)
    // hold^2 - time*hold + distance > 0

    // solutions for hold
    // hold = (time +- sqrt(time^2 - 4*distance)) / 2

    // winning values for hold will be
    // - centered at (time/2)
    // - with a "radius" of (sqrt(time^2 - 4*distance) / 2)

    // number of integer solutions
    // - needs to deal with time being both even and odd
    // - needs to deal with the inequality being greater, not greater or equal

    const int i_double_radius_squared =
      std::pow(race.i_time, 2) - 4*race.i_distance;
    const int i_double_radius = std::sqrt(i_double_radius_squared);
    const bool b_radius_is_square =
      std::pow(i_double_radius, 2) == i_double_radius_squared;

    const int i_count = 
      race.i_time % 2 == 0 ?
        1 + (i_double_radius / 2 * 2) + (b_radius_is_square ? -2 : 0) :
        ((i_double_radius+1) / 2 * 2)
        ;

    i_result *= i_count;
  }

  std::cout << i_result << '\n';

  return 0;
}
