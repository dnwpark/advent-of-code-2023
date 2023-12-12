#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <ranges>
#include <regex>
#include <string>
#include <unordered_map>

struct PathLoop
{
  std::size_t i_start;
  std::size_t i_length;
  std::vector<std::size_t> ai_initial_terminals;
  std::vector<std::size_t> ai_looping_terminals;
};

PathLoop
delay_start(PathLoop path_loop, std::size_t i_delay)
{
  path_loop.i_start += i_delay;

  const std::size_t i_loop_count = i_delay / path_loop.i_length;

  for (
    std::size_t i_loop_index = 0;
    i_loop_index < i_loop_count;
    ++i_loop_index
    )
  {
    path_loop.ai_initial_terminals.insert(
      path_loop.ai_initial_terminals.end(),
      path_loop.ai_looping_terminals.begin(),
      path_loop.ai_looping_terminals.end()
      );
    std::ranges::for_each(
      path_loop.ai_looping_terminals,
      [&](std::size_t& i_terminal){i_terminal += path_loop.i_length;}
      );
  }

  auto iter_added_to_initial = std::ranges::lower_bound(
    path_loop.ai_looping_terminals,
    path_loop.i_start
    );

  path_loop.ai_initial_terminals.insert(
    path_loop.ai_initial_terminals.end(),
    path_loop.ai_looping_terminals.begin(),
    iter_added_to_initial
    );
  std::for_each(
    path_loop.ai_looping_terminals.begin(),
    iter_added_to_initial,
    [&](std::size_t& i_terminal){i_terminal += path_loop.i_length;}
    );

  std::ranges::rotate(
    path_loop.ai_looping_terminals,
    iter_added_to_initial
    );

  return path_loop;
}

int
main()
{
  std::ifstream file(SRC_DIR"/input.txt");

  // Setup

  const std::regex regex_node("(\\w+)\\W+(\\w+)\\W+(\\w+)\\W+");

  // Parse input

  std::string z_line;

  std::getline(file, z_line);
  std::vector<std::size_t> ai_turns;
  std::ranges::transform(
    z_line,
    std::back_inserter(ai_turns),
    [](char c_turn)
    {
      return c_turn == 'R';
    });

  std::unordered_map<std::string, std::vector<std::string>> aaz_map;
  while (std::getline(file, z_line))
  {
    std::smatch node_match;
    if (!std::regex_match(z_line, node_match, regex_node))
      continue;

    aaz_map.insert({
      node_match.str(1),
      {node_match.str(2), node_match.str(3)}
      });
  }

  // Find looping paths

  auto ai_repeating_turns =
    std::views::repeat(ai_turns | std::views::enumerate) |
    std::views::join
    ;

  std::vector<decltype(aaz_map)::const_iterator> a_iter_path_positions;
  for (
    auto iter_position = aaz_map.begin();
    iter_position != aaz_map.end();
    ++iter_position
    )
  {
    if (iter_position->first.back() == 'A')
      a_iter_path_positions.push_back(iter_position);
  }
  const decltype(a_iter_path_positions) a_iter_path_start_positions =
    a_iter_path_positions; 

  std::vector<
    // don't want to bother with hashing tuple
    std::map<
      std::tuple<std::size_t, std::string>, // turn index + position
      std::size_t // round index
      >
    > aai_path_round_visited; 
  for (const auto& iter_position : a_iter_path_positions)
  {
    aai_path_round_visited.push_back({
      {{0, iter_position->first}, 0}
      });
  }

  std::vector<PathLoop> ai_path_loops(
    a_iter_path_positions.size(),
    {0, 0, {}, {}}
    );

  auto iter_turn = ai_repeating_turns.begin();

  int i_round_number = 0;
  while (std::ranges::any_of(
    ai_path_loops,
    [](const auto& path_loop){return path_loop.i_length == 0;}
    ))
  {
    ++i_round_number;

    const auto& [i_turn_index, i_turn] = *iter_turn;
    const std::size_t i_next_turn_index = (i_turn_index+1) % ai_turns.size();

    for (
      auto [i_position_index, iter_position] :
      a_iter_path_positions | std::views::enumerate
      )
    {
      iter_position = aaz_map.find(iter_position->second.at(i_turn));

      PathLoop& path_loop = ai_path_loops[i_position_index];

      if (path_loop.i_length == 0)
      {
        // loop not yet found

        std::pair<std::size_t, std::string> turn_index_and_position{
          i_next_turn_index,
          iter_position->first
          };

        auto& ai_round_visited = aai_path_round_visited[i_position_index];

        auto iter_round_visited =
          ai_round_visited.find(turn_index_and_position);
        if (iter_round_visited == ai_round_visited.end())
        {
          if (iter_position->first.back() == 'Z')
          {
            path_loop.ai_initial_terminals.push_back(i_round_number);
          }

          ai_round_visited.insert({
            turn_index_and_position,
            i_round_number
            });
        }
        else
        {
          const std::size_t i_start = iter_round_visited->second;
          const std::size_t i_length = i_round_number - i_start;

          path_loop.i_start = i_start;
          path_loop.i_length = i_length;

          auto iter_looped_terminal = std::ranges::lower_bound(
            path_loop.ai_initial_terminals,
            i_start
            );

          path_loop.ai_looping_terminals.insert(
            path_loop.ai_looping_terminals.end(),
            iter_looped_terminal,
            path_loop.ai_initial_terminals.end()
            );
          path_loop.ai_initial_terminals.erase(
            iter_looped_terminal,
            path_loop.ai_initial_terminals.end()
            );
        }
      }
    }
    ++iter_turn;
  }

  const std::size_t i_latest_start = std::ranges::max_element(
    ai_path_loops,
    [](const PathLoop& lhs, const PathLoop& rhs)
    {
      return lhs.i_start < rhs.i_start;
    })->i_start;

  if (false)
  {
    for (
      auto [i_position_index, iter_position] :
      a_iter_path_positions | std::views::enumerate
      )
    {
      const PathLoop path_loop = delay_start(
        ai_path_loops[i_position_index],
        i_latest_start - ai_path_loops[i_position_index].i_start
        );

      std::cout
        << a_iter_path_start_positions[i_position_index]->first << ' '
        << path_loop.i_start << ' '
        << path_loop.i_length << ' '
        << '\n';
      for (std::size_t i_terminal : path_loop.ai_initial_terminals)
      {
        std::cout << i_terminal << ' ';
      }
      std::cout << '\n';
      for (std::size_t i_terminal : path_loop.ai_looping_terminals)
      {
        std::cout << i_terminal << ' ';
      }
      std::cout << '\n';
    }
    std::cout << '\n';
  }

  // noting that the loops all have a single terminal
  // the terminal number is also the length of the loop
  // can simply use LCM

  const std::size_t i_result = std::accumulate(
    ai_path_loops.begin(),
    ai_path_loops.end(),
    std::size_t{1},
    [](std::size_t i_lhs, const PathLoop& path_loop)
    {
      return std::lcm(i_lhs, path_loop.i_length);
    });

  std::cout << i_result << '\n';

  return 0;
}
