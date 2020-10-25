
#include "boss_pjon_serial.hpp"
#include "PJONThroughSerial.h"

using namespace std::string_literals;

namespace BOSS
{

    PJONSerial PJONSerial::Init(const std::string &file, int baud)
    {
        PJONSerial ser;

        auto fh = serialOpen(file.c_str(), baud);

        if (fh == -1)
        {
            throw std::runtime_error(("Couldn't open serial file "s + file).c_str());
        }

        ser.fh = fh;

        return ser;
    }

    int PJONSerial::dataAvail()
    {
        return serialDataAvailable(fh);
    }

    char PJONSerial::getChar()
    {
        return serialGetCharacter(fh);
    }

    std::byte PJONSerial::getByte()
    {
        std::byte b{(uint8_t)serialGetCharacter(fh)};
        return b;
    }

    std::string PJONSerial::getString()
    {
        auto avail = dataAvail();
        std::string ret(avail, '\0');
        for (auto i = 0; i < avail; i++)
        {
            ret[i] = getChar();
        }
        return ret;
    }

    PJONSerial::~PJONSerial()
    {
        close(fh);
    }
}