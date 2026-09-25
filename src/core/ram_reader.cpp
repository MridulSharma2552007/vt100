#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include "ram_reader.hpp"




MemInfo read_mem_info(){
    MemInfo info;


    std::ifstream file("/proc/meminfo");
    std::string line;


    while(std::getline(file,line)){

        if(line.rfind("MemTotal:",0)==0){
            std::istringstream iss(line);
            std::string label,unit;
            long value;

            iss>> label>> value >>unit;

            info.total_kb=value;
        }

         else if(line.rfind("MemAvailable:",0)==0){
            std::istringstream iss(line);
            std::string unit, label;
            long value;

            iss>> label>>value>>unit;


            info.available_kb=value;
         }
    }
    info.used_kb=info.total_kb - info.available_kb;

    return info;
}