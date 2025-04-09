/*
	Copyright (c) 1994-2003 Macromedia, Inc.	All Rights Reserved

	You may utilize this source file to create and compile object code for use within products
	you may create. THIS CODE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, AND
	MACROMEDIA DISCLAIMS ALL IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.	IN NO EVENT WILL MACROMEDIA BE
	LIABLE TO YOU FOR ANY CONSEQUENTIAL, INDIRECT OR INCIDENTAL DAMAGES ARISING OUT OF YOUR
	USE OR INABILITY TO USE THIS CODE.
*/

#ifndef _H_Script
#define _H_Script

#include "moastdif.h"

//#include "mmiutil.h"
//#include "mmiimage.h"
//#include "moafile2.h"
//#include "mmixval.h"
//#include "mmiClr.h"
#include "xmmvalue.h"
#include "moanotif.h"

#define USING_NOTIFICATION_CLIENT

#include "steam_api.h"

/* --------------------------------------------------- */

// GUID
// {B7FEBDAC-EE72-4716-AE62-2FBE661016C8}
DEFINE_GUID(CLSID_TStdXtra,
	0xb7febdac, 0xee72, 0x4716, 0xae, 0x62, 0x2f, 0xbe, 0x66, 0x10, 0x16, 0xc8);

EXTERN_BEGIN_DEFINE_CLASS_INSTANCE_VARS(TStdXtra)

	PIMoaDrMovie			pDrMovie = NULL;
	PIMoaDrPlayer			pDrPlayer = NULL;
	PIMoaMmValue			pMmValue = NULL;
	PIMoaDrMovieContext		pDrContext = NULL;

	//PIMoaMmUtils2 		pMmUtils = NULL;
	//PIMoaMmImage			pMmImage = NULL;
	//PIMoaDrUtils			pDrUtils = NULL;
	//PIMoaDrCastMem		pDrCastMem = NULL;
	//PIMoaDrCast			pDrCast = NULL;
	//PIMoaMmList			pMmList = NULL;
	//PIMoaMmXValueUtils	pMmXValueUtils = NULL;
	//PIMoaMmColor			pMmColor = NULL;
	
#ifdef USING_NOTIFICATION_CLIENT
	PIMoaNotification		pMmNotification = NULL;
	PIMoaNotificationClient	pNotificationClient = NULL;
#endif

	bool					m_bApiInitialized = false;

	bool					m_bISteamFriends_GameOverlayActivated = false;
	bool					m_bISteamFriends_GameOverlayIsActive = false;

EXTERN_END_DEFINE_CLASS_INSTANCE_VARS

/*****************************************************************************
 * Xtra Moa Registration Interface - Class definition
 ****************************************************************************/

EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaRegister)
	EXTERN_DEFINE_METHOD(MoaError, Register, (PIMoaCache, PIMoaXtraEntryDict))
EXTERN_END_DEFINE_CLASS_INTERFACE

/*****************************************************************************
 * Xtra Moa Scripting Interface - Class definition
 ****************************************************************************/

EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaMmXScript)

	EXTERN_DEFINE_METHOD(MoaError, Call, (PMoaDrCallInfo))

	STEAM_CALLBACK(TStdXtra_IMoaMmXScript, ISteamFriends_OnGameOverlayActivated, GameOverlayActivated_t);

private:

	// Methods that implement the Lingo commands. These are dispatched from "Call"
	EXTERN_DEFINE_METHOD(MoaError, sx_new, (PMoaDrCallInfo))
	//EXTERN_DEFINE_METHOD(MoaError, sx_forget, (PMoaDrCallInfo))

	EXTERN_DEFINE_METHOD(MoaError, sx_SteamAPI_RestartAppIfNecessary, (PMoaDrCallInfo))
	EXTERN_DEFINE_METHOD(MoaError, sx_SteamAPI_Init, (PMoaDrCallInfo))
	EXTERN_DEFINE_METHOD(MoaError, sx_SteamAPI_Shutdown, (PMoaDrCallInfo))
	EXTERN_DEFINE_METHOD(MoaError, sx_SteamAPI_IsSteamRunning, (PMoaDrCallInfo))

	// ISteamFriends
	EXTERN_DEFINE_METHOD(MoaError, sx_ISteamFriends_GetPersonaName, (PMoaDrCallInfo))

	// ISteamUser
	EXTERN_DEFINE_METHOD(MoaError, sx_ISteamUser_GetSteamID, (PMoaDrCallInfo))

	// ISteamUserStats
	EXTERN_DEFINE_METHOD(MoaError, sx_ISteamUserStats_GetAchievement, (PMoaDrCallInfo))
	EXTERN_DEFINE_METHOD(MoaError, sx_ISteamUserStats_SetAchievement, (PMoaDrCallInfo))

	EXTERN_DEFINE_METHOD(MoaError, sx_ISteamUserStats_GetStatFloat, (PMoaDrCallInfo))
	EXTERN_DEFINE_METHOD(MoaError, sx_ISteamUserStats_SetStatFloat, (PMoaDrCallInfo))
	EXTERN_DEFINE_METHOD(MoaError, sx_ISteamUserStats_GetStatInt, (PMoaDrCallInfo))
	EXTERN_DEFINE_METHOD(MoaError, sx_ISteamUserStats_SetStatInt, (PMoaDrCallInfo))
	EXTERN_DEFINE_METHOD(MoaError, sx_ISteamUserStats_StoreStats, (PMoaDrCallInfo))

	// ISteamUtils
	EXTERN_DEFINE_METHOD(MoaError, sx_ISteamUtils_GetAppID, (PMoaDrCallInfo))
	EXTERN_DEFINE_METHOD(MoaError, sx_ISteamUtils_GetIPCountry, (PMoaDrCallInfo))
	
EXTERN_END_DEFINE_CLASS_INTERFACE

#ifdef USING_NOTIFICATION_CLIENT
EXTERN_BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaNotificationClient)
	EXTERN_DEFINE_METHOD(MoaError, Notify, (ConstPMoaNotifyID, PMoaVoid, PMoaVoid))
EXTERN_END_DEFINE_CLASS_INTERFACE
#endif

#endif /* _H_Script */
