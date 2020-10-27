#include "hwlib.hpp"

void receive(auto &tsop_signal){
   uint8_t byte = 0x00;
   
   for(int i=0;i<8;i++){
      for(;;){
         int on_time = 0;
         if(tsop_signal.read() == 0 ){
            on_time = hwlib::now_us();
            while(tsop_signal.read() == 0){}
            on_time = (hwlib::now_us() - on_time);
         }
            
         if(on_time >= 1200 ){
            byte = byte | 0x01;
            if(i!=7){
                  byte = byte << 1;
            }
            break;
         }
            
         else if(on_time >=500 && on_time <=1200){
            if(i!=7){
                  byte = byte << 1;
            }
            break;
         }
      }
   }

   uint8_t byte_index = 128;
   for(unsigned int i=0;i<8;i++){
   if(byte & (byte_index >> i)){
      hwlib::cout << "1";
   }
   else{
      hwlib::cout << "0";
   }
}
   hwlib::cout << "\n";
   hwlib::cout << byte << "   BYTE DONE \n";
}

int main( void ){
   namespace target = hwlib::target;
   
   auto tsop_signal = target::pin_in( target::pins::d8 );
   
   for(;;){
      tsop_signal.refresh();
      if( tsop_signal.read() == 0 ){
         receive(tsop_signal);
      }
   }
}
