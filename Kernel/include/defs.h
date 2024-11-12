#ifndef _DEFS_
#define _DEFS_

/* flags for access rights to segments */
#define ACS_PRESENT 0x80 /* segment present in memory */
#define ACS_CSEG 0x18    /* segment in code */
#define ACS_DSEG 0x10    /* segment in data */
#define ACS_READ 0x02    /* segmento in reading */
#define ACS_WRITE 0x02   /* segmento in writing */
#define ACS_IDT ACS_DSEG
#define ACS_INT_386 0x0E /* Interrupt GATE 32 bits */
#define ACS_INT (ACS_PRESENT | ACS_INT_386)
#define ACS_CODE (ACS_PRESENT | ACS_CSEG | ACS_READ)
#define ACS_DATA (ACS_PRESENT | ACS_DSEG | ACS_WRITE)
#define ACS_STACK (ACS_PRESENT | ACS_DSEG | ACS_WRITE)

#endif