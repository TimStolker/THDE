#include "hwlib.hpp"


void write_bit(bool num){
   auto ir = hwlib::target::d2_36kHz();
   if(num){ 
      ir.write(1);
      ir.flush();
      hwlib::wait_us(1600);
      ir.write(0);
      ir.flush();
      hwlib::wait_us(4000);
   }
   else{
      ir.write(1);
      ir.flush();
      hwlib::wait_us(800);
      ir.write(0);
      ir.flush();
      hwlib::wait_us(4000);
      
   }
}

void write_byte(uint8_t byte){
   
   uint8_t byte_index = 128;

   for(unsigned int i=0;i<8;i++){
      
      bool bit = (byte & (byte_index >> i));
      if(bit){
         write_bit(1);
      }
      else{
         write_bit(0);
      }
   }
}

int main( void ){    
   auto sw = hwlib::target::pin_in( hwlib::target::pins::d43 );

   for(;;){
      hwlib::wait_us( 1000 ); 
      if(sw.read() == 0){
         write_byte(129);
         for(;;){
            if(sw.read() == 1){
               break;
            }
         }
      }

   }
}
