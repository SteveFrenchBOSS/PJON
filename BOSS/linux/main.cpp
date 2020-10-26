
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "spdlog/spdlog.h"

#include <boost/program_options.hpp>

// This needs to be the last thing you include for some fucked up reason...
#include <PJONThroughSerial.h>

using namespace std::string_literals;
namespace po = boost::program_options;

// This needs to be in the same file beacause of the
// *terrible* way PJON_LINUX_Interface is written
class serial_raii
{
private:
    int _fh = 0;
    serial_raii(int fh) { _fh = fh; }

public:
    static serial_raii init(const char *file, int baud)
    {
        auto fh = serialOpen(file, baud);

        if (fh == -1)
        {
            spdlog::error("Couldnt open file {}", file);
            throw std::runtime_error("Couldn't open serial file");
        }

        return serial_raii(fh);
    }
    ~serial_raii() { close(_fh); }
    int get_file_handle() { return _fh; }
};

const uint8_t ID = 45;

const uint32_t BAUD = 115200;
const std::string SERIAL_FILE = "/dev/ttySO1"; // todo:

PJONThroughSerial bus(ID);

void receiver_function(uint8_t *payload,
                       uint16_t length,
                       const PJON_Packet_Info &packet_info)
{
    /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
    std::stringstream ss;
    ss << "payload :" << std::hex;
    for (auto i = 0; i < length; i++)
    {
        ss << payload[i];
    }
    ss << " packet info: ";
    ss << packet_info.header;

    std::cout << ss.str() << std::endl;
};

int main(int argc, char *argv[])
{
    spdlog::info("Command line arguments:");
    for (auto i = 0; i < argc; i++)
    {
        spdlog::info(argv[i]);
    }

    po::options_description desc("Allowed command line arguments");
    desc.add_options()("PJON ID", po::value<uint8_t>(), "Bus ID to use for PJON");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    spdlog::info("Setting reciever");

    bus.set_receiver(receiver_function);

    spdlog::info("Opening serial... \n");
    try
    {
        serial_raii serial = serial_raii::init(SERIAL_FILE.c_str(), BAUD);

        spdlog::info("Setting serial");
        bus.strategy.set_serial((uint16_t)serial.get_file_handle());
        bus.strategy.set_baud_rate(BAUD);

        spdlog::info("Opening bus... \n");
        bus.begin();
        spdlog::info("Attempting to roll bus... \n");
        bus.update();
        spdlog::info("Attempting to receive from bus... \n");
        bus.receive();
        spdlog::info("Success! \n");

        while (true)
        {
            bus.update();
            bus.receive();
        }

        return 0;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what();
        return -1;
    }
}