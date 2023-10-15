#ifndef CARD_H
#define CARD_H
#include <string_view>

void WaitForNewCard();
bool CardIsPresent();

extern std::string_view gamename;
extern std::string_view gameid;

inline bool IsIrCardInserted() {
    return gameid[0] == 'I';
}

#endif //CARD_H
