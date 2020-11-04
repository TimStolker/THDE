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
    rtos::clock timeClock;
    rtos::channel<char, 10> KeyPadChannel;
    DisplayTask & display;
    irSendControlClass & irSend;
    TransferHitControl & TransferHit;
    
    hitList HitList;

    int PlayerData;
    int PlayerPower;
    int time;
    int Health=100;
    int gunCooldown; 
    uint16_t TmpByte = 0;
    uint16_t ShootData = 32'768; //start bit


    void main(){
        for(;;){
            switch(state){
                // ================================================================
                case IDLE: {
                    auto evt = wait(StartFlag);
                    if(evt == StartFlag){
                        state = NORMAL;
                    }
                    break;
                }
                // ================================================================
                case NORMAL:{
                    if((time%10)==0){
                        display.clearDisplay();
                        hwlib::wait_ms(500);
                        display.writeDisplay("Health:",1);
                        hwlib::wait_ms(100);
                        display.writeDisplay(Health,0);
                        display.writeDisplay("Time:",1);
                        hwlib::wait_ms(100);
                        display.writeDisplay(time,0);
                    }
                    

                    auto evt = wait(HitFlag+timeClock);
                    if(evt == HitFlag){

                        Health = (Health - HitPowerPool.read());
                        HitList.set(HitPlayerPool.read(), HitPowerPool.read());
                        if(Health == 0){
                            display.clearDisplay();
                            hwlib::wait_ms(100);
                            display.writeDisplay("GAME OVER", 1);
                            state = IDLE;
                        }
                    }
                    else if(KeyPadChannel.read() == '*'){
                        if(gunCooldown<=0){
                            state = SHOOT;
                            break;
                        }
                    }
                    else if(time == 0){
                        display.clearDisplay();
                        hwlib::wait_ms(100);
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
    RunGameClass(irSendControlClass & irSend, DisplayTask & display, long long delay, TransferHitControl & TransferHit): rtos::task<>("RunGameTask"), HitPowerPool("HitPowerPool"), HitPlayerPool("HitPlayerPool"), HitFlag(this, "HitFlag"), StartFlag(this, "StartFlag"), timeClock( this, delay, "timeClock" ), KeyPadChannel(this, "character"), display(display), irSend(irSend), TransferHit(TransferHit){ }
    void GetHit(int PlayerNmr, int power){ HitPowerPool.write(power); HitPlayerPool.write(PlayerNmr); HitFlag.set(); }
    void StartGame(){StartFlag.set();}
    void SetPlayerData(int PlayerNmr, int power){ PlayerData = PlayerNmr; PlayerPower = power; }
    void SetGameTime(int Time){ time = Time; }
    void buttonPressed(char buttonNumber){KeyPadChannel.write(buttonNumber);}

};
#endif
