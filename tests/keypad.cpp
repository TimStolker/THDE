#include "hwlib.hpp"
#include "rtos.hpp"
#include "Keypad.h"

int main( void ){	
	auto mykeypad = keypadclass();
	rtos::run();
}
