// stdafx.h: Includedatei f�r Standardsystem-Includedateien
// oder h�ufig verwendete projektspezifische Includedateien,
// die nur in unregelm��igen Abst�nden ge�ndert werden.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <string>


// TODO: Hier auf zus�tzliche Header, die das Programm erfordert, verweisen.
#include <Windows.h>
#include <iostream>

#include "Injector.h"
#include "RemoteThreadInjector.h"
#include "WindowsHookInjector.h"
#include "AppInitDllInjector.h"
#include "Logger.h"

#include <TlHelp32.h>
