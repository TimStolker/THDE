#include "hwlib.hpp"
#include "rtos.hpp"
#ifndef REGISTERGAME_H
#define REGISTERGAME_H

class Registergame : public rtos::task <>{
	//enum state_t = {IDLE, PLAYERNUMBER, WEAPONPOWER, WAITPLAYTIME};
	
		
		
	private:
		//state_t state = IDLE;
		
		rtos::channel<char, 10> KeyPadChannel;
		
		//displayClass : display;
		//RungameControl& : runGame;
		//rtos::flag flagCommand;

	public:
		Registergame():rtos::task <>(1,"ParameterTask"),KeyPadChannel( this, "character" ){hwlib::cout<<"Construct reg\n";};
		void buttonPressed(char buttonNumber){KeyPadChannel.write(buttonNumber);}
		//void RecieveCmd(byte){flagCommand.set()}

	private:
		void main(){
			
			hwlib::wait_ms( 500 );
			hwlib::cout<<"Main regist";
			for(;;){
				auto c= KeyPadChannel.read();
				hwlib::cout<<c<<hwlib::endl;
						//auto evt = wait(KeyPadChannel)
						//if(evt==KeyPadChannel)
						//{
							//bnID=KeyPadChannel.read()
							//if(bnID){
								
							//}
						//}

					
			
			}
		
		}
};
#endif