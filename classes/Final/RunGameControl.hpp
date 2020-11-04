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
    int Health=100;
    int gunCooldown; 
    uint16_t TmpTime = 0;
    uint16_t ShootData = 32'768; //start bit
    rtos::channel<char, 10> KeyPadChannel;

    void main(){
        for(;;){
            switch(state){
                // ================================================================
                case IDLE: {
                    auto evt = wait(StartFlag);
                    if(evt == StartFlag){
                        state = NORMAL;
                        TmpTime = time-5; 
                    }
                    break;
                }
                // ================================================================
                case NORMAL:{
                    hwlib::wait_ms(100);    
                    auto evt = wait(HitFlag+timeClock+KeyPadChannel);
                    if(evt == HitFlag){
                        Health = (Health - HitPowerPool.read());
                        HitList.set(HitPlayerPool.read(), HitPowerPool.read());
                        if(Health == 0){
                            display.clearDisplay();
                            display.writeDisplay("GAME OVER", 1);
                            state = IDLE;
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
                // ================================================================
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
    RunGameClass(irSendControlClass & irSend, DisplayTask & display, long long delay, TransferHitControl & TransferHit): rtos::task<>("RunGameTask"), HitPowerPool("HitPowerPool"), HitPlayerPool("HitPlayerPool"), HitFlag(this, "HitFlag"), StartFlag(this, "StartFlag"), timeClock( this, delay, "timeClock" ), display(display), irSend(irSend), TransferHit(TransferHit), KeyPadChannel( this, "character" ){ }
    void GetHit(int PlayerNmr, int power){ HitPowerPool.write(power); HitPlayerPool.write(PlayerNmr); HitFlag.set(); }
    void StartGame(){StartFlag.set();}
    void SetPlayerData(int PlayerNmr, int power){ PlayerData = PlayerNmr; PlayerPower = power; }
    void SetGameTime(int Time){ time = Time; }
    void buttonPressed(char buttonNumber){KeyPadChannel.write(buttonNumber);}

};
#endif
