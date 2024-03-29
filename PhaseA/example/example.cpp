#include "example.h"
#include "Arduino.h"
/**
 * \brief Implement enable method.
 */
template <int times>
auto example::hello_world<times>::enable () -> void
{
	Serial.begin (115200);
}

/**
 * \brief Implement print method.
 * \return total number of character printed.
 */
template <int times>
auto example::hello_world<times>::print () -> size_t
{
  String str;
  if(times>=3){
    str = "Hello 3\n";
  } else {
    str = "Hello 1\n";
  }
	auto sum = 0;
	for (auto it = 0; it < times; ++it)
	{
		sum += Serial.print (str);
	}
	return sum;
}

// Explicit instantiation of hello_world
template class example::hello_world<1>;
template class example::hello_world<2>;
template class example::hello_world<3>;
template class example::hello_world<4>;
template class example::hello_world<5>;
