#define INITGUID 1

#include "xtra.h"

#include <string.h>
#include <stdlib.h>

#include "xclassver.h"
#include "moatry.h"

/* -- MACRO DEFINITIONS ------------------------------------------------- */

#define CALL_CASE(x) case m_ ## x: ThrowErr(x(callPtr)); break;
#define SAFE_RELEASE(x) { if (x!=NULL) x->Release(); x = NULL; }
#define PARAM(x) (x+1)

// for quick runtime debugging
#define DBGS(x) OutputDebugStringA(x);
#define DBGI(x) {char dbg[32];sprintf(dbg,"%ld",x);OutputDebugStringA(dbg);}
#define DBGW(x) OutputDebugStringW(x);

/*******************************************************************************
 * SCRIPTING XTRA MESSAGE TABLE DESCRIPTION.
 *
 * The general format is:
 * xtra <nameOfXtra>
 * new object me [ args ... ]
 * <otherHandlerDefintions>
 * --
 * The first line must give the name of the Scripting xtra.
 * The remaining lines give the names of the handlers that this xtra implements
 * along with the required types of the arguments for each handler.
 *
 * -- Pre-defined handler new
 * The new handler will be called when a child object is created,
 * the first argument is always the child object and you defined any remaining arguments.
 * The new handler is the place to initialize any memory required by the child object.
 *
 * -- Simple Handler Definitions
 * Each handler definition line is format like this:
 * <handlerName> <argType1> <argName1>, <argType2> <argName2> ...
 * The first word is the handler name. Following this are types description for
 * the argument to the handler, each separated by a comma.
 * The argument name <argName>, may be omited.
 * Permited argument types are:
 * 	integer
 * 	float
 * 	string
 * 	symbol
 * 	object
 * 	any
 * 	*
 * For integer, float, string, symbol, and object, the type of the argument must
 * match. The type any means allow any type. The asterisk (*) means any number and
 * any type of arguments.
 *
 * The first argument is the child object and is always declared to be of type object.
 *
 * -- Global Handlers
 * An asterisk (*) preceeding the handler name signifies a global handler.
 * This handler is at the global scope level and can be called from any
 * movie.
 *
 * -- Xtra level handlers
 * A plus (+) preceeding the handler name signifies an Xtra level handler.
 * This kind of handler can be called directly from the Xtra reference,
 * without creating a child object.
 *
 * The enumerated list that follows must correspond directly with the msgTable
 * (i.e. they must be in the same order).
 *
 *******************************************************************************/

 /* This is the list of handlers for the xtra. The versionInfo string is combined
 /*	with the msgTable string in the register method to create a single string that
 /* used when registering the xtra as a scripting xtra. */

static char versionInfo[] =
{
	"xtra " DVER_PRODUCTNAME "\n"
	"-- v%s.%s (c) %s %s\n"
	"-- https://github.com/59de44955ebd/SteamXtra\n"
	"\n"
};

static char msgTable[] =
{
	"new object me\n"
	//"/forget object me\n"
	"\n"
	"SteamAPI_RestartAppIfNecessary object me, integer appID\n"
	"SteamAPI_Init object me\n"
	"SteamAPI_Shutdown object me\n"
	"SteamAPI_IsSteamRunning object me\n"

	"\n-- ISteamFriends\n"
	"ISteamFriends_GetPersonaName object me\n"

	"\n-- ISteamUser\n"
	"ISteamUser_GetSteamID object me\n"

	"\n-- ISteamUserStats\n"
	"ISteamUserStats_GetAchievement object me, string achievement\n"
	"ISteamUserStats_SetAchievement object me, string achievement\n"
	"ISteamUserStats_GetStat object me, string statName\n"
	"ISteamUserStats_SetStat object me, string statName, float data\n"
	"ISteamUserStats_StoreStats object me, string achievement\n"

	"\n-- ISteamUtils\n"
	"ISteamUtils_GetAppID object me\n"
	"ISteamUtils_GetIPCountry object me\n"

	// add more functions here...
};

/* 	This is the enumerated scripting method list. This list should
 *	directly correspond to the msgTable defined above. It is used
 *	to dispatch method calls via the methodSelector. The 'm_XXXX' method must
 *	be last.
 */

enum
{
	m_sx_new = 0,
	//m_sx_forget,

	m_sx_SteamAPI_RestartAppIfNecessary,
	m_sx_SteamAPI_Init,
	m_sx_SteamAPI_Shutdown,
	m_sx_SteamAPI_IsSteamRunning,

	// ISteamFriends
	m_sx_ISteamFriends_GetPersonaName,

	// ISteamUser
	m_sx_ISteamUser_GetSteamID,

	// ISteamUserStats
	m_sx_ISteamUserStats_GetAchievement,
	m_sx_ISteamUserStats_SetAchievement,
	m_sx_ISteamUserStats_GetStat,
	m_sx_ISteamUserStats_SetStat,
	m_sx_ISteamUserStats_StoreStats,

	// ISteamUtils
	m_sx_ISteamUtils_GetAppID,
	m_sx_ISteamUtils_GetIPCountry,

	m_XXXX
};

/* ============================================================================= */
/* Xtra Glue Stuff */
/* ============================================================================= */

#define XTRA_VERSION_NUMBER XTRA_CLASS_VERSION

BEGIN_XTRA
	BEGIN_XTRA_DEFINES_CLASS(TStdXtra, XTRA_CLASS_VERSION)
		CLASS_DEFINES_INTERFACE(TStdXtra, IMoaRegister, XTRA_VERSION_NUMBER)
		CLASS_DEFINES_INTERFACE(TStdXtra, IMoaMmXScript, XTRA_VERSION_NUMBER)
		/*
		 * ---> insert additional interface(s) -->
		 */
#ifdef USING_NOTIFICATION_CLIENT
		CLASS_DEFINES_INTERFACE(TStdXtra, IMoaNotificationClient, XTRA_VERSION_NUMBER)
#endif
	END_XTRA_DEFINES_CLASS
END_XTRA

/* ============================================================================= */
/* Create/Destroy for class TStdXtra */
/* ============================================================================= */

STDMETHODIMP_(MoaError) MoaCreate_TStdXtra (TStdXtra FAR * This)
{
moa_try

	ThrowErr (This->pCallback->QueryInterface(&IID_IMoaDrPlayer,		(PPMoaVoid) &This->pDrPlayer));
	ThrowErr (This->pCallback->QueryInterface(&IID_IMoaMmValue,			(PPMoaVoid)&This->pMmValue));

	//ThrowErr (This->pCallback->QueryInterface(&IID_IMoaDrUtils,	(PPMoaVoid) &This->pDrUtils));
	//ThrowErr (This->pCallback->QueryInterface(&IID_IMoaMmColor,	(PPMoaVoid)&This->pMmColor));
	//ThrowErr (This->pCallback->QueryInterface(&IID_IMoaMmImage,	(PPMoaVoid) &This->pMmImage));
	//ThrowErr (This->pCallback->QueryInterface(&IID_IMoaMmList,	(PPMoaVoid) &This->pMmList));
	//ThrowErr (This->pCallback->QueryInterface(&IID_IMoaMmUtils2,	(PPMoaVoid) &This->pMmUtils));
	//ThrowErr (This->pCallback->QueryInterface(&IID_IMoaMmXValueUtils,	(PPMoaVoid) &This->pMmXValueUtils));

#ifdef USING_NOTIFICATION_CLIENT
	ThrowErr (This->pCallback->QueryInterface(&IID_IMoaNotification,	(PPMoaVoid)&This->pMmNotification));
#endif

moa_catch
moa_catch_end
moa_try_end
}

STDMETHODIMP_(void) MoaDestroy_TStdXtra(TStdXtra FAR * This)
{
	SAFE_RELEASE(This->pDrMovie);
	SAFE_RELEASE(This->pDrPlayer);
	SAFE_RELEASE(This->pMmValue);
	SAFE_RELEASE(This->pDrContext);

	//SAFE_RELEASE(This->pDrUtils);
	//SAFE_RELEASE(This->pMmColor);
	//SAFE_RELEASE(This->pMmImage);
	//SAFE_RELEASE(This->pMmList);
	//SAFE_RELEASE(This->pMmUtils);
	//SAFE_RELEASE(This->pMmXValueUtils);

#ifdef USING_NOTIFICATION_CLIENT
	SAFE_RELEASE(This->pMmNotification);
#endif
}

#ifdef USING_NOTIFICATION_CLIENT

TStdXtra_IMoaNotificationClient::TStdXtra_IMoaNotificationClient(MoaError FAR * pErr)
{
	*pErr = (kMoaErr_NoErr);
}
TStdXtra_IMoaNotificationClient::~TStdXtra_IMoaNotificationClient()
{
}

#endif

/* ============================================================================= */
/* Methods in TStdXtra_IMoaRegister */
/* ============================================================================= */

/*****************************************************************************
 *  Data needed for Registering
 *	---------------------------
 *	Specific code needed to register different types of Xtras.  The skeleton
 *	source files include minimal implementations appropriate for each Xtra
 *	type.  Current necessary actions:
 *
 *	Scripting Xtra:				Add the Scripting Xtra Message Table
 *	Sprite Asset Xtra:			Nothing
 *	Tool Xtra:					Nothing
 *	Transition Asset Xtra		Nothing
 *
 *****************************************************************************/

STD_INTERFACE_CREATE_DESTROY(TStdXtra, IMoaRegister)

BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaRegister)
END_DEFINE_CLASS_INTERFACE

#ifdef USING_NOTIFICATION_CLIENT
BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaNotificationClient)
END_DEFINE_CLASS_INTERFACE
#endif

/* ----------------------------------------------------------------------------- */
STDMETHODIMP TStdXtra_IMoaRegister::Register(
	PIMoaCache pCache,
	PIMoaXtraEntryDict pXtraDict
)
{
moa_try
	PIMoaRegistryEntryDict pReg;
#define MAX_VERSION_LEN 256
	char versionStr[MAX_VERSION_LEN];
	PMoaVoid pMemStr = NULL;

	/* Register the lingo xtra */
	ThrowErr(pCache->AddRegistryEntry(pXtraDict, &CLSID_TStdXtra, &IID_IMoaMmXScript, &pReg));

	/* Register the method table */
	sprintf_s(
		versionStr,
		MAX_VERSION_LEN,
		versionInfo,
		DVER_MAJORVERSION_STRING,
		DVER_MINORVERSION_STRING,
		DVER_YEAR,
		DVER_COMPANY
	);

	pMemStr = pObj->pCalloc->NRAlloc(sizeof(msgTable) + sizeof(versionStr));

	ThrowNull(pMemStr);

	strcpy_s((char *)pMemStr, sizeof(msgTable) + sizeof(versionStr), versionStr);
	strcat_s((char *)pMemStr, sizeof(msgTable) + sizeof(versionStr), msgTable);
	ThrowErr(pReg->Put(kMoaDrDictType_MessageTable, pMemStr, 0, kMoaDrDictKey_MessageTable));

moa_catch
moa_catch_end
	if (pMemStr)
		pObj->pCalloc->NRFree(pMemStr);
moa_try_end
}

/* ============================================================================= */
/*  Methods in TStdXtra_IMoaMmXScript */
/* ============================================================================= */

BEGIN_DEFINE_CLASS_INTERFACE(TStdXtra, IMoaMmXScript)
END_DEFINE_CLASS_INTERFACE

//******************************************************************************
TStdXtra_IMoaMmXScript::TStdXtra_IMoaMmXScript(MoaError FAR * pError)
//------------------------------------------------------------------------------
{
	*pError = kMoaErr_NoErr;
}

//******************************************************************************
TStdXtra_IMoaMmXScript::~TStdXtra_IMoaMmXScript()
//------------------------------------------------------------------------------
{
}

/* ----------------------------------------------------------------------------- */
STDMETHODIMP TStdXtra_IMoaMmXScript::Call (PMoaDrCallInfo callPtr)
{
moa_try

	// Get the active movie
	//ThrowErr (pObj->pDrPlayer->GetActiveMovie(&pObj->pDrMovie) );
	//ThrowErr (pObj->pDrMovie->QueryInterface(&IID_IMoaDrMovieContext, (PPMoaVoid)&pObj->pDrContext));

	switch (callPtr->methodSelector)
	{
		CALL_CASE(sx_new)
		//CALL_CASE(sx_forget)

		CALL_CASE(sx_SteamAPI_Init)
		CALL_CASE(sx_SteamAPI_RestartAppIfNecessary)
		CALL_CASE(sx_SteamAPI_Shutdown)
		CALL_CASE(sx_SteamAPI_IsSteamRunning)
			
		// ISteamFriends
		CALL_CASE(sx_ISteamFriends_GetPersonaName)

		// ISteamUser
		CALL_CASE(sx_ISteamUser_GetSteamID)

		// ISteamUserStats
		CALL_CASE(sx_ISteamUserStats_GetAchievement)
		CALL_CASE(sx_ISteamUserStats_SetAchievement)
		CALL_CASE(sx_ISteamUserStats_GetStat)
		CALL_CASE(sx_ISteamUserStats_SetStat)
		CALL_CASE(sx_ISteamUserStats_StoreStats)

		// ISteamUtils
		CALL_CASE(sx_ISteamUtils_GetAppID)
		CALL_CASE(sx_ISteamUtils_GetIPCountry)
	}

moa_catch
moa_catch_end
moa_try_end
}

#ifdef USING_NOTIFICATION_CLIENT
//###########################################################################
//
//###########################################################################
STDMETHODIMP TStdXtra_IMoaNotificationClient::Notify(ConstPMoaNotifyID nid, PMoaVoid pNData, PMoaVoid pRefCon)
{
	TStdXtra_IMoaMmXScript* pThis = (TStdXtra_IMoaMmXScript*)pRefCon;
	if (pThis==0)
		return kMoaErr_NoErr;

	SteamAPI_RunCallbacks();

	// Establish the movie context to be able to call safely the player
	DrContextState drContextState;
	pThis->pObj->pDrContext->PushXtraContext(&drContextState);

	if (pObj->m_bISteamFriends_GameOverlayActivated)
	{
		pObj->m_bISteamFriends_GameOverlayActivated = false;

		MoaMmSymbol moaSymbol;
		pObj->pMmValue->StringToSymbol("ISteamFriends_GameOverlayActivated", &moaSymbol);

		MoaMmValue params[1];
		pObj->pMmValue->IntegerToValue(pObj->m_bISteamFriends_GameOverlayIsActive, &params[0]);
		pThis->pObj->pDrMovie->CallHandler(moaSymbol, 1, &params[0], 0);
	}

	pThis->pObj->pDrContext->PopXtraContext(&drContextState);

	return kMoaErr_NoErr;
}
#endif

/*****************************************************************************
 *  Private Methods
 *  -------------------
 *  Implementation of Private Class Methods
 /*
 *  This is the actual code for the defined methods. They are defined as
 *  functions, and called from the switch statement in Call().
 *
 ****************************************************************************/

//###########################################################################
// 
//###########################################################################
MoaError TStdXtra_IMoaMmXScript::sx_new(PMoaDrCallInfo callPtr)
{
moa_try
	ThrowErr (pObj->pDrPlayer->GetActiveMovie(&pObj->pDrMovie));
	ThrowErr (pObj->pDrMovie->QueryInterface(&IID_IMoaDrMovieContext, (PPMoaVoid)&pObj->pDrContext));

#ifdef USING_NOTIFICATION_CLIENT
	if (pObj->pMmNotification != 0)
	{
		// Instantiate a notification client object
		if (pObj->pNotificationClient == 0)
			pObj->pCallback->MoaCreateInstance(&CLSID(TStdXtra), &IID(IMoaNotificationClient), (PPMoaVoid)&pObj->pNotificationClient);
		// Register the notification client object to receive IDLE events
		if (pObj->pNotificationClient != 0) 
			pObj->pMmNotification->RegisterNotificationClient(pObj->pNotificationClient, &NID_DrNIdle , 0, this);
	}
#endif

moa_catch
moa_catch_end
moa_try_end
}

//###########################################################################
// S_API bool S_CALLTYPE SteamAPI_RestartAppIfNecessary( uint32 unOwnAppID ) -> bool
//###########################################################################
MoaError TStdXtra_IMoaMmXScript::sx_SteamAPI_RestartAppIfNecessary(PMoaDrCallInfo callPtr)
{
	MoaLong appID;
	MoaMmValue moaValue = { 0 };
	GetArgByIndex(PARAM(1), &moaValue);
	pObj->pMmValue->ValueToInteger(&moaValue, &appID);
	bool ok = SteamAPI_RestartAppIfNecessary((uint32)appID);
	pObj->pMmValue->IntegerToValue(ok, &callPtr->resultValue);
	return kMoaErr_NoErr;
}

//###########################################################################
// S_API bool S_CALLTYPE SteamAPI_Init() -> bool
//###########################################################################
MoaError TStdXtra_IMoaMmXScript::sx_SteamAPI_Init(PMoaDrCallInfo callPtr)
{
	if (!pObj->m_bApiInitialized)
	{
		pObj->m_bApiInitialized = SteamAPI_Init();
	}
	pObj->pMmValue->IntegerToValue(pObj->m_bApiInitialized, &callPtr->resultValue);
	return kMoaErr_NoErr;
}

//###########################################################################
// S_API void S_CALLTYPE SteamAPI_Shutdown()
//###########################################################################
MoaError TStdXtra_IMoaMmXScript::sx_SteamAPI_Shutdown(PMoaDrCallInfo callPtr)
{
	if (pObj->m_bApiInitialized)
	{
		SteamAPI_Shutdown();
		pObj->m_bApiInitialized = false;
	}
	return kMoaErr_NoErr;
}

//###########################################################################
// S_API bool S_CALLTYPE SteamAPI_IsSteamRunning() -> bool
//###########################################################################
MoaError TStdXtra_IMoaMmXScript::sx_SteamAPI_IsSteamRunning(PMoaDrCallInfo callPtr)
{
	bool ok = SteamAPI_IsSteamRunning();
	pObj->pMmValue->IntegerToValue(ok, &callPtr->resultValue);
	return kMoaErr_NoErr;
}

//###########################################################################
// ISteamFriends_GetPersonaName() -> string or void
//###########################################################################
MoaError TStdXtra_IMoaMmXScript::sx_ISteamFriends_GetPersonaName(PMoaDrCallInfo callPtr)
{
	if (pObj->m_bApiInitialized)
	{
		const char *name = SteamFriends()->GetPersonaName();
		pObj->pMmValue->StringToValue(name, &callPtr->resultValue);
	}
	return kMoaErr_NoErr;
}

//###########################################################################
// ISteamUser_GetSteamID() -> integer or void
//###########################################################################
MoaError TStdXtra_IMoaMmXScript::sx_ISteamUser_GetSteamID(PMoaDrCallInfo callPtr)
{
	if (pObj->m_bApiInitialized)
	{
		MoaLong accountID = (SteamUser()->GetSteamID()).GetAccountID();
		pObj->pMmValue->IntegerToValue(accountID, &callPtr->resultValue);
	}
	return kMoaErr_NoErr;
}

//###########################################################################
// ISteamUserStats_GetAchievement(string achievement) -> bool or void
//###########################################################################
MoaError TStdXtra_IMoaMmXScript::sx_ISteamUserStats_GetAchievement(PMoaDrCallInfo callPtr)
{
	if (pObj->m_bApiInitialized)
	{
		bool bAchieved = false;
		MoaMmValue moaValue = { 0 };
		GetArgByIndex(PARAM(1), &moaValue);
		const char *achName;
		pObj->pMmValue->ValueToStringPtr(&moaValue, &achName);
		SteamUserStats()->GetAchievement(achName, &bAchieved);
		pObj->pMmValue->ValueReleaseStringPtr(&moaValue);
		pObj->pMmValue->IntegerToValue(bAchieved, &callPtr->resultValue);
	}
	return kMoaErr_NoErr;
}

//###########################################################################
// ISteamUserStats_SetAchievement(string achievement) -> bool or void
//###########################################################################
MoaError TStdXtra_IMoaMmXScript::sx_ISteamUserStats_SetAchievement(PMoaDrCallInfo callPtr)
{
	if (pObj->m_bApiInitialized)
	{
		bool bOk = false;
		MoaMmValue moaValue = { 0 };
		GetArgByIndex(PARAM(1), &moaValue);
		const char *achName;
		pObj->pMmValue->ValueToStringPtr(&moaValue, &achName);
		bOk = SteamUserStats()->SetAchievement(achName);
		pObj->pMmValue->ValueReleaseStringPtr(&moaValue);
		pObj->pMmValue->IntegerToValue(bOk, &callPtr->resultValue);
	}
	return kMoaErr_NoErr;
}

//###########################################################################
// sx_ISteamUserStats_GetStat(string statName) -> float or void
//###########################################################################
MoaError TStdXtra_IMoaMmXScript::sx_ISteamUserStats_GetStat(PMoaDrCallInfo callPtr)
{
	if (pObj->m_bApiInitialized)
	{
		MoaMmValue moaValue = { 0 };
		GetArgByIndex(PARAM(1), &moaValue);
		const char *statName;
		pObj->pMmValue->ValueToStringPtr(&moaValue, &statName);
		float data = 0;
		SteamUserStats()->GetStat(statName, &data);
		pObj->pMmValue->ValueReleaseStringPtr(&moaValue);
		pObj->pMmValue->FloatToValue(data, &callPtr->resultValue);
	}
	return kMoaErr_NoErr;
}

//###########################################################################
// sx_ISteamUserStats_SetStat(string statName, float data) -> bool or void
//###########################################################################
MoaError TStdXtra_IMoaMmXScript::sx_ISteamUserStats_SetStat(PMoaDrCallInfo callPtr)
{
	if (pObj->m_bApiInitialized)
	{
		bool bOk = false;
		MoaMmValue moaValue = { 0 };

		MoaDouble data = 0;
		GetArgByIndex(PARAM(2), &moaValue);
		pObj->pMmValue->ValueToFloat(&moaValue, &data);

		GetArgByIndex(PARAM(1), &moaValue);
		const char *statName;
		pObj->pMmValue->ValueToStringPtr(&moaValue, &statName);

		bOk = SteamUserStats()->SetStat(statName, (float)data);
		pObj->pMmValue->ValueReleaseStringPtr(&moaValue);
		pObj->pMmValue->IntegerToValue(bOk, &callPtr->resultValue);
	}
	return kMoaErr_NoErr;
}

//###########################################################################
// ISteamUserStats_StoreStats() -> bool or void
//###########################################################################
MoaError TStdXtra_IMoaMmXScript::sx_ISteamUserStats_StoreStats(PMoaDrCallInfo callPtr)
{
	if (pObj->m_bApiInitialized)
	{
		bool bOk = SteamUserStats()->StoreStats();
		pObj->pMmValue->IntegerToValue(bOk, &callPtr->resultValue);
	}
	return kMoaErr_NoErr;
}

//###########################################################################
// ISteamUtils_GetAppID() -> integer or void
//###########################################################################
MoaError TStdXtra_IMoaMmXScript::sx_ISteamUtils_GetAppID(PMoaDrCallInfo callPtr)
{
	if (pObj->m_bApiInitialized)
	{
		MoaLong appID = SteamUtils()->GetAppID();
		pObj->pMmValue->IntegerToValue(appID, &callPtr->resultValue);
	}
	return kMoaErr_NoErr;
}

//###########################################################################
// ISteamUtils_GetIPCountry() -> string or void
//###########################################################################
MoaError TStdXtra_IMoaMmXScript::sx_ISteamUtils_GetIPCountry(PMoaDrCallInfo callPtr)
{
	if (pObj->m_bApiInitialized)
	{
		const char* country = SteamUtils()->GetIPCountry();
		pObj->pMmValue->StringToValue(country, &callPtr->resultValue);
	}
	return kMoaErr_NoErr;
}

//###########################################################################
// Callback
//###########################################################################
void TStdXtra_IMoaMmXScript::ISteamFriends_OnGameOverlayActivated(GameOverlayActivated_t* pCallback)
{
	pObj->m_bISteamFriends_GameOverlayActivated = true;
	pObj->m_bISteamFriends_GameOverlayIsActive = pCallback->m_bActive;
	//if (pCallback->m_bActive)
	//	DBGS("Steam overlay now active");
	//else
	//	DBGS("Steam overlay now inactive");
}
