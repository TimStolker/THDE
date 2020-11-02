#include "hwlib.hpp"
#include "rtos.hpp"
#include "display.hpp"
#include "RunGameControl.hpp"

#ifndef REGISTERGAME_H
#define REGISTERGAME_H

class Registergame : public rtos::task <>{
	enum state_t {IDLE, PLAYERNUMBER, WEAPONPOWER, WAITPLAYTIME};
		
		
	
	
private:
		rtos::channel<char, 10> KeyPadChannel;

		state_t state = IDLE;
		
		rtos::flag Commandflag;
		rtos::pool <uint8_t>CmdRecievePool;
		rtos::flag Startflag;
		DisplayTask & display;
		RunGameClass & runGame;
		int Command;
		
		void main(){
			int sendnum = 0;
			
			hwlib::wait_ms( 500 );
			for(;;){
				switch(state){
					//=========================================================
					case IDLE:{
						display.clearDisplay();
						hwlib::wait_ms(500);
						display.writeDisplay("Press A",1);
						
						auto evt = wait(KeyPadChannel);
						if(evt==KeyPadChannel)
						{
							if('A'==KeyPadChannel.read()){
								display.clearDisplay();
								hwlib::wait_ms(500);
								display.writeDisplay("Player: ");
								state = PLAYERNUMBER;
							}
						}
						break;
					}
					//=========================================================
					case PLAYERNUMBER:{
						
						auto evt = wait(KeyPadChannel);
						if(evt==KeyPadChannel)
						{
							char press=KeyPadChannel.read();
							if(press-'0'>=0&&press-'0'<=9)
							{
								display.clearDisplay();
								sendnum = press-48; //SEND NAAR GAME
								hwlib::wait_ms(500);
								display.writeDisplay("Player: ");
								display.writeDisplay(sendnum,0);
							}
							if(press=='B'){
								if(!(sendnum==0)){	
									display.clearDisplay();
									hwlib::wait_ms(500);
									display.writeDisplay("Weaponpower: ",1);
									state = WEAPONPOWER;
								}	
							}
						}
						break;
					}
					//===========================================================
					case WEAPONPOWER:{

						auto evt = wait(KeyPadChannel);
						if(evt==KeyPadChannel)
						{
							char press=KeyPadChannel.read();
							if(press-'0'>0&&press-'0'<=9)
							{
								sendnum = press-48; //SEND NAAR GAME

	
								display.writeDisplay(sendnum,0);
								//write weap
								hwlib::wait_ms(500);
								state = WAITPLAYTIME;
							}
						
						}
						break;
						
					}
					//===========================================================
					case WAITPLAYTIME:{
						display.clearDisplay();
						hwlib::wait_ms(500);
						display.writeDisplay("Wait Time",1);
						auto evt = wait(Commandflag+Startflag);
						if(evt == Commandflag){
							Command = CmdRecievePool.read();
							runGame.SetGameTime(Command);
						}
						else if(evt == Startflag){
							runGame.StartGame();
							state = IDLE;
						}
						
						break;
						
					}
				
				}
			}

		}
public:
		Registergame(DisplayTask & display, RunGameClass & runGame):rtos::task <>(2,"ParameterTask"),KeyPadChannel( this, "character" ),Commandflag( this, "Commandflag " ), CmdRecievePool("CmdRecievePool"), Startflag(this, "Startflag"), display(display), runGame(runGame){}; // ,Commandflag( this, "Commandflag " ) moet er bijjj voor recieve
		void buttonPressed(char buttonNumber){KeyPadChannel.write(buttonNumber);}
		void GameTime(int time){Commandflag.set();CmdRecievePool.write(time);}
		void Start(){Startflag.set();}

	
};
#endif
