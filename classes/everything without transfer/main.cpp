#include "hwlib.hpp"
#include "rtos.hpp"
#include "display.hpp"
#include "InitGameControl.hpp"
#include "RunGameControl.hpp"
#include "Registergame.hpp"
#include "KeyPad.hpp"
#include "receive.hpp"
#include "TransferHitControl.hpp"

int main( void ){
    hwlib::wait_ms(500);
    auto transfer = TransferHitControl();
    auto displayClass = DisplayClass();
    auto display = DisplayTask(displayClass);
    auto irSend = irSendControlClass();
    auto Init = InitGameControl(display,irSend);
    auto runGame = RunGameClass(irSend, display, 10000 * rtos::ms, transfer);
    auto reg = Registergame(display, runGame);
    auto mykeypad = Keypadclass(Init,reg);
    auto receive = irReceiveControlClass(runGame,reg);

    rtos::run();
}
