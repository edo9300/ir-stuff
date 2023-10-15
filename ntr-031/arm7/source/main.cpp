#include <nds.h>


static bool exitflag = false;

int main() {
	readUserSettings();
	ledBlink(0);

	irqInit();
	// Start the RTC tracking IRQ
	fifoInit();

	SetYtrigger(80);

	installSystemFIFO();

	irqSet(IRQ_VCOUNT, []{ inputGetAndSend(); });
	irqSet(IRQ_VBLANK, []{});

	irqEnable(IRQ_VBLANK | IRQ_VCOUNT);

	setPowerButtonCB([]{ exitflag = true; });

	// Keep the ARM7 mostly idle
	while (!exitflag) {
		if ( 0 == (REG_KEYINPUT & (KEY_SELECT | KEY_START | KEY_L | KEY_R))) {
			exitflag = true;
		}
		swiWaitForVBlank();
	}
	return 0;
}
