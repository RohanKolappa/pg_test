/*
 * keymap.hh
 *
 *  Created on: Dec 14, 2010
 *      Author: rkale
 */

#ifndef KEYMAP_HH_
#define KEYMAP_HH_

class CKeymap
{
public:
  bool KeySymToVirtualCode(UINT32 keySym, BYTE *vkCode);
  bool VirtualCodeToKeySym(UINT32 *keySym, BYTE vkCode);

  bool KeySymToUnicodeChar(UINT32 keySym, WCHAR *ch);

  bool UnicodeCharToKeySym(WCHAR ch, UINT32 *keySym);
};


#endif /* KEYMAP_HH_ */
