//this file contains Doxygen lines
///file display.hpp
/// \brief display class and display task 
/// \details Contains all the necessary information about the DisplayClass and DisplayTask

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
                    
                    display << "\n" << displayPool.read() << hwlib::flush; //print de text op een nieuwe lijn
                }
                else{
                    display  << displayPool.read() << hwlib::flush; //print de text
                }

                
            }   
            else if(evt == flagDisplayInt){
                if(NewLine){
                    
                    display << "\n" << displayPoolInt.read() << hwlib::flush; //print de text op een nieuwe lijn
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
    rtos::flag flagClear;
    DisplayClass & display;
    bool NewLine;

    void main(){
        for(;;){ 
            auto evt = wait(flagDisplayReadChar+flagDisplayReadInt+flagClear);
            if(evt == flagClear){
                display.clearDisplay();
                hwlib::wait_ms(100);
            }
            else if(evt == flagDisplayReadChar){
                display.setDisplay(displayPoolReadChar.read(), NewLine);
                hwlib::wait_ms(100);

            }
            else if(evt == flagDisplayReadInt){
                display.setDisplay(displayPoolReadInt.read(), NewLine);
                hwlib::wait_ms(100); 
            }

            
            
        }
    }

public:
    DisplayTask(DisplayClass & displayClass): 
    task("displayTask"), 
    flagDisplayReadChar(this, "flagDisplayReadChar"), 
    displayPoolReadChar("displayPoolReadChar"), 
    flagDisplayReadInt(this, "flagDisplayReadInt"), 
    displayPoolReadInt("displayPoolReadInt"), 
    flagClear(this, "flagClear"), 
    display(displayClass)
    {}
    
    /// \brief Writes something on the display
    /// \details Requires a const char* text and a bool newLine. 1 for newLine, 0 for same line
    void writeDisplay(const char* text, bool newLine = false){ NewLine = newLine; displayPoolReadChar.write(text); flagDisplayReadChar.set(); }

    /// \brief Writes something on the display
    /// \details Requires an int i and a bool newLine. 1 for newLine, 0 for same line
    void writeDisplay(int text, bool newLine = false){ NewLine = newLine; displayPoolReadInt.write(text); flagDisplayReadInt.set(); }

    /// \brief Clears the display
    /// \details Sets the flagClear
    void clearDisplay(){ flagClear.set(); }
};

#endif
