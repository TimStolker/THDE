#include "hwlib.hpp"
#include "rtos.hpp"
#include "receive.hpp"

int main( void ){	
	auto receiver = irReceiveControlClass();
	rtos::run();
}
