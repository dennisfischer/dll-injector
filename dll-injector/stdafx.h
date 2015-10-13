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

#include "Injector.h"
#include "RemoteThreadInjector.h"
#include "WindowsHookInjector.h"
#include "AppInitDllInjector.h"
#include "Logger.h"

#include <TlHelp32.h>
