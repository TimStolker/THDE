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
		hitlist.set(1, 1);	// TIJDELIJK
		hitlist.set(2, 2);	// TIJDELIJK
		hitlist.set(3, 3);	// TIJDELIJK
		hitlist.set(1, 10);	// TIJDELIJK
		auto ButtonInput = hwlib::target::pin_in( target::pins::d39 );
		hwlib::cout<<"Press button" << hwlib::endl; // TIJDELIJK
		for(;;)
		{
			switch(state)
			{
				case IDLE:
				{
                    auto evt = wait(transferFlag);
                    if(evt == transferFlag){
                        for(;;){
                            if(ButtonInput.read() == 0)
                            {
                                hwlib::wait_ms(200);
                                state = DATATRANSFER;
                            }
                        }

                    }
					break;
				}
				case DATATRANSFER:
				{
					for(int i = 0; i<10; i++){
						if(!(hitlist.get(i) == 0)){
							hwlib::cout << "Speler: " << i << " Hits " << hitlist.get(i) << hwlib::endl;
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
	TransferHitControl():rtos::task <>("TransferHitTaak"), transferFlag(this, "transferFlag"){}
    void transfer(hitList & list){ hitlist = list; transferFlag.set(); }
};

#endif
