#include "hwlib.hpp"
#include "rtos.hpp"
#include "display.hpp"
#include "InitGameControl.hpp"
#include "RunGameControl.hpp"

#include "KeyPad.hpp"
#include "receive.hpp"

int main( void ){	
    hwlib::wait_ms(500);
    auto displayClass = DisplayClass();
    auto display = DisplayTask(displayClass);
    auto Init = InitGameControl(display);
    auto mykeypad = Keypadclass(Init);
    auto irSend = irSendControlClass();
    auto runGame = RunGameClass(irSend, display, 1000 * rtos::ms);
    
	rtos::run();
}
