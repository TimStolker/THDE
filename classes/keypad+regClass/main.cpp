#include "hwlib.hpp"
#include "rtos.hpp"
#include "Keypad.h"
#include "Registergame.h"


int main( void ){
	hwlib::wait_ms(500);
	hwlib::cout<<"Pre reg\n";
	
	auto reg = Registergame();
	hwlib::cout<<"Pre mykey\n";
	auto mykeypad = Keypadclass(reg);
	hwlib::cout<<"pre run\n";
	rtos::run();
}
