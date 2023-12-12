#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

enum class HandType
{
  // Ordered by increasing strength
  HighCard,
  OnePair,
  TwoPair,
  ThreeOfAKind,
  FullHouse,
  FourOfAKind,
  FiveOfAKind,
};

HandType
get_hand_type(const std::string& z_hard)
{
  std::unordered_map<char, int> ai_card_type_counts;
  for (char c_card : z_hard)
  {
    ai_card_type_counts[c_card] += 1;
  }

  std::vector<int> ai_card_counts;
  for (const auto& [c_card, i_count] : ai_card_type_counts)
  {
    ai_card_counts.push_back(i_count);
  }
  std::ranges::sort(ai_card_counts | std::views::reverse);

  if (ai_card_counts[0] == 5)
    return HandType::FiveOfAKind;
  if (ai_card_counts[0] == 4)
    return HandType::FourOfAKind;
  if (ai_card_counts[0] == 3 && ai_card_counts[1] == 2)
    return HandType::FullHouse;
  if (ai_card_counts[0] == 3)
    return HandType::ThreeOfAKind;
  if (ai_card_counts[0] == 2 && ai_card_counts[1] == 2)
    return HandType::TwoPair;
  if (ai_card_counts[0] == 2)
    return HandType::OnePair;
  return HandType::HighCard;
}

static const std::unordered_map<char, int> ai_strengths{
  {'A', 14},
  {'K', 13},
  {'Q', 12},
  {'J', 11},
  {'T', 10},
  {'9',  9},
  {'8',  8},
  {'7',  7},
  {'6',  6},
  {'5',  5},
  {'4',  4},
  {'3',  3},
  {'2',  2},
  };

bool
compare_by_cards(const std::string& z_lhs, const std::string& z_rhs)
{
  for (const auto& [c_lhs, c_rhs] : std::views::zip(z_lhs, z_rhs))
  {
    const int i_difference = ai_strengths.at(c_lhs) - ai_strengths.at(c_rhs);
    if (i_difference < 0)
      return true;
    if (i_difference > 0)
      return false;
  }
  return false;
}

bool
compare_hands(const std::string& z_lhs, const std::string& z_rhs)
{
  const HandType i_hand_type_lhs = get_hand_type(z_lhs);
  const HandType i_hand_type_rhs = get_hand_type(z_rhs);
  if (i_hand_type_lhs < i_hand_type_rhs)
    return true;
  if (i_hand_type_lhs > i_hand_type_rhs)
    return false;
  return compare_by_cards(z_lhs, z_rhs);
}

int
main()
{
  std::ifstream file(SRC_DIR"/input.txt");

  // Process input

  std::vector<std::pair<std::string, std::int64_t>> a_entries;

  std::string z_line;
  while (std::getline(file, z_line))
  {
    const std::string z_hand = z_line.substr(0, 5);
    const std::int64_t i_bid = std::stoll(z_line.substr(6));
    a_entries.push_back({z_hand, i_bid});
  }

  std::ranges::sort(a_entries,
    [](const auto& lhs, const auto& rhs)
    {
      return compare_hands(lhs.first, rhs.first);
    });

  auto a_enumerated_entries = a_entries | std::views::enumerate;

  const int i_result = std::accumulate(
    a_enumerated_entries.begin(),
    a_enumerated_entries.end(),
    0ll,
    [](std::int64_t i_lhs, const auto& rhs)
    {
      const auto& [i_index, entry] = rhs;
      const auto& [z_hand, i_bid] = entry;
      return i_lhs + (i_index+1) * i_bid;
    });

  std::cout << i_result << '\n';

  return 0;
}
