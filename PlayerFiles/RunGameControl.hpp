//this file contains Doxygen lines
///file RunGameControl.hpp
/// \brief RunGameControl class 
/// \details Contains all the necessary information about the RunGameControl class

#include "hwlib.hpp"
#include "rtos.hpp"
#include "send.hpp"
#include "display.hpp"
#include "TransferHitControl.hpp"
#ifndef RUNGAMECONTROL_HPP
#define RUNGAMECONTROL_HPP

namespace target = hwlib::target; 

class RunGameClass : public rtos::task <>{
private:
    enum state_t {IDLE,NORMAL,SHOOT};
    state_t state = IDLE;
    rtos::pool <int> HitPowerPool;
    rtos::pool <int> HitPlayerPool;
    rtos::flag HitFlag;
    rtos::flag StartFlag;
    rtos::clock timeClock;
    DisplayTask & display;
    irSendControlClass & irSend;
    TransferHitControl & TransferHit;
    hitList HitList;

    int PlayerData;
    int PlayerPower;
    int time;
    int Health=5;
    int gunCooldown; 
    uint16_t TmpTime = 0;
    uint16_t ShootData = 32'768; //start bit
    rtos::channel<char, 10> KeyPadChannel;

    void main(){
        for(;;){
            switch(state){
                case IDLE: {
                    auto evt = wait(StartFlag);
                    if(evt == StartFlag){
                        state = NORMAL;
                        TmpTime = time-5; 
                    }
                    break;
                }

                case NORMAL:{
                    hwlib::wait_ms(100);    
                    auto evt = wait(HitFlag+timeClock+KeyPadChannel);
                    if(evt == HitFlag){
                        Health = (Health - HitPowerPool.read());
                        HitList.set(HitPlayerPool.read(), HitPowerPool.read());
                        if(Health <= 0){
                            display.clearDisplay();
                            display.writeDisplay("GAME OVER", 1);
                            TransferHit.transfer(HitList);
                            state = IDLE;
                            break;
                        }
                    }
                    if(evt==KeyPadChannel){
                        if(KeyPadChannel.read()=='*'){
                            if(gunCooldown<=0){
                                state = SHOOT;
                                break;
                            }
                        }
                    }
                    if(evt == timeClock){
                        time--;
                        gunCooldown--;
                        
                        if(time == 0){
                            display.clearDisplay();
                            display.writeDisplay("Time's up",1);
                            TransferHit.transfer(HitList);
                            state = IDLE;
                            break;
                        }
                        else if((TmpTime+2) == time){
                            display.clearDisplay();
                            display.writeDisplay("Time:",1);
                            display.writeDisplay(time,1);
                        }
                        else if(TmpTime == time){
                            display.writeDisplay("Health:",1);
                            display.writeDisplay(Health,1);
                            TmpTime = time-5;   
                        }
                    }
                    break;
                }

                case SHOOT:{
                    ShootData = 32'768;
                    ShootData = ShootData | (PlayerData << 10);
                    ShootData = ShootData | (PlayerPower << 5);
                    ShootData = ShootData | (PlayerPower^PlayerData << 0);
                    
                    irSend.setSignal(ShootData); 
                    gunCooldown = PlayerPower;
                    state = NORMAL;
                }
                break;
            }
        }
    }

public:
    RunGameClass(irSendControlClass & irSend, DisplayTask & display, long long delay, TransferHitControl & TransferHit): 
        rtos::task<>("RunGameTask"),
        HitPowerPool("HitPowerPool"),
        HitPlayerPool("HitPlayerPool"),
        HitFlag(this, "HitFlag"),
        StartFlag(this, "StartFlag"),
        timeClock( this, delay, "timeClock" ),
        display(display), irSend(irSend), 
        TransferHit(TransferHit), 
        KeyPadChannel( this, "character" )
    {}

    /// \brief Gets playerNmr and power and write them in a pool
    /// \details Requires an int playerNmr and int power
    void GetHit(int PlayerNmr, int power){ HitPowerPool.write(power); HitPlayerPool.write(PlayerNmr); HitFlag.set(); }

    /// \brief Sets the StartFlag
    /// \details Is used by receive
    void StartGame(){StartFlag.set();}

    /// \brief Sets the player's number and power
    /// \details Requires an innt PlayerNmr and int power
    void SetPlayerData(int PlayerNmr, int power){ PlayerData = PlayerNmr; PlayerPower = power; }

    /// \brief Sets the game time
    /// \details Requires an int Time in minutes
    void SetGameTime(int Time){ time = Time; }

    /// \brief keypad button press
    /// \details Requires a char buttonNumber
    void buttonPressed(char buttonNumber){KeyPadChannel.write(buttonNumber);}

};
#endif
