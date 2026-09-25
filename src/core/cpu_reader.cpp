



#include "cpu_reader.hpp"

#include<iostream>
#include <istream>
#include <sstream>
#include <fstream>
#include<string>

cpuInfo read_cpu_info(){
    cpuInfo info;


    std::ifstream file("/proc/cpuinfo");
    std::string line;

    while(std::getline(file,line)){
        if(line.rfind("model name",0)==0){


            auto pos=line.find(":");
            if(pos==std::string::npos)continue;
            std::string value=line.substr(pos+1);
            info.model_name=value.substr(value.find_first_not_of(" \t"));
          
            break;
        }
    }
    return info;
}
