#include "hwlib.hpp"
#include "rtos.hpp"
#include "display.hpp"
#include "InitGameControl.hpp"
#include "RunGameControl.hpp"
#include "Keypad.hpp"
#include "receive.hpp"
#include "Registergame.hpp" 


int main( void ){
	hwlib::wait_ms(500);
	hwlib::cout<<"Pre reg\n";
	
	
	hwlib::cout<<"Pre mykey\n";
	
	auto displayClass = DisplayClass();
	
    auto display = DisplayTask(displayClass);
	auto reg = Registergame(display);
    auto Init = InitGameControl(display);
	
	auto mykeypad = Keypadclass(Init,reg);
	auto irSend = irSendControlClass();
    auto runGame = RunGameClass(irSend, display, 1000 * rtos::ms);
	hwlib::cout<<"pre run\n";
	rtos::run();
}
