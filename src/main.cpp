#include<iostream>
#include<fstream>
#include<string>


// <<<--header files->>>
#include "ram_reader.hpp"

int main(){
    
    MemInfo info=read_mem_info();
    std::cout<<"used Ram:"<<info.total_kb / 1048576.0<<" Gb \n";
    std::cout<<"used Ram:"<<info.available_kb/ 1048576.0<<" Gb \n";
    std::cout<<"used Ram:"<<info.used_kb / 1048576.0<<" Gb \n";
    
return 0;
}
