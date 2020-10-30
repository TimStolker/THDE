#include "hwlib.hpp"
#include "rtos.hpp"
#include "display.hpp"
#include "InitGameControl.hpp"
#include "KeyPad.hpp"

int main( void ){	
    hwlib::wait_ms(500);
    auto displayClass = DisplayClass();
    auto display = DisplayTask(displayClass);
    auto Init = InitGameControl(display);
    auto mykeypad = Keypadclass(Init);
    
	rtos::run();
}
