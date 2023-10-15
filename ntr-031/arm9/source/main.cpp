#include <nds.h>
#include <stdio.h>
#include <string>
#include <utility>
#include <vector>
#include <ranges>

#include "card.h"
#include "ir.h"

sNDSHeader header;

#define DISPLAY_COLUMNS 32

PrintConsole upperScreen;
PrintConsole lowerScreen;

void WriteMessage(std::string_view text, bool clearScreen = false, bool topscreen = false) {
    consoleSelect(topscreen ? &upperScreen : &lowerScreen);
    if(clearScreen)
        consoleClear();
    if(text.size() <= DISPLAY_COLUMNS) {
        iprintf("%.*s", text.size(), text.data());
        return;
    }

    size_t curColumn = 0;
    bool first = true;
    for(const auto row : std::views::split(text, '\n')) {
        if(!std::exchange(first, false)){
            puts("");
            curColumn = 0;
        }
        for(const auto word : std::views::split(row, ' ')) {
            auto nextSize = curColumn + word.size();
            if(nextSize > DISPLAY_COLUMNS){
                puts("");
                curColumn = 0;
                nextSize = word.size();
            }
            auto shouldPrintSpace = ((nextSize + 1) <= DISPLAY_COLUMNS);
            iprintf("%.*s%s", word.size(), word.data(), shouldPrintSpace ? " " : "");
            curColumn += word.size() + shouldPrintSpace;
        }
    }
}

void displayInit() {
    lowerScreen = *consoleDemoInit();
    videoSetMode(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);
    consoleInit(&upperScreen, 3, BgType_Text4bpp, BgSize_T_256x256, 31, 0, true, true);
}

void ShowGameInfo() {
    consoleSelect(&upperScreen);
    consoleSetWindow(&upperScreen, 0, 3, DISPLAY_COLUMNS, 23);
    consoleClear();
    iprintf("Game id: %.*s\nName:    %.*s", gameid.size(), gameid.data(), gamename.size(), gamename.data());
}

void PrintMenu() {
    ShowGameInfo();
}

int main() {
    using namespace std::literals;
    displayInit();
    ShowGameInfo();

    if(isDSiMode() && (REG_SCFG_EXT & BIT(31)) == 0) {
        WriteMessage("Missing permissions, restart\n", true);
        while(true) swiWaitForVBlank();
    }

    WriteMessage("Waiting card\n", true);

    sysSetCardOwner(BUS_OWNER_ARM9);

    while(true) {
        WaitForNewCard();
        PrintMenu();
        if(!IsIrCardInserted()) {
            WriteMessage("Cart is not a cart with an IR chip!\nPlease insert a valid one\n", true);
            do {
                WaitForNewCard();
            } while(!IsIrCardInserted());
        }

        WriteMessage("Press A to send, press B to receive\n", true);

        IrInit();

        do {
            if(!CardIsPresent()) {
                gamename = "????";
                gameid = "????";
                PrintMenu();
                break;
            }
            scanKeys();
            auto keys = keysDown();
            if(keys & KEY_A){
                while(true){
                    WriteMessage("Sending\n");
                    char to_write[] = "abc";
                    IrWriteSomething(to_write, sizeof(to_write));
                    swiWaitForVBlank();
                }
            } else if(keys & KEY_B){
                WriteMessage("Receiving\n");
                while(true){
                    std::array<char, 4> to_read{};
                    IrReadSomething(to_read.data(), to_read.size());
                    iprintf("%.*s", to_read.size(), to_read.data());
                    swiWaitForVBlank();
                }
            }
            swiWaitForVBlank();
        } while(true);
    }
    return 0;
}
