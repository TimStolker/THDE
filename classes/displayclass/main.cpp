#include "hwlib.hpp"
#include "rtos.hpp"
#include "display.hpp"

int main( void ){	
    auto displayClass = DisplayClass();
	auto displayTask = DisplayTask(displayClass);
	rtos::run();
}
