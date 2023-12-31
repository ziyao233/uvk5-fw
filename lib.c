/*
 *	uvk5-fw
 *	/lib.c
 *	This file is distributed under Apache License Version 2.0
 *	Copyright (c) 2023 Yao Zi. All rights reserved.
 */

void *
memset(void *s, int c, unsigned long int n)
{
	char *p = s;
	for (unsigned long int i = 0; i < n; i++)
		p[i] = (char)c;
	return s;
}

void *
memcpy(void *dst, const void *src, unsigned long int n)
{
	void *d = dst;
	while (n--)
		*((char *)dst++) = *((char *)src++);
	return d;
}

char *
strcpy(char *dst, const char *src)
{
	char *d = dst;
	while (*src) {
		*(dst++) = *(src++);
	}
	*dst = '\0';
	return d;
}

unsigned long int
strlen(const char *s)
{
	unsigned long int l = 0;
	while (*(s++))
		l++;
	return l;
}
