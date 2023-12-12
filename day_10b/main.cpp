#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <ranges>
#include <set>
#include <string>
#include <vector>

bool
is_north_connected(char c_pipe)
{
  return c_pipe == '|' || c_pipe == 'L' || c_pipe == 'J';
}

bool
is_south_connected(char c_pipe)
{
  return c_pipe == '|' || c_pipe == 'F' || c_pipe == '7';
}

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
  Position start_position{0, 0};

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

  std::vector<std::optional<std::pair<Position, Direction>>> ao_poses{
    {{start_position, Direction::North}},
    {{start_position, Direction::East}},
    {{start_position, Direction::South}},
    {{start_position, Direction::West}},
    };

  std::optional<std::pair<std::size_t, std::size_t>> o_looping_path_indexes;
  std::vector<std::vector<Position>> a_path_positions(ao_poses.size());

  int i_step = 1;
  for (;; ++i_step)
  {
    std::vector<std::optional<std::pair<Position, Direction>>> ao_next_poses(
      ao_poses.size()
      );

    for (const auto& [i_pose_index, o_pose] : ao_poses | std::views::enumerate)
    {
      if (!o_pose)
        continue;

      const auto& [position, e_direction] = *o_pose;

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

      ao_next_poses[i_pose_index] = {*o_next_position, e_next_direction};
      a_path_positions[i_pose_index].push_back(*o_next_position);
    }

    // A loop will always have an even number of spaces
    // Since the first space of the loop was already considered as the start,
    // tracing the loop in both directions will reach the other end at the same
    // time.
    std::map<Position, std::size_t> a_seen_next_positions;
    for (
      const auto& [i_pose_index, o_next_pose] :
      ao_next_poses | std::views::enumerate
      )
    {
      if (!o_next_pose)
        continue;

      auto iter_seen = a_seen_next_positions.find(o_next_pose->first);
      if (iter_seen == a_seen_next_positions.end())
      {
        a_seen_next_positions[o_next_pose->first] = i_pose_index;
      }
      else
      {
        o_looping_path_indexes = {{i_pose_index, iter_seen->second}};

        // ensure path indexes are sorted
        if (o_looping_path_indexes->first > o_looping_path_indexes->second)
        {
          std::swap(
            o_looping_path_indexes->first,
            o_looping_path_indexes->second
            );
        }
      }
    }

    if (o_looping_path_indexes)
      break;

    ao_poses = std::move(ao_next_poses);
  }

  std::vector<Position> a_loop_positions{start_position};
  a_loop_positions.insert(
    a_loop_positions.end(),
    a_path_positions[o_looping_path_indexes->first].begin(),
    a_path_positions[o_looping_path_indexes->first].end()
    );
  a_loop_positions.insert(
    a_loop_positions.end(),
    a_path_positions[o_looping_path_indexes->second].rbegin()+1,
    a_path_positions[o_looping_path_indexes->second].rend()
    );

  // replace 'S' with a normal pipe
  using PathIndexes = std::pair<std::size_t, std::size_t>;
  az_grid[start_position.i_row][start_position.i_col] =
    *o_looping_path_indexes == PathIndexes{0, 2} ? '|' :
    *o_looping_path_indexes == PathIndexes{1, 3} ? '-' :
    *o_looping_path_indexes == PathIndexes{0, 1} ? 'L' :
    *o_looping_path_indexes == PathIndexes{0, 3} ? 'J' :
    *o_looping_path_indexes == PathIndexes{1, 2} ? 'F' :
    *o_looping_path_indexes == PathIndexes{2, 3} ? '7' :
    '.';

  // Find inner area

  std::map<std::size_t, std::set<std::size_t>> aai_loop_row_cols;
  for (const Position& position : a_loop_positions)
  {
    aai_loop_row_cols[position.i_row].insert(position.i_col);
  }

  std::size_t i_inner_area = 0;

  for (const auto& [i_row, ai_cols] : aai_loop_row_cols)
  {
    std::optional<std::size_t> oi_prev_boundary;
    std::size_t i_other_pipes = 0;
    bool b_north_connected = false;
    bool b_south_connected = false;
    for (const std::size_t i_col : ai_cols)
    {
      const char c_pipe = az_grid[i_row][i_col];
      b_north_connected = b_north_connected != is_north_connected(c_pipe);
      b_south_connected = b_south_connected != is_south_connected(c_pipe);

      if (b_north_connected && b_south_connected)
      {
        // a boundary encountered
        // either entering or exiting the enclosed area

        if (oi_prev_boundary)
        {
          i_inner_area += i_col - *oi_prev_boundary - i_other_pipes;

          oi_prev_boundary.reset();
          i_other_pipes = 0;
        }
        else
        {
          oi_prev_boundary = i_col;
        }

        b_north_connected = false;
        b_south_connected = false;
      }

      if (oi_prev_boundary)
      {
        // while inside, keep track of pipes seen
        // these will be subtracted from the area
        ++i_other_pipes;
      }
    }
  }

  std::cout << i_inner_area << '\n';

  return 0;
}
