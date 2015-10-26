// stdafx.h: Includedatei für Standardsystem-Includedateien
// oder häufig verwendete projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <string>


// TODO: Hier auf zusätzliche Header, die das Programm erfordert, verweisen.
#include <Windows.h>
#include <iostream>
#include <DbgHelp.h>
#include <Ntsecapi.h>
#include <TlHelp32.h>

#include "Injector.h"
#include "RemoteThreadInjector.h"
#include "WindowsHookInjector.h"
#include "AppInitDllInjector.h"
#include "IATHookInjector.h"
#include "Logger.h"