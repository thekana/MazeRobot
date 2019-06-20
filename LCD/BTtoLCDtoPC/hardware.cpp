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

//auto hardware::receive_maze_layout() -> maze_layout_message
//{
//	//TODO: Student 4 to design maze_layout_message class
//	return maze_layout_message();
//}


//auto hardware::parse_maze_layout(maze_layout_message maze) -> maze_layout
//{
//	//TODO: Student 4 to design maze_layout class
//	return maze_layout();
//}

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

//auto hardware::display::print(maze_layout maze, cell_location cell) -> size_t
//{
//	//TODO: Student 4 to design cell_location class
//	return size_t();
//}

auto hardware::display::clear() -> void
{
}