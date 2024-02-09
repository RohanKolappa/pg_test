#ifndef __EEPROM_H__
#define __EEPROM_H__

#ifdef __KERNEL__
extern int eeprom_init(void);
extern void eeprom_exit(void);
extern int setEpromBytes(int where, char *bytes, int length);
#define getEpromBytes(w, b, l) do{}while(0)

extern int eeprom_ioctl(unsigned int cmd, unsigned long data);

extern int setMac(char *macstr);
extern char *getMac(void);
extern char *getSerialNumber(void);
extern int setSerialNumber(char *str);
extern unsigned char *getLanMAC(void);
extern unsigned char *getEtherMAC(void);
extern int setMacAddress(char *interfaceName, int unitNum, char *macbytes);
extern int setKeyboardData(short scancode);
extern short getKeyboardData(void);
void eepromDump(int unit);

extern int setHorizontalShift(short val);
extern short getHorizontalShift(void);
extern int setVerticalShift(short val);
extern short getVerticalShift(void);
extern int setPhaseAdjust(short val);
extern short getPhaseAdjust(void);

#endif /* __KERNEL__ */

#define EEPROM_MAC_START                0
#define EEPROM_SERIALNO_START           32
#define EEPROM_KBOARD_START             30
#define EEPROM_HDELAY_START             28
#define EEPROM_VDELAY_START             26
#define EEPROM_PSHIFT_START             24


#endif /* __EEPROM_H__ */
