#include <3ds.h>

#include "ir.h"

namespace
{
[[gnu::aligned(0x1000)]]
u32 irmemloc[0x1000 / 4];

Result irucmd_GetTransferState(u8 *state)
{
    u32 *cmdbuf = getThreadCommandBuffer();

    cmdbuf[0] = 0x000F0000;

    if(auto ret = svcSendSyncRequest(iruGetServHandle()); ret < 0){
        return ret;
    }

    *state = cmdbuf[2];

    return static_cast<Result>(cmdbuf[1]);
}

}

void IrInit() {
    iruInit(irmemloc, 0x1000);
    IRU_SetBitRate(3);
}

void IrDeInit() {
    iruExit();
}

int32_t IrWriteSomethingInt(const uint8_t* buf, size_t len) {
    if(auto ret = IRU_StartSendTransfer(const_cast<u8*>(buf), len); ret < 0)
        return ret;

    return IRU_WaitSendTransfer();
}

int32_t IrReadSomethingInt(uint8_t* out, size_t len) {
    u8 StatusIR;
    Result res;
    if((res = irucmd_GetTransferState(&StatusIR)) < 0 || StatusIR != 1)
        return res;
    if((res = IRU_StartRecvTransfer(len, 0x0)) < 0)
        return res;
    while(true){
        if((res = irucmd_GetTransferState(&StatusIR)) < 0)
            return res;
        if(StatusIR == 1) {
            u32 received{};
            IRU_WaitRecvTransfer(&received);
            __builtin_memcpy(out, irmemloc, received);
            return received;
        }
    }
}
