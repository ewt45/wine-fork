/*
 * Wine MIDI mapper driver
 *
 * Copyright 	1999, 2000, 2001 Eric Pouech
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 *
 * TODO:
 *	notification has to be implemented
 *	IDF file loading
 */

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "windef.h"
#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"
#include "mmddk.h"
#include "winreg.h"
#include "wine/debug.h"
#include <winsock2.h>

/*
 * Here's how Windows stores the midiOut mapping information.
 *
 * Windows XP form (in HKU) is:
 *
 * [Software\\Microsoft\\Windows\\CurrentVersion\\Multimedia\\MIDIMap]
 * "szPname"="TiMidity port 0"
 * (incomplete)
 *
 * szPname:             name of midiOut device to use.
 *
 * If szPname isn't defined, we use Windows 2000 form (also in HKU):
 *
 * [Software\\Microsoft\\Windows\\CurrentVersion\\Multimedia\\MIDIMap] 988836060
 * "AutoScheme"=dword:00000000
 * "ConfigureCount"=dword:00000004
 * "CurrentInstrument"="Wine OSS midi"
 * "CurrentScheme"="epp"
 * "DriverList"=""
 * "UseScheme"=dword:00000000
 *
 * AutoScheme: 		?
 * CurrentInstrument: 	name of midiOut device to use when UseScheme is 0. Wine uses an extension
 *			of the form #n to link to n'th midiOut device of the system
 * CurrentScheme:	when UseScheme is non null, it's the scheme to use (see below)
 * DriverList:		?
 * UseScheme:		trigger for simple/complex mapping
 *
 * A scheme is defined (in HKLM) as:
 *
 * [System\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Midi\\Schemes\\<nameScheme>]
 * <nameScheme>:	one key for each defined scheme (system wide)
 * under each one of these <nameScheme> keys, there's:
 * [...\\<nameScheme>\\<idxDevice>]
 * "Channels"="<bitMask>"
 * (the default value of this key also refers to the name of the device).
 *
 * this defines, for each midiOut device (identified by its index in <idxDevice>), which
 * channels have to be mapped onto it. The <bitMask> defines the channels (from 0 to 15)
 * will be mapped (mapping occurs for channel <ch> if bit <ch> is set in <bitMask>
 *
 * Further mapping information can also be defined in:
 * [System\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Midi\\Ports\\<nameDevice>\\Instruments\\<idx>]
 * "Definition"="<.idf file>"
 * "FriendlyName"="#for .idx file#"
 * "Port"="<idxPort>"
 *
 * This last part isn't implemented (.idf file support).
 */

WINE_DEFAULT_DEBUG_CHANNEL(midi);
WINE_DECLARE_DEBUG_CHANNEL(winediag);

typedef struct tagMIDIOUTPORT
{
    WCHAR		name[MAXPNAMELEN];
    int			loaded;
    HMIDIOUT		hMidi;
    unsigned short	uDevID;
    LPBYTE		lpbPatch;
    unsigned int	aChn[16];
} MIDIOUTPORT;

typedef	struct tagMIDIMAPDATA
{
    struct tagMIDIMAPDATA*	self;
    MIDIOUTPORT*	ChannelMap[16];
    MIDIOPENDESC	midiDesc;
    BYTE		runningStatus;
    WORD		wCbFlags;
} MIDIMAPDATA;

static	MIDIOUTPORT*	midiOutPorts;
static  unsigned	numMidiOutPorts;
// static  unsigned    idxAndroidPorts = -1; //记录安卓虚拟设备的序号。-1为不使用。
/*
drvOpen 获取环境变量，建立socket连接
modOpen 
    - midiOutOpen 是winmm的函数。其调用最终会进入alsa驱动 winealsa 的 midi_out_open
      但是安卓上alsa不支持midi，所以应该将midiOutOpen去掉，改为建立socket连接。
modData 通过socket发送数据
    - 这里会调用midiOutShortMsg。改成自己的函数，midiOut要不要删掉？
modReset 退出比赛时好像会调用，停止播放。
    - 这里调用midiOutReset，根据alsamidi，其内部其实也是调用modData发送控制器参数
    - 实现了这个之后，退出比赛不会报错了
drvClose 断开socket连接

发送socket数据时，第一个byte放TYPE，后三个byte放evt，d1和d2（经过处理，参考alsamidi的midi_out_data）
*/

#define TYPE_ON_CONNECT      0x12 //初始连接成功时
#define TYPE_SHORT_MSG       0x13 //midiOutShortMsg时
//复制自asoundef.h
#define MIDI_CMD_CONTROL		    0xb0	/**< control change */
#define MIDI_CTL_SUSTAIN            0x40	/**< Sustain pedal */
#define MIDI_CTL_ALL_SOUNDS_OFF		0x78	/**< All sounds off */

static char *androidPort = NULL; //记录socket端口号，字符串
static SOCKET connectSocket = INVALID_SOCKET;
static void connect_socket();
static void disconnect_socket();
static DWORD send_socket_data(MIDIMAPDATA*, DWORD_PTR);
static DWORD send_socket_long_data(DWORD_PTR);



static	BOOL	MIDIMAP_IsBadData(MIDIMAPDATA* mm)
{
    if (!IsBadReadPtr(mm, sizeof(MIDIMAPDATA)) && mm->self == mm)
	return FALSE;
    TRACE("Bad midimap data (%p)\n", mm);
    return TRUE;
}

static BOOL	MIDIMAP_FindPort(const WCHAR* name, unsigned* dev)
{
    for (*dev = 0; *dev < numMidiOutPorts; (*dev)++)
    {
	TRACE("%s\n", wine_dbgstr_w(midiOutPorts[*dev].name));
	if (lstrcmpW(midiOutPorts[*dev].name, name) == 0)
	    return TRUE;
    }
    /* try the form #nnn */
    if (*name == '#' && name[1] >= '0' && name[1] <= '9')
    {
        const WCHAR*  ptr = name + 1;
        *dev = 0;
        do 
        {
            *dev = *dev * 10 + *ptr - '0';
            ptr++;
        } while (*ptr >= '0' && *ptr <= '9');
	if (*dev < numMidiOutPorts)
	    return TRUE;
    }
    return FALSE;
}

static BOOL	MIDIMAP_LoadSettingsDefault(MIDIMAPDATA* mom, const WCHAR* port)
{
    unsigned i, dev = 0;

    if (port != NULL && !MIDIMAP_FindPort(port, &dev))
    {
	ERR("Registry glitch: couldn't find midi out (%s)\n", wine_dbgstr_w(port));
	dev = 0;
    }

    /* this is necessary when no midi out ports are present */
    if (dev >= numMidiOutPorts)
	return FALSE;
    /* sets default */
    for (i = 0; i < 16; i++) mom->ChannelMap[i] = &midiOutPorts[dev];

    return TRUE;
}

static BOOL	MIDIMAP_LoadSettingsScheme(MIDIMAPDATA* mom, const WCHAR* scheme)
{
    HKEY	hSchemesKey, hKey, hPortKey;
    unsigned	i, idx, dev;
    WCHAR       buffer[256], port[256];
    DWORD	type, size, mask;

    for (i = 0; i < 16; i++)	mom->ChannelMap[i] = NULL;

    if (RegOpenKeyA(HKEY_LOCAL_MACHINE,
		    "System\\CurrentControlSet\\Control\\MediaProperties\\PrivateProperties\\Midi\\Schemes",
		    &hSchemesKey))
    {
	return FALSE;
    }
    if (RegOpenKeyW(hSchemesKey, scheme, &hKey))
    {
	RegCloseKey(hSchemesKey);
	return FALSE;
    }

    for (idx = 0; !RegEnumKeyW(hKey, idx, buffer, ARRAY_SIZE(buffer)); idx++)
    {
	if (RegOpenKeyW(hKey, buffer, &hPortKey)) continue;

	size = sizeof(port);
	if (RegQueryValueExW(hPortKey, NULL, 0, &type, (void*)port, &size)) continue;

	if (!MIDIMAP_FindPort(port, &dev)) continue;

	size = sizeof(mask);
	if (RegQueryValueExA(hPortKey, "Channels", 0, &type, (void*)&mask, &size))
	    continue;

	for (i = 0; i < 16; i++)
	{
	    if (mask & (1 << i))
	    {
		if (mom->ChannelMap[i])
		    ERR("Quirks in registry, channel %u is mapped twice\n", i);
		mom->ChannelMap[i] = &midiOutPorts[dev];
	    }
	}
    }

    RegCloseKey(hSchemesKey);
    RegCloseKey(hKey);

    return TRUE;
}

static BOOL	MIDIMAP_LoadSettings(MIDIMAPDATA* mom)
{
    HKEY 	hKey;
    BOOL	ret;

    if (RegOpenKeyA(HKEY_CURRENT_USER,
		    "Software\\Microsoft\\Windows\\CurrentVersion\\Multimedia\\MIDIMap", &hKey))
    {
	ret = MIDIMAP_LoadSettingsDefault(mom, NULL);
    }
    else
    {
	DWORD	type, size, out;
	WCHAR	buffer[256];

	size = sizeof(buffer);
	if (!RegQueryValueExW(hKey, L"szPname", 0, &type, (void*)buffer, &size))
	{
	    ret = MIDIMAP_LoadSettingsDefault(mom, buffer);
	}
	else
	{
	    ret = 2;
	    size = sizeof(out);
	    if (!RegQueryValueExA(hKey, "UseScheme", 0, &type, (void*)&out, &size) && out)
	    {
		size = sizeof(buffer);
		if (!RegQueryValueExW(hKey, L"CurrentScheme", 0, &type, (void*)buffer, &size))
		{
		    if (!(ret = MIDIMAP_LoadSettingsScheme(mom, buffer)))
			ret = MIDIMAP_LoadSettingsDefault(mom, NULL);
		}
		else
		{
		    ERR("Wrong registry: UseScheme is active, but no CurrentScheme found\n");
		}
	    }
	    if (ret == 2)
	    {
		size = sizeof(buffer);
		if (!RegQueryValueExW(hKey, L"CurrentInstrument", 0, &type, (void*)buffer, &size) && *buffer)
		{
		    ret = MIDIMAP_LoadSettingsDefault(mom, buffer);
		}
		else
		{
		    ret = MIDIMAP_LoadSettingsDefault(mom, NULL);
		}
	    }
	}
    }
    RegCloseKey(hKey);

    if (ret && TRACE_ON(midi))
    {
	unsigned	i;

	for (i = 0; i < 16; i++)
	{
	    TRACE("chnMap[%2d] => %d\n",
		  i, mom->ChannelMap[i] ? mom->ChannelMap[i]->uDevID : -1);
	}
    }
    return ret;
}

static void MIDIMAP_NotifyClient(MIDIMAPDATA* mom, WORD wMsg,
				 DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    DriverCallback(mom->midiDesc.dwCallback, mom->wCbFlags, (HDRVR)mom->midiDesc.hMidi,
		   wMsg, mom->midiDesc.dwInstance, dwParam1, dwParam2);
}

static DWORD modOpen(DWORD_PTR *lpdwUser, LPMIDIOPENDESC lpDesc, DWORD dwFlags)
{
    MIDIMAPDATA*	mom = HeapAlloc(GetProcessHeap(), 0, sizeof(MIDIMAPDATA));

    TRACE("(%p %p %08lx)\n", lpdwUser, lpDesc, dwFlags);

    if (!mom) return MMSYSERR_NOMEM;
    if (!lpDesc) {
        HeapFree(GetProcessHeap(), 0, mom);
        return MMSYSERR_INVALPARAM;
    }

    if (MIDIMAP_LoadSettings(mom))
    {
        UINT chn;
        *lpdwUser = (DWORD_PTR)mom;
        mom->self = mom;
        mom->wCbFlags = HIWORD(dwFlags & CALLBACK_TYPEMASK);
        mom->midiDesc = *lpDesc;
        mom->runningStatus = 0;

        for (chn = 0; chn < 16; chn++)
        {
            if (mom->ChannelMap[chn]->loaded) continue;

            //midiOutOpen不会成功，因为alsa找不到seq。所以需要手动创建socket连接并初始化对应内容。(参考alsamidi.c midi_out_open())
            if (androidPort) {
                FIXME("midi调试 安卓没有alsa_seq, midiOutOpen不成功。我们手动初始化。\n");
                if(midiOutPorts[0].hMidi != 0) FIXME("midi调试 手动初始化有潜在问, hMidi不为0!\n");
                mom->ChannelMap[chn]->loaded = 1;
                continue;
            }
            if (midiOutOpen(&mom->ChannelMap[chn]->hMidi, mom->ChannelMap[chn]->uDevID,
                    0L, 0L, CALLBACK_NULL) == MMSYSERR_NOERROR)
                mom->ChannelMap[chn]->loaded = 1;
            else
                mom->ChannelMap[chn]->loaded = -1;

            /* FIXME: should load here the IDF midi data... and allow channel and
            * patch mappings
            */
        }
        MIDIMAP_NotifyClient(mom, MOM_OPEN, 0L, 0L);
        return MMSYSERR_NOERROR;
    }

    HeapFree(GetProcessHeap(), 0, mom);
    return MIDIERR_INVALIDSETUP;
}

static	DWORD	modClose(MIDIMAPDATA* mom)
{
    UINT	i;
    DWORD	ret = MMSYSERR_NOERROR;

    if (MIDIMAP_IsBadData(mom)) 	return MMSYSERR_ERROR;

    for (i = 0; i < 16; i++)
    {
	DWORD	t;
	if (mom->ChannelMap[i] && mom->ChannelMap[i]->loaded > 0)
	{
	    t = midiOutClose(mom->ChannelMap[i]->hMidi);
	    if (t == MMSYSERR_NOERROR)
	    {
		mom->ChannelMap[i]->loaded = 0;
		mom->ChannelMap[i]->hMidi = 0;
	    }
	    else if (ret == MMSYSERR_NOERROR)
		ret = t;
	}
    }
    if (ret == MMSYSERR_NOERROR) {
	MIDIMAP_NotifyClient(mom, MOM_CLOSE, 0L, 0L);
	HeapFree(GetProcessHeap(), 0, mom);
    }
    return ret;
}

static DWORD modLongData(MIDIMAPDATA* mom, LPMIDIHDR lpMidiHdr, DWORD_PTR dwParam2)
{
    WORD	chn;
    DWORD	ret = MMSYSERR_NOERROR;
    MIDIHDR	mh;

    TRACE("midi调试 播放长消息\n");

    if (MIDIMAP_IsBadData(mom))
	return MMSYSERR_ERROR;
    if (!(lpMidiHdr->dwFlags & MHDR_PREPARED))
	return MIDIERR_UNPREPARED;
    if (lpMidiHdr->dwFlags & MHDR_INQUEUE)
	return MIDIERR_STILLPLAYING;

    mh = *lpMidiHdr;
    lpMidiHdr->dwFlags &= ~MHDR_DONE;
    lpMidiHdr->dwFlags |= MHDR_INQUEUE;
    for (chn = 0; chn < 16; chn++)
    {
	if (mom->ChannelMap[chn] && mom->ChannelMap[chn]->loaded > 0)
	{
	    mh.dwFlags = 0;
	    midiOutPrepareHeader(mom->ChannelMap[chn]->hMidi, &mh, sizeof(mh));
	    ret = midiOutLongMsg(mom->ChannelMap[chn]->hMidi, &mh, sizeof(mh));
	    /* As of 2009, wineXYZ.drv's LongData handlers are synchronous */
	    if (!ret && !(mh.dwFlags & MHDR_DONE))
		FIXME("wait until MHDR_DONE\n");
	    midiOutUnprepareHeader(mom->ChannelMap[chn]->hMidi, &mh, sizeof(mh));
	    if (ret != MMSYSERR_NOERROR) break;
	}
    }
    mom->runningStatus = 0;
    lpMidiHdr->dwFlags &= ~MHDR_INQUEUE;
    lpMidiHdr->dwFlags |= MHDR_DONE;
    MIDIMAP_NotifyClient(mom, MOM_DONE, (DWORD_PTR)lpMidiHdr, 0L);
    return ret;
}

static DWORD modData(MIDIMAPDATA* mom, DWORD_PTR dwParam)
{
    BYTE	status = LOBYTE(LOWORD(dwParam));
    WORD	chn;
    DWORD	ret = MMSYSERR_NOERROR;

    if (MIDIMAP_IsBadData(mom))
	return MMSYSERR_ERROR;

    if (status < 0x80)
    {
        if (mom->runningStatus)
        {
            status = mom->runningStatus;
            dwParam = ((LOWORD(dwParam) << 8) | status);
        }
        else
        {
            FIXME("ooch %Ix\n", dwParam);
            return MMSYSERR_NOERROR;
        }
    }
    chn = status & 0x0F;

    if (!mom->ChannelMap[chn]) return MMSYSERR_NOERROR;

    
    switch (status & 0xF0)
    {
    case 0x80:
    case 0x90:
    case 0xA0:
    case 0xB0:
    case 0xC0:
    case 0xD0:
    case 0xE0:
        if (mom->ChannelMap[chn]->loaded > 0)
        {
            /* change channel */
            dwParam &= ~0x0F;
            dwParam |= mom->ChannelMap[chn]->aChn[chn];

            if ((LOBYTE(LOWORD(dwParam)) & 0xF0) == 0xC0 /* program change */ &&
            mom->ChannelMap[chn]->lpbPatch)
            {
                BYTE patch = HIBYTE(LOWORD(dwParam));

                /* change patch */
                dwParam &= ~0x0000FF00;
                dwParam |= mom->ChannelMap[chn]->lpbPatch[patch];
            }

            ret = androidPort 
                ? send_socket_data(mom, dwParam) 
                : midiOutShortMsg(mom->ChannelMap[chn]->hMidi, dwParam);
        }
        mom->runningStatus = status;
        break;
    case 0xF0:
        for (chn = 0; chn < 16; chn++)
        {
            if (mom->ChannelMap[chn]->loaded > 0)
            {
                ret = androidPort 
                    ? send_socket_data(mom, dwParam)
                    : midiOutShortMsg(mom->ChannelMap[chn]->hMidi, dwParam);
            }
        }
        /* system common message */
        if (status <= 0xF7)
            mom->runningStatus = 0;
        break;
    default:
        FIXME("ooch %Ix\n", dwParam);
    }

    return ret;
}

static DWORD modPrepare(MIDIMAPDATA* mom, LPMIDIHDR lpMidiHdr, DWORD_PTR dwSize)
{
    if (MIDIMAP_IsBadData(mom)) return MMSYSERR_ERROR;
    if (dwSize < offsetof(MIDIHDR,dwOffset) || lpMidiHdr == 0 || lpMidiHdr->lpData == 0)
	return MMSYSERR_INVALPARAM;
    if (lpMidiHdr->dwFlags & MHDR_PREPARED)
	return MMSYSERR_NOERROR;

    lpMidiHdr->dwFlags |= MHDR_PREPARED;
    lpMidiHdr->dwFlags &= ~(MHDR_DONE|MHDR_INQUEUE); /* flags cleared since w2k */
    return MMSYSERR_NOERROR;
}

static DWORD modUnprepare(MIDIMAPDATA* mom, LPMIDIHDR lpMidiHdr, DWORD_PTR dwSize)
{
    if (MIDIMAP_IsBadData(mom)) return MMSYSERR_ERROR;
    if (dwSize < offsetof(MIDIHDR,dwOffset) || lpMidiHdr == 0 || lpMidiHdr->lpData == 0)
	return MMSYSERR_INVALPARAM;
    if (!(lpMidiHdr->dwFlags & MHDR_PREPARED))
	return MMSYSERR_NOERROR;
    if (lpMidiHdr->dwFlags & MHDR_INQUEUE)
	return MIDIERR_STILLPLAYING;

    lpMidiHdr->dwFlags &= ~MHDR_PREPARED;
    return MMSYSERR_NOERROR;
}

static DWORD modGetVolume(MIDIMAPDATA* mom, DWORD* lpdwVolume)
{
    if (!lpdwVolume) return MMSYSERR_INVALPARAM;
    *lpdwVolume = 0xFFFFFFFF; /* tests show this initial value */
    return MMSYSERR_NOERROR;
}

static DWORD modSetVolume(MIDIMAPDATA* mom, DWORD dwVolume)
{
    /* Native forwards it to some underlying device
     * GetVolume returns what was last set here. */
    FIXME("stub\n");
    return MMSYSERR_NOERROR;
}

static DWORD modGetDevCaps(UINT wDevID, MIDIMAPDATA* mom, LPMIDIOUTCAPSW lpMidiCaps, DWORD_PTR size)
{
    static const MIDIOUTCAPSW mappercaps = {
	0x00FF, 0x0001, 0x0100, /* Manufacturer and Product ID */
        L"Wine midi mapper", MOD_MAPPER, 0, 0, 0xFFFF,
	/* Native returns volume caps of underlying device + MIDICAPS_STREAM */
	MIDICAPS_VOLUME|MIDICAPS_LRVOLUME
    };
    if (lpMidiCaps == NULL) return MMSYSERR_INVALPARAM;
    if (!numMidiOutPorts) return MMSYSERR_BADDEVICEID;

    memcpy(lpMidiCaps, &mappercaps, min(size, sizeof(*lpMidiCaps)));
    return MMSYSERR_NOERROR;
}

static	DWORD	modReset(MIDIMAPDATA* mom)
{
    WORD	chn;
    DWORD	ret = MMSYSERR_NOERROR;

    if (MIDIMAP_IsBadData(mom))
	return MMSYSERR_ERROR;

    for (chn = 0; chn < 16; chn++)
    {
        if (mom->ChannelMap[chn] && mom->ChannelMap[chn]->loaded > 0)
        {
            //参考alsamidi.c的midi_out_reset
            if (androidPort) {
                modData(mom, (MIDI_CTL_ALL_SOUNDS_OFF << 8) | MIDI_CMD_CONTROL | chn); /* turn off every note */
                modData(mom, (MIDI_CTL_SUSTAIN << 8) | MIDI_CMD_CONTROL | chn); /* remove sustain on all channels */
                continue;
            }
            ret = midiOutReset(mom->ChannelMap[chn]->hMidi);
            if (ret != MMSYSERR_NOERROR) break;
        }
    }
    mom->runningStatus = 0;

    return ret;
}

static LRESULT MIDIMAP_drvOpen(void);
static LRESULT MIDIMAP_drvClose(void);

/**************************************************************************
 * 				modMessage (MIDIMAP.@)
 */
DWORD WINAPI MIDIMAP_modMessage(UINT wDevID, UINT wMsg, DWORD_PTR dwUser,
				DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
    TRACE("(%u, %04X, %08IX, %08IX, %08IX);\n",
	  wDevID, wMsg, dwUser, dwParam1, dwParam2);

    switch (wMsg)
    {
    case DRVM_INIT:
        return MIDIMAP_drvOpen();
    case DRVM_EXIT:
        return MIDIMAP_drvClose();
    case DRVM_ENABLE:
    case DRVM_DISABLE:
	/* FIXME: Pretend this is supported */
	return 0;

    case MODM_OPEN: return modOpen((DWORD_PTR *)dwUser, (LPMIDIOPENDESC)dwParam1, dwParam2);
    case MODM_CLOSE:	 	return modClose		((MIDIMAPDATA*)dwUser);

    case MODM_DATA:		return modData		((MIDIMAPDATA*)dwUser, dwParam1);
    case MODM_LONGDATA:		return modLongData      ((MIDIMAPDATA*)dwUser, (LPMIDIHDR)dwParam1,     dwParam2);
    case MODM_PREPARE:	 	return modPrepare	((MIDIMAPDATA*)dwUser, (LPMIDIHDR)dwParam1, 	dwParam2);
    case MODM_UNPREPARE: 	return modUnprepare	((MIDIMAPDATA*)dwUser, (LPMIDIHDR)dwParam1, 	dwParam2);
    case MODM_RESET:		return modReset		((MIDIMAPDATA*)dwUser);

    case MODM_GETDEVCAPS:	return modGetDevCaps	(wDevID, (MIDIMAPDATA*)dwUser, (LPMIDIOUTCAPSW)dwParam1,dwParam2);
    case MODM_GETNUMDEVS:	return 1;
    case MODM_GETVOLUME:	return modGetVolume	((MIDIMAPDATA*)dwUser, (DWORD*)dwParam1);
    case MODM_SETVOLUME:	return modSetVolume	((MIDIMAPDATA*)dwUser, dwParam1);
    default:
	FIXME("unknown message %d!\n", wMsg);
    }
    return MMSYSERR_NOTSUPPORTED;
}

/*======================================================================*
 *                  Driver part                                         *
 *======================================================================*/

/**************************************************************************
 * 				MIDIMAP_drvOpen			[internal]
 */
static LRESULT MIDIMAP_drvOpen(void)
{
    MIDIOUTCAPSW	moc;
    unsigned		dev, i;
    BOOL                found_valid_port = FALSE;

    if (midiOutPorts)
        return 0;

    numMidiOutPorts = midiOutGetNumDevs();

    if (!androidPort) 
        androidPort = getenv("MIDI_SOCKET_PORT");
    FIXME("midi调试 获取到的环境变量值为%s\n", androidPort);

    if (androidPort) 
    {
        // numMidiOutPorts ++; //如果变量存在则设备个数+1,下面分配数组大小要用到
        numMidiOutPorts = 1; //直接设置成只有1个设备
        midiOutPorts = HeapAlloc(GetProcessHeap(), 0, numMidiOutPorts * sizeof(MIDIOUTPORT));

        lstrcpyW(midiOutPorts[0].name, L"android-virtual-device");
        midiOutPorts[0].loaded = 0;
        midiOutPorts[0].hMidi = 0;
        midiOutPorts[0].uDevID = dev;
        midiOutPorts[0].lpbPatch = NULL;
        for (i = 0; i < 16; i++)
            midiOutPorts[0].aChn[i] = i;
        found_valid_port = TRUE;

        connect_socket();
        return 1;
    }

    midiOutPorts = HeapAlloc(GetProcessHeap(), 0,
			     numMidiOutPorts * sizeof(MIDIOUTPORT));
    for (dev = 0; dev < numMidiOutPorts; dev++)
    {
        if (midiOutGetDevCapsW(dev, &moc, sizeof(moc)) == 0L)
        {
            lstrcpyW(midiOutPorts[dev].name, moc.szPname);
            midiOutPorts[dev].loaded = 0;
            midiOutPorts[dev].hMidi = 0;
            midiOutPorts[dev].uDevID = dev;
            midiOutPorts[dev].lpbPatch = NULL;
            for (i = 0; i < 16; i++)
                midiOutPorts[dev].aChn[i] = i;
            if (wcsncmp(midiOutPorts[dev].name, L"Midi Through", lstrlenW(L"Midi Through")) != 0)
                found_valid_port = TRUE;
        }
        else
        {
            midiOutPorts[dev].loaded = -1;
        }
    }

    if (!found_valid_port)
        ERR_(winediag)("No software synthesizer midi port found, Midi sound output probably won't work.\n");

    return 1;
}

/**************************************************************************
 * 				MIDIMAP_drvClose		[internal]
 */
static LRESULT MIDIMAP_drvClose(void)
{
    if (midiOutPorts)
    {
        if (androidPort) //断开socket
            disconnect_socket();
        HeapFree(GetProcessHeap(), 0, midiOutPorts);
        midiOutPorts = NULL;
        return 1;
    }
    return 0;
}

/**************************************************************************
 * 				DriverProc (MIDIMAP.@)
 */
LRESULT CALLBACK MIDIMAP_DriverProc(DWORD_PTR dwDevID, HDRVR hDriv, UINT wMsg,
                                    LPARAM dwParam1, LPARAM dwParam2)
{
/* EPP     TRACE("(%08lX, %04X, %08lX, %08lX, %08lX)\n",  */
/* EPP 	  dwDevID, hDriv, wMsg, dwParam1, dwParam2); */

    switch (wMsg)
    {
    case DRV_LOAD:		return 1;
    case DRV_FREE:		return 1;
    case DRV_OPEN:		return 1;
    case DRV_CLOSE:		return 1;
    case DRV_ENABLE:		return 1;
    case DRV_DISABLE:		return 1;
    case DRV_QUERYCONFIGURE:	return 1;
    case DRV_CONFIGURE:		MessageBoxA(0, "MIDIMAP MultiMedia Driver !", "OSS Driver", MB_OK);	return 1;
    case DRV_INSTALL:		return DRVCNF_RESTART;
    case DRV_REMOVE:		return DRVCNF_RESTART;
    default:
	return DefDriverProc(dwDevID, hDriv, wMsg, dwParam1, dwParam2);
    }
}

/*
疑问：
1. IPPROTO_TCP 这里用tcp没问题吗？要不要用0
2. WSACleanup。 好像是卸载dll，所有线程上的socket全部断开，好像会影响到其他的？
3. connectSocket是否需要手动释放内存？

参考：
https://learn.microsoft.com/en-us/windows/win32/winsock
*/
static void connect_socket() {
    if(connectSocket != INVALID_SOCKET)
        return;

    WSADATA wsaData;
    
    struct sockaddr_in serverAddr;
    int result;
    char* serverIP = "127.0.0.1"; // 服务器 IP 地址
    int serverPort = atoi(androidPort);        // 服务器端口
    if(!serverPort) serverPort = 43456;
    
    // 初始化 Winsock
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        ERR("midi调试 WSAStartup failed: %d\n", result);
        return;
    }

    // 创建 Socket
    connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (connectSocket == INVALID_SOCKET) {
        ERR("midi调试 Socket creation failed: %ld\n", WSAGetLastError());
        WSACleanup();
        return;
    }

    // 设置服务器地址信息
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);
    memset(serverAddr.sin_zero, 0X00, 8); 

    // 连接到服务器
    result = connect(connectSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        ERR("midi调试 Connection failed: %ld\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        connectSocket = INVALID_SOCKET;
        return;
    }

    FIXME("midi调试 与服务器建立socket成功 -- %s:%d", serverIP, serverPort);

    return 0;
}

static void disconnect_socket() {
    // 关闭连接
    closesocket(connectSocket);
    WSACleanup();
    connectSocket = INVALID_SOCKET;
}

static DWORD send_socket_data(MIDIMAPDATA *dest, DWORD_PTR data) {
    if(connectSocket == INVALID_SOCKET)
        return MMSYSERR_NOERROR;

    char buf[5];
    BYTE evt = LOBYTE(LOWORD(data)), d1, d2; //evt=1,2位，d1=3,4位，d2=5,6位
    // struct midi_dest *dest;

    if (evt & 0x80)
    {
        d1 = HIBYTE(LOWORD(data));
        d2 = LOBYTE(HIWORD(data));
    }
    else if (dest->runningStatus)
    {
        evt = dest->runningStatus;
        d1 = LOBYTE(LOWORD(data));
        d2 = HIBYTE(LOWORD(data));
    }
    else
    {
        FIXME("ooch %x\n", data);
        return MMSYSERR_NOERROR;
    }


    //手动格式化,低位在前高为在后,8位一个char,第5个写'\0'
    buf[0] = TYPE_SHORT_MSG;
    buf[1] = evt;
    buf[2] = d1;
    buf[3] = d2;
    buf[4] = '\0';

    TRACE("midi调试 socket发送短消息。data=%08x\n", data);

    // 发送数据
    if (send(connectSocket, buf, 4, 0) == SOCKET_ERROR) {
        ERR("Send failed: %ld\n", WSAGetLastError());
    }

    return MMSYSERR_NOERROR;
}

static DWORD send_socket_long_data() {
    if(connectSocket == INVALID_SOCKET)
        return MMSYSERR_NOERROR;

    FIXME("midi调试 开始通过socket发送长消息数据(尚未实现). dwParam=\n");
    

    return MMSYSERR_NOERROR;
}