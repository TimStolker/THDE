//this file contains Doxygen lines
///file InitGameControl.hpp
/// \brief InitGameControl class
/// \details Contains all the necessary information about the InitGameControl class

#include "hwlib.hpp"
#include "rtos.hpp"
#include "display.hpp"
#include "send.hpp"
#ifndef INITGAMECONTROL_H
#define INITGAMECONTROL_H

namespace target = hwlib::target;

class InitGameControl : public rtos::task <>{
private:
	rtos::channel<char, 10> KeyPadChannel;
	int KeyPadPressedTime;
        DisplayTask & display;
	irSendControlClass & irSend;
	void main(){
		enum state_t { IDLE, TIMECOMMAND, SHOOTSTART, SHOOTTIME };
		state_t state = IDLE;
		char ButtonID;
		for(;;){
			switch(state){
				case IDLE:{
					display.clearDisplay();
					display.writeDisplay("Press C");
                                        auto evt = wait(KeyPadChannel);
					if(evt==KeyPadChannel){
						ButtonID = KeyPadChannel.read();
						if(ButtonID == 'C'){
							KeyPadPressedTime = 0;
							display.clearDisplay();
							display.writeDisplay("Select time", 1); 
							state = TIMECOMMAND;
						}
						else{
							state = IDLE;
						}
					}
				break;
				}
				case TIMECOMMAND:{
					auto evt = wait(KeyPadChannel);
					if(evt==KeyPadChannel){
						ButtonID = KeyPadChannel.read();
						if(ButtonID>=48&&ButtonID<57){
							KeyPadPressedTime += ButtonID-'0';
							display.clearDisplay();
							display.writeDisplay("TIME= ",1);
							display.writeDisplay(KeyPadPressedTime,0);
							state = TIMECOMMAND;
						}
						else if(ButtonID == '#'){
							if(KeyPadPressedTime>0&&KeyPadPressedTime<=15){
								KeyPadPressedTime = (KeyPadPressedTime << 5) | 32'768;
								irSend.setSignal(KeyPadPressedTime);
								state = SHOOTTIME;
							}
							else{
								KeyPadPressedTime = 0;
								state = TIMECOMMAND;
							}
						}
						else{
							state = TIMECOMMAND;
						}
					}
				break;
				}
				case SHOOTTIME:{
                    		auto evt = wait(KeyPadChannel);
					if(evt==KeyPadChannel){
						ButtonID = KeyPadChannel.read();
						if(ButtonID == '*'){
							display.clearDisplay();					
                            				display.writeDisplay("Press * to start");
							state = SHOOTSTART;
						}
						else if(ButtonID == '#'){
							irSend.setSignal(KeyPadPressedTime);
						}
					}
				break;
				}
				case SHOOTSTART:{
                    			auto evt = wait(KeyPadChannel);
					if(evt==KeyPadChannel){
						ButtonID = KeyPadChannel.read();
						if(ButtonID == 'C'){
							state = IDLE;
						}
						else if(ButtonID == '*'){
							irSend.setSignal(32'768);
							state = SHOOTSTART;
						}
					}
				}
			}
		}
	}
public:
    InitGameControl(DisplayTask & display, irSendControlClass & irSend):
	rtos::task<>("InitGameTask"),
	KeyPadChannel(this, "character"), 
	display(display), 
	irSend(irSend)
    {}

    /// \brief keypad button press
    /// \details Requires a char buttonNumber
    void buttonPressed(char buttonNumber){KeyPadChannel.write(buttonNumber);}

};

#endif
