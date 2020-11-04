#include "hwlib.hpp"
#include "rtos.hpp"
#include "display.hpp"
#include "send.hpp"
namespace target = hwlib::target;
#ifndef INITGAMECONTROL_H
#define INITGAMECONTROL_H

class InitGameControl : public rtos::task <>{

private:
    
	rtos::channel<char, 10> KeyPadChannel;
	int KeyPadPressedTime;
    DisplayTask & display;
	irSendControlClass & irSend;
	void main()
	{
		enum state_t { IDLE, TIMECOMMAND, SHOOTSTART, SHOOTTIME };
		state_t state = IDLE;
		char ButtonID;
		for(;;)
		{
			switch(state)
			{
				// ================================================================
				
				case IDLE:
				{
					display.clearDisplay();
					hwlib::wait_ms(100);
					display.writeDisplay("Press C");
                
                    auto evt = wait(KeyPadChannel);
					if(evt==KeyPadChannel)
					{
						ButtonID = KeyPadChannel.read();
						hwlib::cout<<ButtonID<<hwlib::endl;
						if(ButtonID == 'C')
						{
							KeyPadPressedTime = 0;
							display.clearDisplay();
							hwlib::wait_ms(100);
							display.writeDisplay("Select time", 1);
							display.writeDisplay("Shoot = #",1);
							state = TIMECOMMAND;
						}
						else
						{
							state = IDLE;
						}
					}
				break;
				}
				// ================================================================
				case TIMECOMMAND:
				{

					auto evt = wait(KeyPadChannel);
					if(evt==KeyPadChannel)
					{
						ButtonID = KeyPadChannel.read();
						hwlib::cout<<ButtonID<<hwlib::endl;
						if(ButtonID>=48&&ButtonID<57)
						{
							KeyPadPressedTime += ButtonID-'0';
							display.clearDisplay();
							hwlib::wait_ms(100);
                            display.writeDisplay("TIME= ",1);
							display.writeDisplay(KeyPadPressedTime,0);
							state = TIMECOMMAND;
						}
						else if(ButtonID == '#')
						{
							if(KeyPadPressedTime>0&&KeyPadPressedTime<=15)
							{
								KeyPadPressedTime = (KeyPadPressedTime << 5) | (32768+KeyPadPressedTime);
								hwlib::cout<<"KeyPadPressTime: " << KeyPadPressedTime << "\n";
								irSend.setSignal(KeyPadPressedTime);
								state = SHOOTTIME;
							}
							else
							{
								KeyPadPressedTime = 0;
								state = TIMECOMMAND;
							}
						}
						else
						{
							state = TIMECOMMAND;
						}
					}
				break;
				}
				// ================================================================
				case SHOOTTIME:
				{

                    auto evt = wait(KeyPadChannel);
					if(evt==KeyPadChannel)
					{
						ButtonID = KeyPadChannel.read();
						hwlib::cout<<ButtonID<<hwlib::endl;
						if(ButtonID == '*')
						{
							
							display.clearDisplay();	
							hwlib::wait_ms(100);				
                            display.writeDisplay("Press * to start");
							
							state = SHOOTSTART;
						}
						else if(ButtonID == '#'){
							//hwlib::cout<<"Shoot again\n";
							irSend.setSignal(KeyPadPressedTime);
						}
					}
				break;
				}
				// ================================================================
				case SHOOTSTART:
				{
                    auto evt = wait(KeyPadChannel);
					if(evt==KeyPadChannel)
					{
						ButtonID = KeyPadChannel.read();
						hwlib::cout<<ButtonID<<hwlib::endl;
						if(ButtonID == 'C')
						{
							state = IDLE;
						}
						else if(ButtonID == '*')
						{
							hwlib::cout<<"Start signal\n";
							irSend.setSignal(32768);
							state = SHOOTSTART;
						}
					}
				}
			}
		}
	}
public:
	InitGameControl(DisplayTask & display, irSendControlClass & irSend):rtos::task<>("InitGameTask"),KeyPadChannel(this, "character"), display(display), irSend(irSend){};
	void buttonPressed(char buttonNumber){KeyPadChannel.write(buttonNumber);}

};

#endif
