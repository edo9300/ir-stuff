/*
 * savegame_manager: a tool to backup and restore savegames from Nintendo
 *  DS cartridges. Nintendo DS and all derivative names are trademarks
 *  by Nintendo. EZFlash 3-in-1 is a trademark by EZFlash.
 *
 * auxspi_core.inc: A thin reimplementation of the AUXSPI protocol
 *   (low level functions)
 *
 * Copyright (C) Pokedoc (2010)
 */
/* 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along 
 * with this program; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef AUXSPI_CORE_H
#define AUXSPI_CORE_H

#include <nds/bios.h>
#include <nds/card.h>

static constexpr int ir_delay = 1200;

inline void auxspi_wait_busy()
{
    while (REG_AUXSPICNT & BIT(7));
}

inline void auxspi_wait_wip()
{
    do { REG_AUXSPIDATA = 0; auxspi_wait_busy(); } while (REG_AUXSPIDATA & BIT(0));	// WIP (Write In Progress) ?
}

enum Baudrate : uint8_t {
    Rate_4MHz,
    Rate_2MHz,
    Rate_1MHz,
    Rate_512KHz,
};

inline void auxspi_open(Baudrate rate)
{
    REG_AUXSPICNT = BIT(6) | BIT(13) | BIT(15) | (static_cast<uint8_t>(rate));
	auxspi_wait_busy();
}

inline void auxspi_close()
{
	REG_AUXSPIDATA = 0;
	auxspi_wait_busy();
	REG_AUXSPICNT = 0;
    auxspi_wait_busy();
}

inline void auxspi_end_command()
{
    REG_AUXSPICNT &= ~BIT(6);
	auxspi_wait_busy();
}

inline uint8_t auxspi_transfer(uint8_t out)
{
	REG_AUXSPIDATA = out;
	auxspi_wait_busy();
    swiDelay(ir_delay);
	return REG_AUXSPIDATA;
}

inline void auxspi_write(uint8_t out)
{
    auxspi_transfer(out);
}

inline uint8_t auxspi_read()
{
    return auxspi_transfer(0);
}

inline void init(){
    auxspi_open(Rate_4MHz);
    swiDelay(ir_delay);
}

#endif //AUXSPI_CORE_H
