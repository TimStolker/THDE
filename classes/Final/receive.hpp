

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
    RunGameClass & runGame;
    Registergame & regGame;

    void main(){
        auto tsop_signal = target::pin_in( target::pins::d8 );
        enum state_t {IDLE, RECEIVE, PROCESS}; 
        state_t state = IDLE;
        for(;;){
            switch(state){
                case IDLE: {
                    tsop_signal.refresh();
                    if( tsop_signal.read() == 0 ){
                        state = RECEIVE;
                    }
                    break;
                }
                    
                case RECEIVE: {
                    for(int i=0;i<16;i++){
                        auto connection_timeout = hwlib::now_us();
                        for(;;){
                            int on_time = 0;
                            if(tsop_signal.read() == 0 ){
                                on_time = hwlib::now_us();
                                while(tsop_signal.read() == 0){}
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
                            if(hwlib::now_us()>=(connection_timeout+4000)){
                                hwlib::cout << "!----------------!\n";
                                hwlib::cout << "CONNECTION TIMEOUT \n";
                                hwlib::cout << "!----------------!\n";
                                return;
                            }

                        }
                        if(hwlib::now_us()>=(connection_timeout+4000)){
                        }
                        
                    }
                    uint16_t playerNumber = (byte >> 1) & 31; //pakt Player Nummer
                    uint16_t playerPower = (byte >> 6) & 31; //pakt Player Power
                    uint16_t PLcoded = (playerNumber & playerPower) << 11;

                    if(!(PLcoded==(PLcoded & byte))){ //als controle bits niet kloppen
                        hwlib::cout<<"Control-bit Error \n";
                        return;
                    }

                    state = PROCESS;
                    break;
                }

                case PROCESS: {
                    uint16_t playerNumber = (byte >> 1) & 31; //pakt Player Nummer
                    uint16_t playerPower = (byte >> 6) & 31; //pakt Player Power
                    if(playerNumber == 0 && playerPower == 0){
                        regGame.Start();
                    }
                    
                    if(playerNumber == 0){
                        //runGame.SetGameTime(playerPower * 60);
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
                    break;
                }

            }
        }
    }

public:
    irReceiveControlClass(RunGameClass & runGame, Registergame & regGame): rtos::task<>(1,"irreceive"), runGame(runGame), regGame(regGame){}

};

#endif
