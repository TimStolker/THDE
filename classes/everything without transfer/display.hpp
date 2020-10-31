#include "hwlib.hpp"
#include "rtos.hpp"
#ifndef DISPLAY_HPP
#define DISPLAY_HPP
namespace target = hwlib::target; 

class DisplayClass : public rtos::task <>{

private:
    rtos::flag flagDisplayChar;
    rtos::pool<const char*> displayPool;
    rtos::flag flagDisplayInt;
    rtos::pool<int> displayPoolInt;
    rtos::flag flagDisplayClear;
    bool NewLine;


    void main(){
        auto scl     = target::pin_oc( target::pins::scl );
        auto sda     = target::pin_oc( target::pins::sda );
        auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda( scl, sda );
        auto oled    = hwlib::glcd_oled( i2c_bus, 0x3c ); 
    
        auto font    = hwlib::font_default_8x8();
        auto display = hwlib::terminal_from( oled, font ); 
        

        
        for(;;){
            auto evt = wait(flagDisplayChar + flagDisplayInt + flagDisplayClear);
            if(evt == flagDisplayChar){
                if(NewLine){
                    hwlib::cout<<"Ye \n";
                    display << "\n" << displayPool.read() << hwlib::flush; //print de text
                }
                else{
                    display  << displayPool.read() << hwlib::flush; //print de text
                }

                
            }   
            else if(evt == flagDisplayInt){
                if(NewLine){
                    hwlib::cout<<"Soms \n";
                    display << "\n" << displayPoolInt.read() << hwlib::flush; //print de text
                }
                else{
                    display << displayPoolInt.read() << hwlib::flush; //print de text
                }
            }

            else if(evt == flagDisplayClear){
                display << "\f" << hwlib::flush;
            }
        } 
    }

public:
    DisplayClass(): task("displayClass"), flagDisplayChar(this, "flagDisplayChar"), displayPool("displayPool"),  flagDisplayInt(this, "flagDisplayInt"), displayPoolInt("displayPoolInt"), flagDisplayClear(this, "flagDisplayClear") { }
    void setDisplay(const char* text, bool newLine = false){ NewLine= newLine; displayPool.write(text); flagDisplayChar.set();  }
    void setDisplay(int i, bool newLine = false){ NewLine = newLine; displayPoolInt.write(i); flagDisplayInt.set();  }
    void clearDisplay(){ flagDisplayClear.set(); }
};

//-----------------------------------------------------------

class DisplayTask : public rtos::task<>{
private:
    rtos::flag flagDisplayReadChar;
    rtos::pool<const char*> displayPoolReadChar;
    rtos::flag flagDisplayReadInt;
    rtos::pool<int> displayPoolReadInt;
    DisplayClass & display;
    void main(){
        for(;;){ 
            auto evt = wait(flagDisplayReadChar+flagDisplayReadInt);
            if(evt == flagDisplayReadChar){
                hwlib::cout<<"Clear display\n";
                display.clearDisplay();
                hwlib::wait_ms(1000);
                display.setDisplay(displayPoolReadChar.read(), 1);
                hwlib::wait_ms(1000);
                hwlib::cout<<"display gewrite\n";   
            }
            else if(evt == flagDisplayReadInt){
                hwlib::cout<<"Clear display\n";
                display.clearDisplay();
                hwlib::wait_ms(1000);
                display.setDisplay(displayPoolReadInt.read(), 1);
                hwlib::wait_ms(1000);
                hwlib::cout<<"display gewrite\n";   
            }
            

            //display.setDisplay("Jumbo" , 1);
            //hwlib::wait_ms(1000);
            //display.setDisplay(6969420, 0);

            //hwlib::wait_ms(1000);
            
        }
    }

public:
    DisplayTask(DisplayClass & displayClass): task("displayTask"), flagDisplayReadChar(this, "flagDisplayReadChar"), displayPoolReadChar("displayPoolReadChar"), flagDisplayReadInt(this, "flagDisplayReadInt"), displayPoolReadInt("displayPoolReadInt"), display(displayClass){}
    void writeDisplay(const char* text){ displayPoolReadChar.write(text); flagDisplayReadChar.set(); }
    void writeDisplay(int text){ displayPoolReadInt.write(text); flagDisplayReadInt.set(); }
};

#endif
