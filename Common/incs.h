/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: incs.h
	Programmer	: Andy Eder
	Date		: 29/04/99 09:19:40

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

//----- [ INCLUDE FILES FOR FROGGER 2 ] -----------------------------------------------------//

#ifdef PC_VERSION
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#include <dinput.h>

#ifndef __cplusplus

#include <math.h>
#endif

#include "pcmisc.h"
#include "directx.h"
//#include "dx_sound.h"
#include "mdxDDraw.h"
#include "mdxD3D.h"

#else

#include <pr/gbi.h>
#include <pr/ucode.h>
#include <pr/gu.h>
#include <pr/gs2dex.h>

#endif


#include "types.h"
#include "define.h"
#include "maths.h"
#include "font.h"
#include "types2dgfx.h"
#include "general.h"
#include "3DText.h"

#ifndef PC_VERSION
#include "nnsched.h"
#endif

#include "actor.h"
#include "layoutvisual.h"

#ifndef PC_VERSION
#include "graphic.h"
#endif

#include "language.h"
#include "levelplaying.h"
#include "jalloc.h"
#include "sprite.h"
#include "particle.h"
#include "overlays.h"
#include "textoverlays.h"
#include "frogger.h"
#include "hud.h"
#include "babyfrogs.h"
#include "specfx.h"
#include "printgfx.h"
#include "anim.h"
#include "path.h"
#include "enemies.h"
#include "platform.h"
#include "character.h"
#include "cam.h"
#include "game.h"
#include "multi.h"
#include "objects.h"
#include "texture.h"
#include "ptexture.h"
#include "collect.h"
#include "path.h"
#include "platform.h"
#include "frogmove.h"
#include "cam.h"
#include "tongue.h"
#include "script.h"
#include "event.h"
#include "eventfuncs.h"

#ifndef PC_VERSION
#include "develop.h"
#include "objview.h"
#endif

#include "frontend.h"
#include "sndview.h"
#include "map.h"
#include "controll.h"

#include "subcube.h"
#include "block.h"
#include "savegame.h"
#include "menus.h"
#include "defaultdata.h"
#include "audio.h"

#ifndef PC_VERSION
#include "libmus.h"
#include "libmus_data.h"
#endif

#include "sndview.h"
#include "water.h"

#ifndef PC_VERSION
#include "sfx.h"
#include "mbuffer.h"
#include "rumblepak.h"
#endif

#ifndef PC_VERSION
#include "levbanks\levext.h"
#else
#include "dx_sound.h"
#include "\work\froggerII\pc\levbanks\levext.h"
#endif

