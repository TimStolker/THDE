#include "hwlib.hpp"
#include "rtos.hpp"
#include "Registergame.hpp"
#include "InitGameControl.hpp"

#ifndef Keypad_H
#define Keypad_H
namespace target = hwlib::target; 



class Keypadclass : public rtos::task <>{

private:
	InitGameControl & InitKeyPad;
	Registergame & reg;
	bool Reg;
	bool Init;
	
private:
	void main()
	{		
		hwlib::wait_ms( 50);
		target::pin_oc out0 = target::pin_oc( target::pins::a3 );
		target::pin_oc out1 = target::pin_oc( target::pins::a2 );
		target::pin_oc out2 = target::pin_oc( target::pins::a1 );
		target::pin_oc out3 = target::pin_oc( target::pins::a0 );

		target::pin_in in0  = target::pin_in( target::pins::a7 );
		target::pin_in in1  = target::pin_in( target::pins::a6 );
		target::pin_in in2  = target::pin_in( target::pins::a5 );
		target::pin_in in3  = target::pin_in( target::pins::a4 );
		auto out_port = hwlib::port_oc_from( out0, out1, out2, out3 );
		auto in_port  = hwlib::port_in_from( in0,  in1,  in2,  in3  );
		auto matrix   = hwlib::matrix_of_switches( out_port, in_port );
		auto keypad   = hwlib::keypad< 16 >( matrix, "123A456B789C*0#D" );
		
		for(;;){
			auto c = keypad.getc();       
			if(Reg){
				reg.buttonPressed(c);
			}
			else if(Init){
				InitKeyPad.buttonPressed(c);
			}
			else{
				InitKeyPad.buttonPressed(c);
				reg.buttonPressed(c);
			}
			
		}
	}
public:
	Keypadclass(InitGameControl & InitKeyPad,Registergame & reg):rtos::task <>("keypadtaak"), InitKeyPad(InitKeyPad), reg(reg){}; // LET OP VOOR REGISTERGAME & REG :: Staat er niet in! Zeflde als reg(reg),
	void Regnoinit(bool reg){Reg=reg;}
	void Initnoreg(bool init){Init=init;}
	
};

#endif