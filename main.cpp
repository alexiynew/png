#include <iostream>
#include <iomanip>

#include "PNGImage.h"

int main()
{
    std::cout << "hello, PNG!" << std::endl;
    
    std::string file_name = "test1.png";
    
    png::PNGImage png;
    if(!png.open(file_name))
    {
        std::cerr << "Can't load " << file_name << std::endl;
    }

    return 0;
   
}