#include "hwlib.hpp"
#include "rtos.hpp"
#include "display.hpp"
#include "InitGameControl.hpp"
#include "KeyPad.hpp"
#include "send.hpp"



int main( void ){	
    hwlib::wait_ms(500);
    auto displayClass = DisplayClass();
    auto display = DisplayTask(displayClass);
    auto irSend = irSendControlClass();
    auto Init = InitGameControl(display,irSend);
    auto mykeypad = Keypadclass(Init);

	rtos::run();
}
