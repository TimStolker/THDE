#include "hwlib.hpp"
#include "rtos.hpp"
#include "send.hpp"
#include "display.hpp"
#include "TransferHitControl.hpp"
#ifndef RUNGAMECONTROL_HPP
#define RUNGAMECONTROL_HPP



namespace target = hwlib::target; 

class RunGameClass : public rtos::task <>{
    enum state_t {IDLE,NORMAL,SHOOT};
    state_t state = IDLE;
private:
    rtos::pool <int> HitPowerPool;
    rtos::pool <int> HitPlayerPool;
    rtos::flag HitFlag;
    rtos::flag StartFlag;
    int time;
    rtos::clock timeClock;
    DisplayTask & display;
    irSendControlClass & irSend;
    TransferHitControl & TransferHit;
    
    hitList HitList;

    int PlayerData;
    int PlayerPower;
    int GameTime;

    int Health;
    int gunCooldown; 
    uint16_t TmpByte = 0;
    uint16_t ShootData = 1; //start bit


    void main(){
        auto sw = hwlib::target::pin_in( hwlib::target::pins::d43 );
        for(;;){
            switch(state){
                // ================================================================
                case IDLE: {
                    auto evt = wait(StartFlag);
                    if(evt == StartFlag){
                        state = NORMAL;
                    }
                    else{
                        state = IDLE;
                    }
                    break;
                }
                // ================================================================
                case NORMAL:{
                    display.clearDisplay();
                    display.writeDisplay(Health,1);
                    display.writeDisplay(time,1);

                    auto evt = wait(HitFlag+timeClock);
                    if(evt == HitFlag){

                        Health = (Health - HitPowerPool.read());
                        HitList.set(HitPlayerPool.read(), HitPowerPool.read());
                        if(Health == 0){
                            display.clearDisplay();
                            display.writeDisplay("GAME OVER", 1);
                            state = IDLE;
                        }
                        else{
                            state = NORMAL;
                        }
                    }
                    else if(sw.read()==0){
                        if(gunCooldown<=0){

                            state = SHOOT;
                        }
                    }
                    else if(time == 0){
                        display.clearDisplay();
                        display.writeDisplay("Time's up",1);
                        TransferHit.transfer(HitList);
                        state = IDLE;
                    }
                    else if(evt == timeClock){
                        time--;
                        gunCooldown--;
                    }
                    break;
                }
                // ================================================================
                case SHOOT:{
                    
                    //convert PlayerData & PlayerPower to uint16_t

                    PlayerData = PlayerData << 1;
                    ShootData = ShootData | PlayerData;

                    PlayerPower = PlayerPower << 6;
                    ShootData = ShootData | PlayerPower;

                    for(int i=11;i<16;i++){
                        TmpByte = 1;
                        PlayerData = ShootData >> (i-10);
                        PlayerPower = ShootData >> (i-5);
                        TmpByte = (TmpByte & (PlayerData & PlayerPower)) << i;
                        ShootData = ShootData | TmpByte;
                    }
                    irSend.setSignal(ShootData); 
                    gunCooldown = PlayerPower;
                    state = NORMAL;
                }
                break;

            }
        }
    }

public:
    RunGameClass(irSendControlClass & irSend, DisplayTask & display, long long delay, TransferHitControl & TransferHit): rtos::task<>("RunGameTask"), HitPowerPool("HitPowerPool"), HitPlayerPool("HitPlayerPool"), HitFlag(this, "HitFlag"), StartFlag(this, "StartFlag"), timeClock( this, delay, "timeClock" ), display(display), irSend(irSend), TransferHit(TransferHit){ }
    void GetHit(int PlayerNmr, int power){ HitPowerPool.write(power); HitPlayerPool.write(PlayerNmr); HitFlag.set(); }
    void StartGame(){StartFlag.set();}
    void SetPlayerData(int PlayerNmr, int power){ PlayerData = PlayerNmr; PlayerPower = power; }
    void SetGameTime(int Time){ time = Time; }

};
#endif
