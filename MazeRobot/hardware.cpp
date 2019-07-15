#include "hardware.h"

auto hardware::digital_pin::config_io_mode (io_mode mode) -> void
{
	pinMode(pin_number, mode);
}

auto hardware::digital_pin::read () -> logic_level
{
	return digitalRead(pin_number);
}

auto hardware::digital_pin::write (logic_level level) -> void
{
	digitalWrite(pin_number, level);
}

auto hardware::digital_pin::high () -> void
{
	write(high);
}

auto hardware::digital_pin::low () -> void
{
	write(low);
}

auto hardware::digital_pin::pwm_write (units::percentage duty_cycle) -> void
{
	analogWrite(pin_number, (int)duty_cycle.count()*255);
}

auto hardware::motor::enable () -> void
{

}

auto hardware::motor::stop () -> void
{

}

auto hardware::motor::forward (units::percentage velocity) -> void
{
	
}

auto hardware::motor::backward (units::percentage velocity) -> void
{
	
}

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

auto hardware::receive_maze_layout() -> maze_layout_message
{
	//TODO: Student 4 to design maze_layout_message class
	return maze_layout_message();
}

auto hardware::parse_maze_layout(maze_layout_message maze) -> maze_layout
{
	//TODO: Student 4 to design maze_layout class
	return maze_layout();
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

auto hardware::display::print(maze_layout maze, cell_location cell) -> size_t
{
	//TODO: Student 4 to design cell_location class
	return size_t();
}

auto hardware::display::clear() -> void
{
}
