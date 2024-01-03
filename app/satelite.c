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
#include"driver/keyboard.h"

#define EEPROM_BASE		0x0640
#define EEPROM_ENTRY(no)	(EEPROM_BASE + (no) * 24)
#define SLICES(no)		(EEPROM_ENTRY(no) + 14)
#define SLICE_N(no, stage)	(SLICES(no) + (stage))
#define UFREQ(no)		(EEPROM_ENTRY(no) + 6)
#define VFREQ(no)		(EEPROM_ENTRY(no) + 10)
#define ATTR(no)		(EEPROM_ENTRY(no) + 5)
#define NAME(no)		(EEPROM_ENTRY(no) + 0)
#define TIME(no)		(EEPROM_ENTRY(no) + 22)

#define SLICE_LENGTH(s)		((s) << 2)

bool gSateliteMode, gSateliteDownCounting;
uint16_t gSateliteRemainTime, gSateliteStageRemainTime, gSateliteTotalTime;
char gSateliteName[6];
uint16_t gSateliteNo, gSateliteNum;
uint8_t gSateliteStage;
float gTimeScale;

static void
satelite_get_time_and_name(void)
{
	EEPROM_ReadBuffer(TIME(gSateliteNo), &gSateliteRemainTime, 2);
	gSateliteTotalTime = gSateliteRemainTime;

	EEPROM_ReadBuffer(NAME(gSateliteNo), gSateliteName, 5);
	return;
}

static void
satelite_set_stage_time(void)
{
	if (gSateliteStage >= 8) {
		gSateliteStageRemainTime = 65535;
		return;
	}

	uint8_t s;
	EEPROM_ReadBuffer(SLICE_N(gSateliteNo, gSateliteStage), &s, 1);
	gSateliteStageRemainTime = (uint16_t)(SLICE_LENGTH(s) * gTimeScale);
	return;
}

static inline void
do_set_freq(uint32_t rx, uint32_t tx)
{
	GENERIC_Set_Freq(0, rx);
	if (tx < rx) {
		gCurrentVfo->FREQUENCY_DEVIATION_SETTING =
							FREQUENCY_DEVIATION_SUB;
		gCurrentVfo->FREQUENCY_OF_DEVIATION = rx - tx;
	} else {
		gCurrentVfo->FREQUENCY_DEVIATION_SETTING =
							FREQUENCY_DEVIATION_ADD;
		gCurrentVfo->FREQUENCY_OF_DEVIATION = tx - rx;
	}

	return;
}

static void
satelite_set_ctcss(void)
{
	uint8_t attr;
	EEPROM_ReadBuffer(ATTR(gSateliteNo), &attr, 1);
	attr &= 0x1f;

	FREQ_Config_t *p = &gTxVfo->ConfigTX;
	if (attr) {
		p->CodeType	= CODE_TYPE_CONTINUOUS_TONE;
		p->Code		= attr - 1;
	} else {
		p->CodeType	= CODE_TYPE_OFF;
		p->Code		= 0;
	}

	return;
}

static void
satelite_set_freq(void)
{
	uint32_t uFreq, vFreq;
	EEPROM_ReadBuffer(UFREQ(gSateliteNo), &uFreq, 4);
	EEPROM_ReadBuffer(VFREQ(gSateliteNo), &vFreq, 4);

	gEeprom.CROSS_BAND_RX_TX = CROSS_BAND_OFF;
	gEeprom.DUAL_WATCH = DUAL_WATCH_OFF;
	gEeprom.TX_VFO = 0;
	RADIO_SelectVfos();

	do_set_freq(uFreq - 250 * gSateliteStage,
		    vFreq + 250 *
		    ((gSateliteStage >= 2) + (gSateliteStage >= 7)));
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

static void
satelite_load_data(void)
{
	satelite_get_time_and_name();
	satelite_set_stage_time();
	satelite_set_freq();
	satelite_set_ctcss();
	return;
}

static inline int
satelite_enter(void)
{
	gSateliteNum = satelite_get_valid_nums();
	if (!gSateliteNum)
		return -1;

	gSateliteNo			= 0;
	gSateliteStage			= 0;

	satelite_load_data();

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
SATELITE_start(void)
{
	if (gSateliteRemainTime != gSateliteTotalTime) {
		gTimeScale = (gSateliteRemainTime + 0.) / gSateliteTotalTime;
		satelite_set_stage_time();
	} else {
		gTimeScale = 1.;
	}
	satelite_set_stage_time();
	gSateliteDownCounting = 1;
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

void
SATELITE_updown_key(bool bKeyPressed, bool bKeyHeld, int adj)
{
	if (bKeyHeld || !bKeyPressed)
		return;

	if (gSateliteDownCounting)
		return;
	if (gWasFKeyPressed) {
		gWasFKeyPressed = false;
		int32_t adjusted = (int32_t)gSateliteRemainTime + adj * 15;
		if (adjusted <= 0 || adjusted > 65535)
			return;
		else
			gSateliteRemainTime = adjusted;
	} else {
		gSateliteNo = (gSateliteNo + adj + gSateliteNum) % gSateliteNum;
		gSateliteStage = 0;
		satelite_load_data();
	}
	gUpdateDisplay = true;
	return;
}

void
SATELITE_skip(void)
{
	if (gSateliteStage < 8) {
		gSateliteRemainTime -= gSateliteStageRemainTime;
		SATELITE_next_stage();
		gUpdateDisplay = true;
	}
	return;
}
