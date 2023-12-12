#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <ranges>
#include <set>
#include <string>
#include <vector>

enum class Direction
{
  Invalid, North, East, South, West
};

Direction
follow_pipe(char c_pipe, Direction e_direction)
{
  if (c_pipe == '|')
  {
    return
      e_direction == Direction::North ? Direction::North :
      e_direction == Direction::South ? Direction::South :
      Direction::Invalid;
  }
  if (c_pipe == '-')
  {
    return
      e_direction == Direction::East ? Direction::East :
      e_direction == Direction::West ? Direction::West :
      Direction::Invalid;
  }
  if (c_pipe == 'L')
  {
    return
      e_direction == Direction::South ? Direction::East :
      e_direction == Direction::West ? Direction::North :
      Direction::Invalid;
  }
  if (c_pipe == 'J')
  {
    return
      e_direction == Direction::East ? Direction::North :
      e_direction == Direction::South ? Direction::West :
      Direction::Invalid;
  }
  if (c_pipe == '7')
  {
    return
      e_direction == Direction::East ? Direction::South :
      e_direction == Direction::North ? Direction::West :
      Direction::Invalid;
  }
  if (c_pipe == 'F')
  {
    return
      e_direction == Direction::West ? Direction::South :
      e_direction == Direction::North ? Direction::East :
      Direction::Invalid;
  }
  return Direction::Invalid;
}

struct Position
{
  std::size_t i_row;
  std::size_t i_col;

  auto operator<=>(const Position&) const = default;
};

std::optional<Position>
move_forward(
  const std::vector<std::string>& az_grid,
  const Position& position,
  Direction e_direction
  )
{
  if (e_direction == Direction::North)
  {
    if (position.i_row == 0)
      return {};
    return {{position.i_row-1, position.i_col}};
  }
  if (e_direction == Direction::East)
  {
    if (position.i_col == az_grid[0].size()-1)
      return {};
    return {{position.i_row, position.i_col+1}};
  }
  if (e_direction == Direction::South)
  {
    if (position.i_row == az_grid.size()-1)
      return {};
    return {{position.i_row+1, position.i_col}};
  }
  if (e_direction == Direction::West)
  {
    if (position.i_col == 0)
      return {};
    return {{position.i_row, position.i_col-1}};
  }
  return {};
}

int
main()
{
  std::ifstream file(SRC_DIR"/input.txt");

  // Read input

  std::vector<std::string> az_grid;
  Position start_position;

  while (std::getline(file, az_grid.emplace_back()))
  {
    auto iter_start = az_grid.back().find('S');
    if (iter_start != std::string::npos)
    {
      start_position = {az_grid.size()-1, iter_start};
    }
  }

  while (az_grid.back().empty())
  {
    az_grid.pop_back();
  }

  // Find loop

  std::vector<std::pair<Position, Direction>> a_poses{
    {start_position, Direction::North},
    {start_position, Direction::East},
    {start_position, Direction::South},
    {start_position, Direction::West},
    };

  int i_step = 1;
  for (;; ++i_step)
  {
    std::vector<std::pair<Position, Direction>> a_next_poses;

    for (const auto& [position, e_direction] : a_poses)
    {
      // try to move forward
      const std::optional<Position> o_next_position =
        move_forward(az_grid, position, e_direction);
      if (!o_next_position)
        continue;

      const char c_pipe =
        az_grid[o_next_position->i_row][o_next_position->i_col];

      // try to follow pipe
      const Direction e_next_direction = follow_pipe(c_pipe, e_direction);
      if (e_next_direction == Direction::Invalid)
        continue;

      a_next_poses.push_back({*o_next_position, e_next_direction});
    }

    // A loop will always have an even number of spaces
    // Since the first space of the loop was already considered as the start,
    // tracing the loop in both directions will reach the other end at the same
    // time.
    auto a_next_positions = a_next_poses |
      std::views::transform([](const auto& pose){return pose.first;});
    std::set<Position> a_next_positions_set(
      a_next_positions.begin(),
      a_next_positions.end()
      );
    const bool b_loop_end = a_next_positions_set.size() != a_next_poses.size();

    if (b_loop_end)
      break;

    a_poses = std::move(a_next_poses);
  }

  std::cout << i_step << '\n';

  return 0;
}
