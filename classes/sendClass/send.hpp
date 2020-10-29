#include "hwlib.hpp"
#include "rtos.hpp"
namespace target = hwlib::target; 
class irSendControlClass : public rtos::task <>{

private:
    rtos::flag flagShoot;

    void main(){
        uint16_t data = 58254; //test waarde
        uint16_t data_index = 32768; //meest linker bit
        enum state_t {IDLE, SHOOT}; 
        state_t state = IDLE;
        auto ir = hwlib::target::d2_36kHz(); 
        for(;;){
            switch(state){
                case IDLE: {
                    flagShoot.set();
                    
                    auto evt = wait(flagShoot);
                    if(evt == flagShoot){
                        state = SHOOT;
                    }
                    break;
                }
                    
                case SHOOT: {
                    for(unsigned int i=0;i<16;i++){
                        bool bit = (data & (data_index >> i));
                        if(bit){ 
                            ir.write(1);
                            ir.flush();
                            hwlib::wait_us(1600);
                            ir.write(0);
                            ir.flush();
                            hwlib::wait_us(800);
                        }
                        else{
                            ir.write(1);
                            ir.flush();
                            hwlib::wait_us(800);
                            ir.write(0);
                            ir.flush();
                            hwlib::wait_us(1600);
                            
                        }
                    }
                    state = IDLE;
                break;
                }
            }
        }
    }

public:
    irSendControlClass(): task (1,"irsend"), flagShoot (this, "flagShoot"){}

};
