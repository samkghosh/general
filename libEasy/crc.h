/*
 * crc.h - routines for implementing CRC
 */

#ifndef LOBYTE
#define LOBYTE(x) ((unsigned char)((x) & 0xFF))
#endif
#ifndef HIBYTE
#define HIBYTE(x) ((unsigned char)((x) >> 8))
#endif
unsigned short icrc1(unsigned short crc, unsigned char onech);
void initcrc();
unsigned short icrc(unsigned short crc, unsigned char *bufptr,
					unsigned long len, short jinit, int jrev);
