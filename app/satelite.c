/*
 *	uvk5-fw
 *	/app/satelite.c
 *	This file is distributed under Apache License Version 2.0
 *	Copyright (c) 2024 Yao Zi. All rights reserved.
 */

#include<string.h>
#include<stdbool.h>

#include"app/app.h"
#include"app/generic.h"
#include"misc.h"
#include"settings.h"
#include"radio.h"
#include"driver/eeprom.h"

#define EEPROM_BASE		0x0640
#define EEPROM_ENTRY(no)	(EEPROM_BASE + (no) * 22)
#define SLICES(no)		(EEPROM_ENTRY(no) + 14)
#define SLICE_N(no, stage)	(SLICES(no) + (stage))
#define UFREQ(no)		(EEPROM_ENTRY(no) + 6)
#define VFREQ(no)		(EEPROM_ENTRY(no) + 10)
#define ATTR(no)		(EEPROM_ENTRY(no) + 5)
#define NAME(no)		(EEPROM_ENTRY(no) + 0)
#define SLICE_LENGTH(s)		((s) << 2)

bool gSateliteMode, gSateliteDownCounting;
uint16_t gSateliteRemainTime, gSateliteStageRemainTime;
char gSateliteName[6];
uint16_t gSateliteNo;
uint8_t gSateliteStage;

static void
satelite_get_time_and_name(void)
{
	uint8_t t[8] = { 0 };
	EEPROM_ReadBuffer(SLICES(gSateliteNo), t, 8);
	gSateliteRemainTime = 0;
	for (int i = 0; i < 8; i++)
		gSateliteRemainTime += SLICE_LENGTH(t[i]);

	EEPROM_ReadBuffer(NAME(gSateliteNo), gSateliteName, 5);
	return;
}

static void
satelite_set_stage_time(void)
{
	uint8_t s;
	EEPROM_ReadBuffer(SLICE_N(gSateliteNo, gSateliteStage), &s, 1);
	gSateliteStageRemainTime = SLICE_LENGTH(s);
	return;
}

static inline void
do_set_freq(uint32_t rx, uint32_t tx)
{
	(void)tx;
	GENERIC_Set_Freq(0, rx);
}

static void
satelite_set_freq(void)
{
	uint32_t baseUFreq;
	EEPROM_ReadBuffer(UFREQ(gSateliteNo), &baseUFreq, 4);

	gEeprom.CROSS_BAND_RX_TX = CROSS_BAND_OFF;
	gEeprom.DUAL_WATCH = DUAL_WATCH_OFF;
	gEeprom.TX_VFO = 0;
	RADIO_SelectVfos();

	do_set_freq(baseUFreq - 250 * gSateliteStage, 0);
	return;
}

static int
satelite_get_valid_nums(void)
{
	for (int i = 0; i < 72; i++) {
		char b;
		EEPROM_ReadBuffer(EEPROM_ENTRY(i), &b, 1);
		if (b == 0xff)
			return i;
	}
	return 72;
}

static inline int
satelite_enter(void)
{
	if (satelite_get_valid_nums() == 0)
		return -1;

	gSateliteNo			= 0;
	gSateliteStage			= 0;

	satelite_get_time_and_name();
	satelite_set_stage_time();
	satelite_set_freq();

	return 0;
}

static inline void
satelite_exit(void)
{
	gSateliteDownCounting = false;
	return;
}

void
SATELITE_mode_switch(void)
{
	if (gSateliteMode) {
		satelite_exit();
	} else {
		if (satelite_enter())
			return;
	}
	gSateliteMode = !gSateliteMode;
	gUpdateStatus = true;
	gUpdateDisplay = true;
	return;
}

void
SATELITE_next_stage(void)
{
	gSateliteStage++;
	satelite_set_stage_time();
	satelite_set_freq();
	return;
}
