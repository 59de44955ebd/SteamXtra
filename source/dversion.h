/*

	Copyright (c) 1994-2004 Macromedia, Inc.	All Rights Reserved

	You may utilize this source file to create and compile object code for use within products
	you may create. THIS CODE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, AND
	MACROMEDIA DISCLAIMS ALL IMPLIED WARRANTIES INCLUDING, BUT NOT LIMITED TO, MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.	IN NO EVENT WILL MACROMEDIA BE
	LIABLE TO YOU FOR ANY CONSEQUENTIAL, INDIRECT OR INCIDENTAL DAMAGES ARISING OUT OF YOUR
	USE OR INABILITY TO USE THIS CODE.

*/

/*
if you want to set up different version information for different xtras in the XDK,
make a copy of this file in the source folder for the example project. The local copy will
be found before the copy in the include folder and so you can customize it as you wish.
*/
					
#ifndef DVERSION_H										
#define DVERSION_H										
			
#define DVER_COUNTRY VER_COUNTRY_US			

#define DVER_MAJORVERSION 0				    
#define DVER_MAJORVERSION_STRING "0"

#define DVER_MINORVERSION 1
#define DVER_MINORVERSION_STRING "1"     
														
#define DVER_BUGFIXVERSION 0
#define DVER_BUGFIXVERSION_STRING "0"

#define DVER_BUILDNUM 0							
#define DVER_BUILDNUM_STRING "0"	

#if VER_BUGFIXVERSION > 0
	#define	DVER_VERSION_STRING	DVER_MAJORVERSION_STRING "." DVER_MINORVERSION_STRING "." DVER_BUGFIXVERSION_STRING 
#else
	#define	DVER_VERSION_STRING	DVER_MAJORVERSION_STRING "." DVER_MINORVERSION_STRING 
#endif

// following are used in version info for the windows resource 
#define DVER_ORIGINALFILENAME	"SteamXtra.x32"
#define DVER_COMPANY			"59de44955ebd"
#define DVER_FILEDESCRIPTION	"SteamXtra"
#define DVER_PRODUCTNAME		"SteamXtra"
#define DVER_INTERNALNAME		"SteamXtra"
#define DVER_YEAR				"2025"

#endif													
