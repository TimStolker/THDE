#include "hwlib.hpp"
#include "rtos.hpp"
namespace target = hwlib::target; 

class keypadclass : public rtos::task <>{
	
   
public:
	keypadclass():rtos::task <>(1,"keypadtaak"){
	};
	
private:
	void main(){
		hwlib::wait_ms( 500 );
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
		
		
		
		hwlib::cout << "Keypad demo\n";
		int n = 0;
		for(;;){
			auto c = keypad.getc();       
			hwlib::cout << ++n << " [" << c << "]\n";
		}
	}
};
