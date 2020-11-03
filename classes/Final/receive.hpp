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
                                
                            if(on_time >= 1200 ){
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
                                hwlib::cout << on_time <<"  OUT! \n";
                                //hwlib::cout << byte << "   Byte \n";
                                state = IDLE;
                                break;             
                            }
                        }
                        if(hwlib::now_us()>=(connection_timeout+4000)){break;}
                    }
                    if(hwlib::now_us()>=(connection_timeout+4000)){break;}
                    hwlib::cout << "\n";
                    playerNumber = (byte >> 1) & 31; //pakt Player Nummer
                    playerPower = (byte >> 6) & 31; //pakt Player Power
                    PLcoded = (playerNumber & playerPower) << 11;

                    if(!(PLcoded==(PLcoded & byte))){ //als controle bits niet kloppen
                        state = IDLE;
                        break;
                    }

                    state = PROCESS;
                    break;
                }

                case PROCESS: {
                    if(playerNumber == 0 && playerPower == 0){
                        hwlib::cout << "Game start \n";
                        regGame.Start();
                    }
                    
                    if(playerNumber == 0){
                        hwlib::cout << "Time Set \n";
                        regGame.GameTime(playerPower * 60);
                    }
                    
                    else{
                        runGame.GetHit(playerNumber,playerPower);
                        for(unsigned int i=0;i<16;i++){
                            if(byte & (byte_index >> i)){
                                hwlib::cout << "1";
                            }
                            else{
                                hwlib::cout << "0";
                            }
                        }
                    }
                    
                    hwlib::cout << "\n";
                    hwlib::cout << byte << "   BYTE'S DONE \n";
                    state = IDLE;
                    break;
                }

            }
        }
    }

public:
    irReceiveControlClass(RunGameClass & runGame, Registergame & regGame): rtos::task<>(1,"irreceive"), runGame(runGame), regGame(regGame){hwlib::cout << "AANGEMAAKT \n";}

};

#endif
