#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <ranges>
#include <regex>
#include <string>

int
main()
{
  std::ifstream file(SRC_DIR"/input.txt");

  // Setup

  std::regex regex_digit(
    // lookahead regex to match overlapping texts
    "(?=(\\d|one|two|three|four|five|six|seven|eight|nine))."
    );

  auto x_digit_to_int = [](const std::string& z_text)
  {
    if (z_text == "one")
      return 1;
    if (z_text == "two")
      return 2;
    if (z_text == "three")
      return 3;
    if (z_text == "four")
      return 4;
    if (z_text == "five")
      return 5;
    if (z_text == "six")
      return 6;
    if (z_text == "seven")
      return 7;
    if (z_text == "eight")
      return 8;
    if (z_text == "nine")
      return 9;

    return std::stoi(z_text);
  };

  // Process input

  int i_result = 0;

  std::string z_line;
  while (std::getline(file, z_line))
  {
    auto ai_digits_range =
      std::ranges::subrange(
        std::sregex_iterator(z_line.begin(), z_line.end(), regex_digit),
        std::sregex_iterator()
        ) |
      std::views::transform(
        [&](const auto& match){return x_digit_to_int(match.str(1));}
        );
    std::vector<int> ai_digits(ai_digits_range.begin(), ai_digits_range.end());

    i_result += ai_digits.front()*10 + ai_digits.back();
  }

  std::cout << i_result << '\n';

  return 0;
}
