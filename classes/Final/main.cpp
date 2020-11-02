#include "hwlib.hpp"
#include "rtos.hpp"
#include "display.hpp"
#include "InitGameControl.hpp"
#include "RunGameControl.hpp"
#include "Registergame.hpp"
#include "KeyPad.hpp"
#include "receive.hpp"

int main( void ){	
    hwlib::wait_ms(500);
    auto displayClass = DisplayClass();
    auto display = DisplayTask(displayClass);
    
    auto irSend = irSendControlClass();
    auto Init = InitGameControl(display,irSend);
    auto runGame = RunGameClass(irSend, display, 60000 * rtos::ms);
    auto reg = Registergame(display, runGame);
    auto mykeypad = Keypadclass(Init,reg);

	rtos::run();
}