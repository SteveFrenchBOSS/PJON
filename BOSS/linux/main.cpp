
#include "boss_pjon_serial.hpp"

#include <iostream>

using namespace std::string_literals;

const uint8_t ID = 45;

const uint32_t BAUD = 115200;
const std::string SERIAL_FILE = "/dev/ttySO1"; // todo:

int main()
{
    try
    {
        // This is basically the
        BOSS::PJONSerial ser = BOSS::PJONSerial::Init(SERIAL_FILE, BAUD);

        std::cout << ser.getString() << std::endl;
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}