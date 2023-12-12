#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <ranges>
#include <regex>
#include <string>
#include <vector>

std::vector<std::int64_t>
get_combined_number(std::string z_text)
{
  z_text.erase(
    std::remove_if(
      z_text.begin(),
      z_text.end(),
      [](char c){return !std::isdigit(c);}
      ),
    z_text.end()
    );
  return {std::stoll(z_text)};
}

struct Race
{
  std::int64_t i_time;
  std::int64_t i_distance;
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

  std::vector<std::int64_t> ai_times = get_combined_number(z_times);
  std::vector<std::int64_t> ai_distances = get_combined_number(z_distances);

  auto a_races =
    std::views::zip(ai_times, ai_distances) |
    std::views::transform(
      [&](const auto& entry)
      {
        const auto& [i_time, i_distance] = entry;
        return Race{i_time, i_distance};
      });

  std::int64_t i_result = 1;

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

    const std::int64_t i_double_radius_squared =
      std::pow(race.i_time, 2) - 4*race.i_distance;
    const std::int64_t i_double_radius = std::sqrt(i_double_radius_squared);
    const bool b_radius_is_square =
      std::pow(i_double_radius, 2) == i_double_radius_squared;

    const std::int64_t i_count = 
      race.i_time % 2 == 0 ?
        1 + (i_double_radius / 2 * 2) + (b_radius_is_square ? -2 : 0) :
        ((i_double_radius+1) / 2 * 2)
        ;

    i_result *= i_count;
  }

  std::cout << i_result << '\n';

  return 0;
}
