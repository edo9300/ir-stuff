#include "ir.h"
#include "auxspi_core.h"
#include <span>

void IrInit() {

}

void IrDeInit() {

}

int32_t IrWriteSomethingInt(const uint8_t* data, size_t len) {
    auxspi_open(Rate_1MHz);
    auxspi_write(0x02);
    for(auto byte : std::span{data, len}){
        auxspi_write(static_cast<uint8_t>(byte));
    }
    auxspi_end_command();
    auxspi_close();
    return 0;
}

int32_t IrReadSomethingInt(uint8_t* out, size_t len) {
    auxspi_open(Rate_1MHz);
    auxspi_write(0x01);
    auxspi_write(len);
    for(auto& val : std::span{out, len}) {
        val = static_cast<uint8_t>(auxspi_read());
    }
    auxspi_end_command();
    auxspi_close();
    return len;
}
