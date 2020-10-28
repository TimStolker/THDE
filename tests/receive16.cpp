#include "hwlib.hpp"

void receive(auto &tsop_signal){
   uint16_t byte = 0x00;
   
   for(int i=0;i<16;i++){
      auto connection_timeout = hwlib::now_us();
      for(;;){
         int on_time = 0;
         if(tsop_signal.read() == 0 ){
            on_time = hwlib::now_us();
            while(tsop_signal.read() == 0){}
            on_time = (hwlib::now_us() - on_time);
         }
            
         if(on_time >= 1200 ){
            byte = byte | 0x01;
            if(i!=15){
                  byte = byte << 1;
            }
            break;
         }
            
         else if(on_time >=500 && on_time <=1200){
            if(i!=15){
                  byte = byte << 1;
            }
            break;
         }
         if(hwlib::now_us()>=(connection_timeout+4000)){
            hwlib::cout << "!----------------!\n";
            hwlib::cout << "CONNECTION TIMEOUT \n";
            hwlib::cout << "!----------------!\n";
            return;
         }
      }
      if(hwlib::now_us()>=(connection_timeout+4000)){
      }
   }

   uint16_t byte_index = 32'768;
   for(unsigned int i=0;i<16;i++){
   if(byte & (byte_index >> i)){
      hwlib::cout << "1";
   }
   else{
      hwlib::cout << "0";
   }
}
   hwlib::cout << "\n";
   hwlib::cout << byte << "   BYTE'S DONE \n";
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
