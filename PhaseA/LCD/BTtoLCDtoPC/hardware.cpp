#include "hardware.h"

auto hardware::i2c::enable() -> void
{
}

auto hardware::i2c::clock(clock_t clock) -> void
{
}

auto hardware::imu::enable() -> bool
{
  return false;
}

auto hardware::imu::update() -> bool
{
  return false;
}

auto hardware::imu::yaw() -> float
{
  return 0.0f;
}

auto hardware::imu::yaw(float value) -> void
{
}

auto hardware::imu::pitch() -> float
{
  return 0.0f;
}

auto hardware::imu::roll() -> float
{
  return 0.0f;
}

auto hardware::imu::stabilize() -> void
{
}

auto hardware::display::enable() -> void
{
}

auto hardware::display::cursor(coordinate cursor_position) -> void
{
}

auto hardware::display::print(char const * string) -> size_t
{
  return size_t();
}

auto hardware::display::print(int n) -> size_t
{
  return size_t();
}

auto hardware::display::print(double n) -> size_t
{
  return size_t();
}

auto hardware::display::clear() -> void
{
}

auto hardware::parse_maze_layout(maze_layout_message maze) -> maze_layout
{
  maze_layout layout(maze.rows, maze.cols, maze.hWall, maze.vWall);
  return layout;
}
