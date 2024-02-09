/*
 * xkb_utils.c
 *
 *  Created on: Nov 12, 2013
 *      Author: rkale
 */

#include "vlog.h"
#include "xkb_utils.h"
#include "hal_hwinfo.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define _XKBCOMMON_COMPAT_H
#include "xkbcommon/xkbcommon.h"

/* The meaning of the input_event 'value' field. */
#define KEY_STATE_RELEASE     0
#define KEY_STATE_PRESS       1
#define KEY_STATE_REPEAT      2

#define COPYINT32(outbuf, val) {  \
        *((unsigned char *)(outbuf) + 0) = (val) & 0xff; \
        *((unsigned char *)(outbuf) + 1) = ((val) >> 8) & 0xff; \
        *((unsigned char *)(outbuf) + 2) = ((val) >> 16) & 0xff; \
        *((unsigned char *)(outbuf) + 3) = ((val) >> 24) & 0xff; \
}


char *LangId[9][3] = {
    {"us", "US"},
    {"de", "German"},
	{"fr", "French"},
	{"be", "Belgian French"},
	{"es", "Spanish"},
	{"nl", "Dutch"},
    {"it", "Italian"},
    {"pt", "Portuguese"},
    {"ru", "Russian"}

};

static void SetDeadKeyMapReference(CLNT *clnt,const char *layout)
{
    int  i=0;
    // clean up previous memory if any
    if(clnt->xkb_info.ptrKeySymToDeadKeyCharStruct)
    {
        free(clnt->xkb_info.ptrKeySymToDeadKeyCharStruct);
        clnt->xkb_info.ptrKeySymToDeadKeyCharStruct = NULL;
    }

    clnt->xkb_info.KeySymToDeadKeyCharStructSize = 0;

    // if layout has been changed 
    if(strcmp(layout,clnt->xkb_info.keyboardLayout))
    {
        memset(clnt->xkb_info.keyboardLayout,0,sizeof(clnt->xkb_info.keyboardLayout));
        strcpy(clnt->xkb_info.keyboardLayout,layout);

        if(!strcmp(layout,"de"))// German_Germany
        {
            clnt->xkb_info.ptrKeySymToDeadKeyCharStruct = (struct _KeySymToDeadKeyCharStruct*) malloc(sizeof(struct _KeySymToDeadKeyCharStruct) * sizeof(KeySymToDeadKeyCharStruct_German));
            if(clnt->xkb_info.ptrKeySymToDeadKeyCharStruct != NULL)
            {
                clnt->xkb_info.KeySymToDeadKeyCharStructSize = sizeof(KeySymToDeadKeyCharStruct_German) / sizeof(struct _KeySymToDeadKeyCharStruct) - 1;
                for(i=0;i<=clnt->xkb_info.KeySymToDeadKeyCharStructSize;i++)
                {
                    clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[i].KSTDKC.DeadKeyKeySym = KeySymToDeadKeyCharStruct_German[i].KSTDKC.DeadKeyKeySym;
                    clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[i].KSTDKC.CharKeySym = KeySymToDeadKeyCharStruct_German[i].KSTDKC.CharKeySym;
                    clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[i].KSTDKC.DeadKeyCharKeySym = KeySymToDeadKeyCharStruct_German[i].KSTDKC.DeadKeyCharKeySym;

                }

            }
        }

        if(!strcmp(layout,"fr"))// French_France
        {
            clnt->xkb_info.ptrKeySymToDeadKeyCharStruct = (struct _KeySymToDeadKeyCharStruct*) malloc(sizeof(struct _KeySymToDeadKeyCharStruct) * sizeof(KeySymToDeadKeyCharStruct_French));
            if(clnt->xkb_info.ptrKeySymToDeadKeyCharStruct != NULL)
            {
                clnt->xkb_info.KeySymToDeadKeyCharStructSize = sizeof(KeySymToDeadKeyCharStruct_French) / sizeof(struct _KeySymToDeadKeyCharStruct) - 1;
                for(i=0;i<=clnt->xkb_info.KeySymToDeadKeyCharStructSize;i++)
                {
                    clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[i].KSTDKC.DeadKeyKeySym = KeySymToDeadKeyCharStruct_French[i].KSTDKC.DeadKeyKeySym;
                    clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[i].KSTDKC.CharKeySym = KeySymToDeadKeyCharStruct_French[i].KSTDKC.CharKeySym;
                    clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[i].KSTDKC.DeadKeyCharKeySym = KeySymToDeadKeyCharStruct_French[i].KSTDKC.DeadKeyCharKeySym;

                }

            }
        }

        if(!strcmp(layout,"be") || !strcmp(layout,"es") || !strcmp(layout,"pt"))// French_Belgium , Spanish_Spain and Portuguese Layout
        {
            clnt->xkb_info.ptrKeySymToDeadKeyCharStruct = (struct _KeySymToDeadKeyCharStruct*) malloc(sizeof(struct _KeySymToDeadKeyCharStruct) * sizeof(KeySymToDeadKeyCharStruct_BelgianSpanishPortuguese));
            if(clnt->xkb_info.ptrKeySymToDeadKeyCharStruct != NULL)
            {
                clnt->xkb_info.KeySymToDeadKeyCharStructSize = sizeof(KeySymToDeadKeyCharStruct_BelgianSpanishPortuguese) / sizeof(struct _KeySymToDeadKeyCharStruct) - 1;
                for(i=0;i<=clnt->xkb_info.KeySymToDeadKeyCharStructSize;i++)
                {
                    clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[i].KSTDKC.DeadKeyKeySym = KeySymToDeadKeyCharStruct_BelgianSpanishPortuguese[i].KSTDKC.DeadKeyKeySym;
                    clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[i].KSTDKC.CharKeySym = KeySymToDeadKeyCharStruct_BelgianSpanishPortuguese[i].KSTDKC.CharKeySym;
                    clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[i].KSTDKC.DeadKeyCharKeySym = KeySymToDeadKeyCharStruct_BelgianSpanishPortuguese[i].KSTDKC.DeadKeyCharKeySym;

                }

            }
        }

        if(!strcmp(layout,"nl"))// Dutch Netherlands
        {
            clnt->xkb_info.ptrKeySymToDeadKeyCharStruct = (struct _KeySymToDeadKeyCharStruct*) malloc(sizeof(struct _KeySymToDeadKeyCharStruct) * sizeof(KeySymToDeadKeyCharStruct_Dutch_Netherlands));
            if(clnt->xkb_info.ptrKeySymToDeadKeyCharStruct != NULL)
            {
                clnt->xkb_info.KeySymToDeadKeyCharStructSize = sizeof(KeySymToDeadKeyCharStruct_Dutch_Netherlands) / sizeof(struct _KeySymToDeadKeyCharStruct) - 1;
                for(i=0;i<=clnt->xkb_info.KeySymToDeadKeyCharStructSize;i++)
                {
                    clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[i].KSTDKC.DeadKeyKeySym = KeySymToDeadKeyCharStruct_Dutch_Netherlands[i].KSTDKC.DeadKeyKeySym;
                    clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[i].KSTDKC.CharKeySym = KeySymToDeadKeyCharStruct_Dutch_Netherlands[i].KSTDKC.CharKeySym;
                    clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[i].KSTDKC.DeadKeyCharKeySym = KeySymToDeadKeyCharStruct_Dutch_Netherlands[i].KSTDKC.DeadKeyCharKeySym;

                }

            }
        }

        if(!strcmp(layout,"it"))// Italian Italy
        {
            // since italian italy keyboard layout does not have any dead key , so no need to create a dead key map reference.
            VLOG_INFO_CAT(VLOG_KBM_KB, "No dead key map reference is required for Italian Italy keyboard layout. \n");
        }

        if(!strcmp(layout,"ru"))// Russian Russia
        {
            // since Russian keyboard layout does not have any dead key , so no need to create a dead key map reference.
            VLOG_INFO_CAT(VLOG_KBM_KB, "No dead key map reference is required for Russian keyboard layout. \n");
        }

    }
}

static int IsModifier(unsigned int keysym)
{
	int min = 0;
	int max = sizeof(Modifiers) / sizeof(unsigned int) - 1;
	int mid;
	int found = 0;

	//binary search in table 
	while (max >= min) {
		mid = (min + max) / 2;
		if (Modifiers[mid] < keysym)
			min = mid + 1;
		else if (Modifiers[mid] > keysym)
			max = mid - 1;
		else {
			//found it 
			found = 1;
			break;
		}
	}

	return found;
}

static unsigned int XKKeySymToDeadKeyChar(CLNT *clnt,unsigned int keysym,unsigned int DeadKeyKeySym)
{
	int min = 0;
	int max = clnt->xkb_info.KeySymToDeadKeyCharStructSize;
	unsigned int DeadKeyCharKeySym = XKB_KEY_VoidSymbol;

	if(clnt->xkb_info.ptrKeySymToDeadKeyCharStruct != NULL )
	{
		for(min=0;min<=max;min++)
		{
			if ((clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[min].KSTDKC.DeadKeyKeySym == DeadKeyKeySym) && (clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[min].KSTDKC.CharKeySym == keysym) )
			{
				DeadKeyCharKeySym =  clnt->xkb_info.ptrKeySymToDeadKeyCharStruct[min].KSTDKC.DeadKeyCharKeySym;
				break;
			}
		}
	}

	return DeadKeyCharKeySym;
}

/* Reading the keyboard layout */
static int get_keyboard_layout( HAL *hp)
{
    FILE *fp;
    char buffer[64];
    int fsize = 0;
    int result = 0;
    int ind = 0;
    char buf[256];
    char *cp;
    sprintf(buf, "/etc/config/rxkblayout_%d.txt", hal_get_board_number(hp));

    fp = fopen(buf, "r");
    if (fp == NULL)
        return 0; //return default English-US layout
    
    fgets(buffer, 64, fp);
    if ((cp = strchr(buffer, '\n')) != 0)
        *cp = '\0';
    fclose(fp);

    while (strcmp(buffer, LangId[ind][1])) 
        ind++; 
        
    VLOG_INFO_CAT(VLOG_KBM_KB, "returning langauge id = %d \n", ind);
    return ind;
  
}

void xkb_CleanUp(CLNT *clnt)
{
	if(clnt->xkb_info.ptrKeySymToDeadKeyCharStruct)
	{
		free(clnt->xkb_info.ptrKeySymToDeadKeyCharStruct);
		clnt->xkb_info.ptrKeySymToDeadKeyCharStruct = NULL;
	}

	clnt->xkb_info.context = NULL;
	clnt->xkb_info.DeadKeyKeySym = 0;
	clnt->xkb_info.keymap = NULL;
	clnt->xkb_info.ModifierKeySym = 0;
	clnt->xkb_info.ProcessAltGr = 0;
	clnt->xkb_info.state = NULL;
	clnt->xkb_info.KeySymToDeadKeyCharStructSize = 0;
}

void clnt_init_xkb(CLNT *clnt, HAL *hp)
{
    enum xkb_context_flags ctx_flags;
    struct xkb_context *context;
    struct xkb_state *state;
    struct xkb_keymap *keymap;
    int langid = 0;

    clnt->xkb_info.context = NULL;
	clnt->xkb_info.DeadKeyKeySym = 0;
	clnt->xkb_info.ModifierKeySym = 0;
	clnt->xkb_info.ProcessAltGr = 0;
    //init_keyboard_layout(hp);

    // Create context
    ctx_flags = XKB_CONTEXT_NO_DEFAULT_INCLUDES | XKB_CONTEXT_NO_ENVIRONMENT_NAMES;
    context = xkb_context_new(ctx_flags);
    if (context == NULL) {
        VLOG_WARNING_CAT(VLOG_KBM_KB, "Failed to create XKB context");
        return;
    }
    clnt->xkb_info.context = context;
    VLOG_INFO_CAT(VLOG_KBM_KB, "Created XKB context %p", context);

    // Add path to xkb data files
    xkb_context_include_path_append(context, "/usr/share/X11/xkb");

    // Create keymap
    struct xkb_rule_names rmlvo = {
        .rules = NULL,
        .model = NULL,
        .layout = "us",
        .variant = NULL,
        .options = NULL
    };
   
    langid =  get_keyboard_layout(hp);
    rmlvo.layout = LangId[langid][0];
	SetDeadKeyMapReference(clnt,rmlvo.layout);

    VLOG_INFO_CAT(VLOG_KBM_KB, "Configuring  Language layout %s \n", LangId[langid][1]);

    keymap = xkb_keymap_new_from_names(context, &rmlvo, 0);
    if (keymap == NULL) {
        VLOG_WARNING_CAT(VLOG_KBM_KB, "Failed to create XKB keymap");
        return;
    }
    VLOG_INFO_CAT(VLOG_KBM_KB, "Created XKB keymap %p", keymap);
    clnt->xkb_info.keymap = keymap;

    // Get state (TODO: this should move to per keyboard state)
    state = xkb_state_new(keymap);
    if (state == NULL) {
        VLOG_WARNING_CAT(VLOG_KBM_KB, "Failed to create XKB state");
        return;
    }
    VLOG_INFO_CAT(VLOG_KBM_KB, "Created XKB state %p", state);
    clnt->xkb_info.state = state;
}

static void process_keycode(struct xkb_state *state, xkb_keycode_t keycode)
{
    struct xkb_keymap *keymap;
    unsigned int nsyms;
    const xkb_keysym_t *syms;
    int i;
    char s[16];

    keymap = xkb_state_get_keymap(state);

    xkb_keysym_t sym = xkb_state_key_get_one_sym(state, keycode);
    xkb_keysym_get_name(sym, s, sizeof(s));
    //printf("keysym [ %-*s ] ", (int) sizeof(s), s);
    VLOG_INFO_CAT(VLOG_KBM_KB, "KeySym %x [ %s ]", sym, s);

#if 0
    nsyms = xkb_state_key_get_syms(state, keycode, &syms);

    if (nsyms <= 0)
        return;

    if (nsyms == 1) {
        xkb_keysym_t sym = xkb_state_key_get_one_sym(state, keycode);
        xkb_keysym_get_name(sym, s, sizeof(s));
        //printf("keysym [ %-*s ] ", (int) sizeof(s), s);
        VLOG_INFO_CAT(VLOG_KBM_KB, "Single keysym %x [ %-*s ]", sym, (int) sizeof(s), s);
    }
    else {
        //printf("keysyms [ ");
        for (i = 0; i < nsyms; i++) {
            xkb_keysym_get_name(syms[i], s, sizeof(s));
            //printf("%-*s ", (int) sizeof(s), s);
            VLOG_INFO_CAT(VLOG_KBM_KB, "Multi keysym[%d] %x [ %-*s ]",
                    i, syms[i], (int) sizeof(s), s);
        }
        //printf("] ");
    }
#endif
}

static int ProcessKeycode(unsigned char *rfbbuf,
           xkb_keysym_t keycode, int keypress) 
{
    int networkbyte = 0;
    int len = 5;

    networkbyte = htonl(keycode);
    COPYINT32(rfbbuf, networkbyte);
    rfbbuf[4] =  keypress;

    VLOG_INFO_HEXDATA_CAT(VLOG_KBM_RFB, rfbbuf, len);
	return len;
}

int clnt_process_xkb(CLNT *clnt,
        struct input_event *evbuf, int evbuflen,
        unsigned char *xkbbuf)
{
    int i;
    int type, code, value;
    struct xkb_keymap *keymap;
    enum xkb_state_component changed;
	xkb_keycode_t keycode;
    xkb_keysym_t sym = 0;
    char buffer[64];
    unsigned char rfbbuf[5];
    int len = 0;
	unsigned int DeadKeyCharKeySym = XKB_KEY_VoidSymbol;
	clnt->xkb_info.ProcessAltGr = 0;

    if (clnt->xkb_info.state == NULL)
        return 0;

    for (i = 0; i < evbuflen / sizeof(struct input_event); i++) {

        struct input_event *ev = &evbuf[i];

        type = ev->type;
        code = ev->code;
        value = ev->value;

        VLOG_DEBUG_CAT(VLOG_KBM_KB, "Processing an event [%d] type=%d code=%d value=%d",
                i, type, code, value);

        if (type != EV_KEY)
            return len;

        keycode = 8 + code;
        keymap = xkb_state_get_keymap(clnt->xkb_info.state);

        if (value == KEY_STATE_REPEAT && !xkb_keymap_key_repeats(keymap, keycode))
            return 0;

       // if (value != KEY_STATE_RELEASE) {
            // process_keycode(clnt->xkb_info.state, keycode);
            sym = xkb_state_key_get_one_sym(clnt->xkb_info.state, keycode);
            xkb_keysym_get_name(sym, buffer, sizeof(buffer));
            //printf("keysym [ %-*s ] ", (int) sizeof(s), s);
			//VLOG_INFO_CAT(VLOG_KBM_KB, "clnt_process_xkb : KeySym [%x] KeyCode [%x] buffer [%s]", sym,keycode,buffer);
			///Dead key handling 
			if(strstr(buffer,"dead")) 
			{
				clnt->xkb_info.DeadKeyKeySym = sym;
				if(value == KEY_STATE_PRESS)
				{
					VLOG_INFO_CAT(VLOG_KBM_KB, "clnt_process_xkb : === Dead Key detected === KeySym [%x] KeyCode [%x] buffer [%s] KeyState[Press]", sym,keycode,buffer);
				}

				if(value == KEY_STATE_RELEASE)
				{
					VLOG_INFO_CAT(VLOG_KBM_KB, "clnt_process_xkb : === Dead Key detected === KeySym [%x] KeyCode [%x] buffer [%s] KeyState[Release]", sym,keycode,buffer);
				}
			}
			else
			{
				if(clnt->xkb_info.DeadKeyKeySym)
				{
					if(value == KEY_STATE_RELEASE)
					{
						if(sym == XKB_KEY_ISO_Level3_Shift)
							clnt->xkb_info.ProcessAltGr = 1;
					}

					if(IsModifier(sym))
					{
						clnt->xkb_info.ModifierKeySym = sym;
					}
					else
					{
						DeadKeyCharKeySym = XKB_KEY_VoidSymbol;
						if(value == KEY_STATE_PRESS)
						{
							DeadKeyCharKeySym = XKKeySymToDeadKeyChar(clnt,sym,clnt->xkb_info.DeadKeyKeySym);
							if(DeadKeyCharKeySym != XKB_KEY_VoidSymbol)
							{
								memset(buffer,0,sizeof(buffer));
								sym = DeadKeyCharKeySym;
								xkb_keysym_get_name(sym, buffer, sizeof(buffer));
							}

							clnt->xkb_info.DeadKeyKeySym = 0;
							clnt->xkb_info.ModifierKeySym = 0;
						}
					}
				}
			}

			//VLOG_INFO_CAT(VLOG_KBM_KB, "clnt_process_xkb : KeySym [%x] KeyCode [%x] buffer [%s] ", sym,keycode,buffer);
			
      //  }

        if (value == KEY_STATE_RELEASE) {

			//only process dead key characters and not dead key
			if( !clnt->xkb_info.DeadKeyKeySym || clnt->xkb_info.ModifierKeySym || clnt->xkb_info.ProcessAltGr )
			{
				VLOG_INFO_CAT(VLOG_KBM_KB, "clnt_process_xkb : KeySym [%x] KeyCode [%x] buffer [%s] State[Release]", sym,keycode,buffer);
				changed = xkb_state_update_key(clnt->xkb_info.state, keycode, XKB_KEY_UP);
				len =  ProcessKeycode(xkbbuf,sym, XKB_KEY_UP);
			}

        }
		else {
			//only process dead key characters and not dead key
			if(!clnt->xkb_info.DeadKeyKeySym || clnt->xkb_info.ModifierKeySym)
			{
				VLOG_INFO_CAT(VLOG_KBM_KB, "clnt_process_xkb : KeySym [%x] KeyCode [%x] buffer [%s] State[Press]", sym,keycode,buffer);
				changed = xkb_state_update_key(clnt->xkb_info.state, keycode, XKB_KEY_DOWN);
				len = ProcessKeycode(xkbbuf, sym, XKB_KEY_DOWN);
			}

		}

    }
    return len;
}


