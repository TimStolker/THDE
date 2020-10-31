#include "hwlib.hpp"
#include "rtos.hpp"
#include "display.hpp"

#ifndef REGISTERGAME_H
#define REGISTERGAME_H

class Registergame : public rtos::task <>{
	enum state_t {IDLE, PLAYERNUMBER, WEAPONPOWER, WAITPLAYTIME};
		
		
	
	
private:
		rtos::channel<char, 10> KeyPadChannel;
		DisplayTask & display;
		//RungameControl& : runGame;
		state_t state = IDLE;
		
		//rtos::flag Commandflag;
		rtos::pool <uint8_t>CmdRecievePool;
		
		void main(){
			int sendnum = 0;
			
			hwlib::wait_ms( 500 );
			hwlib::cout<<"Main regist";
			for(;;){
				switch(state){
					//=========================================================
					case IDLE:{
						
						hwlib::cout<<"REG:PRESS A IF START" << hwlib::endl;
						auto evt = wait(KeyPadChannel);
						if(evt==KeyPadChannel)
						{
							if('A'==KeyPadChannel.read()){
								state = PLAYERNUMBER;
							}
						}
						break;
					}
					//=========================================================
					case PLAYERNUMBER:{
						hwlib::cout<<"REGSTATE: PLAYERNUM" << hwlib::endl;
						hwlib::cout<<"PLAYERNUM: "<<sendnum<<hwlib::endl;
						display.writeDisplay("Player: ");
						auto evt = wait(KeyPadChannel);
						if(evt==KeyPadChannel)
						{
							char press=KeyPadChannel.read();
							if(press-'0'>=0&&press-'0'<=9)
							{
								sendnum = press-48; //SEND NAAR GAME
								hwlib::cout<<"Playernumber: "<<sendnum<<hwlib::endl;
							}
							if(press=='B'){
								if(sendnum==0){	
									hwlib::cout<<"PLAYER 0 Not avaible" << hwlib::endl;
								}
								else{
									state = WEAPONPOWER;
								}
								
							}
						
						}
						break;
					}
					//===========================================================
					case WEAPONPOWER:{
						hwlib::cout<<"REGSTATE: WEAPONPOWER" << hwlib::endl;
						hwlib::cout<<"WeaponPower: 0"<<hwlib::endl;
						display.writeDisplay("Weaponpower: ");
						auto evt = wait(KeyPadChannel);
						if(evt==KeyPadChannel)
						{
							char press=KeyPadChannel.read();
							if(press-'0'>0&&press-'0'<=9)
							{
								sendnum = press-48; //SEND NAAR GAME
								hwlib::cout<<"WeaponPower: "<<sendnum<<hwlib::endl;
								display.writeDisplay("Weaponpower: "+(sendnum+'0'));
								state = WAITPLAYTIME;
							}
						
						}
						break;
						
					}
					//===========================================================
					case WAITPLAYTIME:{
						//wait(Commandflag);
						//Command = CmdRecievePool.read();
						//wait(Startflag);
						//Command = CmdRecievePool.read();
						
						state = IDLE;
						break;
						
					}
				
				}
			}

		}
public:
		Registergame(DisplayTask & display):rtos::task <>(2,"ParameterTask"),KeyPadChannel( this, "character" ), display(display){hwlib::cout<<"Construct reg\n";}; // ,Commandflag( this, "Commandflag " ) moet er bijjj voor recieve
		void buttonPressed(char buttonNumber){KeyPadChannel.write(buttonNumber);}
		//void GameTime(int time){Commandflag.set();CmdRecievePool.write(int);}
		//void Start(){Startflag.set();}

	
};
#endif