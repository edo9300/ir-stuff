#include <nds.h>

#include "card.h"

std::string_view gamename{"????"};
std::string_view gameid{"????"};

void WriteMessage(std::string_view text, bool clearScreen = false, bool topscreen = false);

struct PaddedHeader {
    sNDSHeader header;
    char padding[0x200 - sizeof(sNDSHeader)];
} paddedHeader;

bool UpdateCardInfo(bool updateGlobalHeader = true) {
    cardReadHeader((u8*)&paddedHeader);
    if(*(uint32_t*)&paddedHeader == 0xffffffff)
        return false;
    if(updateGlobalHeader){
        gameid = {paddedHeader.header.gameCode, 4};
        gamename = {paddedHeader.header.gameTitle, 12};
    }
    return true;
}

bool CardIsPresent() {
    if(isDSiMode())
        return (REG_SCFG_MC & BIT(0)) == 0;
    static int counter = 0;

    //poll roughly every 30 frames
    return ((counter++) % 30) != 0 || UpdateCardInfo(true);
}

void WaitCardStabilize(){
    disableSlot1();
    for(size_t i = 0; i < 25; ++i) { swiWaitForVBlank(); }
    enableSlot1();
}

void WaitCard() {
    if(isDSiMode()) {
        if((REG_SCFG_MC & BIT(0)) == 1) {
            WriteMessage("No cartridge detected!\nPlease insert a cartridge to continue!\n", true);
            while((REG_SCFG_MC & BIT(0)) == 1)
                swiWaitForVBlank();
        }
        WaitCardStabilize();
    } else {
        (void)0;
    }
}

void WaitForNewCard(){
    do {
        WaitCard();
        if(UpdateCardInfo())
            break;
        WriteMessage("Cartridge not read properly!\nPlease reinsert it\n", true);
        do {
            swiWaitForVBlank();
        } while(isDSiMode() && CardIsPresent());
    } while(true);
}
