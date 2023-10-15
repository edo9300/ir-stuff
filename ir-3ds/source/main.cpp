#include <malloc.h>
#include <stdio.h>
#include <3ds.h>
#include <string.h>
#include <stdio.h>
#include <array>
#include "ir.h"

#define VBlankAndCheckBreak() do { \
hidScanInput(); \
if((hidKeysHeld() & (KEY_START | KEY_SELECT)) == (KEY_START | KEY_SELECT)) \
    goto exit;\
gspWaitForVBlank(); \
gfxSwapBuffers(); \
} while(0)

int main(void) {
    srvInit();        // services
    hidInit();    // input
    gfxInitDefault();
    consoleInit(GFX_BOTTOM, NULL);

    IrInit();

    printf("Press A to send, press B to receive\n");
    printf("Start + Select to exit.\n");

    while(aptMainLoop()) {
        VBlankAndCheckBreak();
        u32 kHeld = hidKeysHeld();
        if(kHeld & KEY_A){
            while(true){
                printf("Sending\n");
                char to_write[] = "abc";
                (void)IrWriteSomething(to_write, sizeof(to_write));
                VBlankAndCheckBreak();
            }
        } else if(kHeld & KEY_B){
            printf("Receiving\n");
            while(true){
                std::array<char, 4> to_read{};
                if(auto received = IrReadSomething(to_read.data(), to_read.size()); received >= 0)
                    printf("%.*s", static_cast<int>(received), to_read.data());
                VBlankAndCheckBreak();
            }
        }
    }

exit:
    printf("Exiting\n");
    IrDeInit();
    hidExit();
    srvExit();
    gfxExit();

	return 0;
}
