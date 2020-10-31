#include "hwlib.hpp"
#include "rtos.hpp"
namespace target = hwlib::target;
#ifndef INITGAMECONTROL_H
#define INITGAMECONTROL_H

class InitGameControl : public rtos::task <>{

private:
    
	rtos::channel<char, 10> KeyPadChannel;
	int KeyPadPressedTime;
    DisplayTask & display;
	void main()
	{
		enum state_t { IDLE, TIMECOMMAND, SHOOTSTART, SHOOTTIME };
		state_t state = IDLE;
		
		char ButtonID;
		//int ButtonIDint;
		for(;;)
		{
			switch(state)
			{
				// ================================================================
				
				case IDLE:
				{
                    
					hwlib::cout<<"STATE: IDLE" << hwlib::endl;
                    auto evt = wait(KeyPadChannel);
					if(evt==KeyPadChannel)
					{
						ButtonID = KeyPadChannel.read();
						hwlib::cout<<ButtonID<<hwlib::endl;
						if(ButtonID == 'C')
						{
							KeyPadPressedTime = 0;
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
					hwlib::cout<<"STATE: TIMECOMMAND" << hwlib::endl;
					hwlib::cout<<"TIME: "<<KeyPadPressedTime<<hwlib::endl;
					// DISPLAY MESSAGE! = PRESS A NUMBER!
					auto evt = wait(KeyPadChannel);
					if(evt==KeyPadChannel)
					{
						ButtonID = KeyPadChannel.read();
						hwlib::cout<<ButtonID<<hwlib::endl;
						if(ButtonID>=48&&ButtonID<57)
						{
							KeyPadPressedTime += ButtonID-'0';
							hwlib::cout<<"TIME: "<<KeyPadPressedTime<<hwlib::endl;
                            display.writeDisplay("TIME=X");
                            hwlib::cout<<"na de write pool\n";
							// DISPLAY MESSAGE! = PRESS A NUMBER OR #, TIME=X min! ( VAR: KeyPadPressedTime )
							state = TIMECOMMAND;
						}
						else if(ButtonID == '#')
						{
							if(KeyPadPressedTime>0&&KeyPadPressedTime<=15)
							{
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
					hwlib::cout<<"STATE: SHOOTTIME" << hwlib::endl;
                    auto evt = wait(KeyPadChannel);
					if(evt==KeyPadChannel)
					{
						ButtonID = KeyPadChannel.read();
						hwlib::cout<<ButtonID<<hwlib::endl;
						if(ButtonID == '*')
						{
							//SetSignal(StartSignal);					
                            display.writeDisplay("Starting Game");
							//SetDisplay(Start);						// DISPLAY MESSAGE! = START GAME.
							
							state = SHOOTSTART;
						}
					}
				break;
				}
				// ================================================================
				case SHOOTSTART:
				{
					hwlib::cout<<"STATE: SHOOTSTART" << hwlib::endl;
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
							//setSignal(StartSignal);				// NOT EXCISTIING
							state = SHOOTSTART;
						}
					}
				}
			}
		}
	}
public:
	InitGameControl(DisplayTask & display):rtos::task<>("InitGameTask"),KeyPadChannel(this, "character"), display(display){};
	void buttonPressed(char buttonNumber){KeyPadChannel.write(buttonNumber);}

};

#endif
