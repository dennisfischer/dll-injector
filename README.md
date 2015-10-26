This repository contains sample code for dll and code injection. Currently 3 ways are supported.
* CreateRemoteThread & LoadLibrary (RemoteThreadInjector)
* SetWindowsHookEx (WindowsHookInjector)
* AppInit_DLLs (AppInitDllInjector, requires **admin** privileges!)
* IAT Hooks [1] (IATHookInjector, fails with Error 299)

Injection techniques for more direct code injection will be added soon!
ToDo:
* Code Cave injection
* CreateRemoteThread + "direct" code injection (not using a dll / just using it's contents)
* Function Detours
* PE Header modification
* DLL Replacement
* Custom Debugger


Sources
[1]: http://www.autosectools.com/IAT-Hooking-Revisited.pdf