#include "hwlib.hpp"
#include "rtos.hpp"
namespace target = hwlib::target;
#ifndef TRANSFERHITCONTROL_H
#define TRANSFERHITCONTROL_H

class hitList{
private:
    int hits[9]; 

public:
    hitList(){
        for(int i=0; i<10; i++){
            hits[i] = 0;
        }
    }
    int get(int player){ return hits[player]; }
    void set(int player, int power){
        hits[player] += power;
    }
};

class TransferHitControl : public rtos::task <>{
	
private:
	enum state_t { IDLE, DATATRANSFER };
	state_t state = IDLE;
    rtos::flag transferFlag;
	
	hitList hitlist;
	
	void main()
	{
		for(;;)
		{
			switch(state)
			{
				case IDLE:{
					auto evt = wait(transferFlag);
					if(evt == transferFlag){
						hwlib::wait_ms(200);
						state = DATATRANSFER;
					}
					break;
				}
				case DATATRANSFER:
				{
					for(int i = 0; i<9; i++){
						if(!(hitlist.get(i) == 0)){
							hwlib::cout << "Player: " << i << " Damage done: " << hitlist.get(i) << hwlib::endl;
						}
					}
					hwlib::cout << "Done" << hwlib:: endl;
					state = IDLE;
				}
					break;
			}
		}
	}
	
public:
    TransferHitControl():
	rtos::task<>("TransferHitTaak"), 
	transferFlag(this, "transferFlag")
    {}
    
    void transfer(hitList & list){ hitlist = list; transferFlag.set(); }
};

#endif
