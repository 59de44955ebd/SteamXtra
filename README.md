# SteamXtra

Stub for integrating the Steamworks SDK into games created with Adobe Director for Windows

## Current interface

```
xtra SteamXtra
-- v0.1 (c) 2025 59de44955ebd
-- https://github.com/59de44955ebd/SteamXtra

new object me

SteamAPI_RestartAppIfNecessary object me, integer appID
SteamAPI_Init object me
SteamAPI_Shutdown object me
SteamAPI_IsSteamRunning object me

-- ISteamFriends
ISteamFriends_GetPersonaName object me

-- ISteamUser
ISteamUser_GetSteamID object me

-- ISteamUserStats
ISteamUserStats_GetAchievement object me, string achievement
ISteamUserStats_SetAchievement object me, string achievement
ISteamUserStats_GetStat object me, string statName
ISteamUserStats_SetStat object me, string statName, float data
ISteamUserStats_StoreStats object me, string achievement

-- ISteamUtils
ISteamUtils_GetAppID object me
ISteamUtils_GetIPCountry object me
```
