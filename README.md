This repository contains sample code for dll and code injection. Currently 3 ways are supported.
* CreateRemoteThread & LoadLibrary (RemoteThreadInjector)
* SetWindowsHookEx (WindowsHookInjector)
* AppInit_DLLs (AppInitDllInjector, requires **admin** privileges!)

Injection techniques for more direct code injection will be added soon!
ToDo:
* Code Cave injection
* CreateRemoteThread + "direct" code injection (not using a dll / just using it's contents)
* Function Detours
* IAT Hooks
* PE Header modification
* DLL Replacement
* Custom Debugger