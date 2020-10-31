#include "hwlib.hpp"
#include "rtos.hpp"

#ifndef SEND_HPP
#define SEND_HPP
namespace target = hwlib::target; 
class irSendControlClass : public rtos::task <>{

private:
    rtos::flag flagShoot;
    uint16_t Data = 58254; //test waarde

    void main(){
        
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
                        bool bit = (Data & (data_index >> i));
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
    irSendControlClass(): rtos::task<>(1,"irsend"), flagShoot(this, "flagShoot"){}
    void setSignal(uint16_t data){ Data = data; }

};

#endif
