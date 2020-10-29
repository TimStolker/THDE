 
#include "hwlib.hpp"
#include "rtos.hpp"
#include "send.hpp"

int main( void ){	
	auto ir = irSendControlClass();
	rtos::run();
}
