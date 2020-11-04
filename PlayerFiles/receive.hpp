//this file contains Doxygen lines
///file receive.hpp
/// \brief receive class
/// \details Contains all the necessary information about the receive class

#include "hwlib.hpp"
#include "rtos.hpp"
#include "RunGameControl.hpp"
#ifndef RECEIVE_HPP
#define RECEIVE_HPP
namespace target = hwlib::target; 
class irReceiveControlClass : public rtos::task <>{

private:
    uint16_t byte = 0x00;
    uint16_t byte_index = 32'768; //eerste bit links
    uint16_t playerPower;
    uint16_t playerNumber;
    uint16_t PLcoded;
    int on_time;
    RunGameClass & runGame;
    Registergame & regGame;
    unsigned int connection_timeout;

    void main(){
        auto tsop_signal = target::pin_in( target::pins::d8 );
        enum state_t {IDLE, RECEIVE, PROCESS}; 
        state_t state = IDLE;
        for(;;){
            switch(state){
                case IDLE: {
                    hwlib::wait_us(1000);
                    tsop_signal.refresh();
                    if( tsop_signal.read() == 0 ){
                        state = RECEIVE;
                    }
                    break;
                }
                    
                case RECEIVE: {
                    byte = 0;
                    for(int i=0;i<16;i++){
                        connection_timeout = hwlib::now_us();
                        for(;;){
                            hwlib::wait_us(100);
                            on_time = 0;
                            if(tsop_signal.read() == 0 ){
                                on_time = hwlib::now_us();
                                while(tsop_signal.read() == 0){hwlib::wait_us(100);}
                                on_time = (hwlib::now_us() - on_time);
                            }
                            if(i==0){
                                byte = byte | 0x01;
                                byte = byte << 1;
                                break;
                            }
                                
                            else if(on_time >= 1200 ){
                                byte = byte | 0x01;
                                if(i!=15){
                                    byte = byte << 1;
                                }
                                break;
                            }
                                
                            else if(on_time >=500 && on_time <=1200){
                                if(i!=15){
                                    byte = byte << 1;
                                }
                                break;
                            }
                            if(hwlib::now_us()>=(connection_timeout+40000)){
                                state = IDLE;
                                break;             
                            }
                        }
                        if(hwlib::now_us()>=(connection_timeout+4000)){break;}
                    }
                    if(hwlib::now_us()>=(connection_timeout+4000)){break;}
                    playerNumber = (byte & ( 31 << 10 )) >> 10;
                    playerPower = (byte & ( 31 << 5 )) >> 5;
                    PLcoded = byte & 31;
                    
                    if(!(PLcoded == (playerNumber^playerPower << 0))){
                        state = IDLE;
                        break;
                    }

                    state = PROCESS;
                    break;
                }

                case PROCESS: {
                    if(playerNumber == 0 && playerPower == 0){
                        regGame.Start();
                    }
                    
                    else if(playerNumber == 0 && playerPower > 0){
                        regGame.GameTime(playerPower);
                    }
                    else{
                        runGame.GetHit(playerNumber, playerPower);
                    }
                    state = IDLE;
                    break;
                }

            }
        }
    }

public:
    irReceiveControlClass(RunGameClass & runGame, Registergame & regGame): 
    rtos::task<>("irreceive"), 
    runGame(runGame), 
    regGame(regGame)
    {}

};

#endif
