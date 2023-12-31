/*
 *	uvk5-fw
 *	/include/string.h
 *	This file is distributed under Apache License Version 2.0
 *	Copyright (c) 2023 Yao Zi. All rights reserved.
 */

#ifndef __STRING_H_INC__
#define __STRING_H_INC__

#include<stddef.h>

void *memset(void *s, int c, unsigned long int n);
void *memcpy(void *dst, const void *src, unsigned long int n);
char *strcpy(char *dst, const char *src);
unsigned long int strlen(const char *s);

#endif	// __STRING_H_INC__
