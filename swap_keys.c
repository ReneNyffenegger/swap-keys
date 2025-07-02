//
//   http://stackoverflow.com/a/27644207/180275
//
#include <stdio.h>

#ifdef __CYGWIN__ // 2017-07-18 (tsettgchabe)
// #define _WIN32_WINNT 0x0404
#define _WIN32_WINNT 0x0500
#endif
#include <windows.h>

HHOOK hook;

#ifdef AS_DLL

int i = 0;

BOOL WINAPI DllMain (
    HINSTANCE hInstDLL,
    DWORD     fdwReason,
    LPVOID    lpvReserved

) {

//  Oh, wy are such things even necessary and what can
//  be done to change it.
  if (! i++) {
    MessageBox(0, "DllMain", "Swap keys", 0);
  }
}

#endif

LRESULT CALLBACK keyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {

    KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *) lParam;

    DWORD newVkCode;
    INPUT inputs[1];
    UINT ret;

    char  wParamStr[11];
    char *vkStr;

    if (nCode < 0) {
     //
     // nCode: A code the hook procedure uses to determine how to process the message.
     // If nCode is less than zero, the hook procedure must pass the message to
     // the CallNextHookEx function without further processing and should return
     // the value returned by CallNextHookEx.
     //
        return CallNextHookEx(hook, nCode, wParam, lParam);
    }

#ifdef SHOW_INFO

    if      (wParam == WM_KEYDOWN)    strcpy(wParamStr, "KEYDOWN");
    else if (wParam == WM_KEYUP)      strcpy(wParamStr, "KEYUP");
    else if (wParam == WM_SYSKEYDOWN) strcpy(wParamStr, "SYSKEYDOWN");
    else if (wParam == WM_SYSKEYUP)   strcpy(wParamStr, "SYSKEYUP");
    else                              strcpy(wParamStr, "UNKNOWN");

    if      (p->vkCode ==   9) vkStr = "<TAB>"    ;
    else if (p->vkCode ==  10) vkStr = "<LF>"     ;
    else if (p->vkCode ==  13) vkStr = "<CR>"     ;
    else if (p->vkCode ==  20) vkStr = "<CAPSLCK>";
    else if (p->vkCode ==  27) vkStr = "<ESC>"    ;
    else if (p->vkCode ==  91) vkStr = "<L-WIN>"  ;
    else if (p->vkCode ==  92) vkStr = "<R-WIN>"  ;
    else if (p->vkCode ==  93) vkStr = "<APPS>"   ;
    else if (p->vkCode == 161) vkStr = "<L-SHIFT>";
    else if (p->vkCode == 162) vkStr = "<L-CTRL>" ;
    else if (p->vkCode == 163) vkStr = "<R-CTRL>" ;
    else if (p->vkCode == 164) vkStr = "<L-ALT>"  ;
    else if (p->vkCode == 165) vkStr = "<R-ALT>"  ;
    else if (p->vkCode == 226) vkStr = "<R-SHIFT>";
    else vkStr = "";

    char wndTitle[256];
    HWND hwnd=GetForegroundWindow();
    GetWindowText(hwnd, wndTitle, sizeof(wndTitle));

    WINDOWPLACEMENT wp;
    wp.length = sizeof(wp);

    GetWindowPlacement(hwnd, &wp);
    char* showCmd;
    if (wp.showCmd == SW_MAXIMIZE) { showCmd = "    SW_MAXIMIZE"; }
    else                           { showCmd = "not SW_MAXIMIZE"; }

    printf(" %-10s - %3lu | %-9s | %3lu - %lu | %-30s %s\n", wParamStr, p->vkCode, vkStr, p->scanCode, p->time, wndTitle, showCmd);

#endif

    inputs[0].type           = INPUT_KEYBOARD;
    inputs[0].ki.wScan       = 0;
    inputs[0].ki.dwFlags     = 0;
    inputs[0].ki.time        = 0;
    inputs[0].ki.dwExtraInfo = 0;

    if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    if ((p->flags & LLKHF_INJECTED) == 0) {
     //
     // If the LLKHF_INJECTED (= 0x10) bit is set, the event was injected.
     //
     // With the previous test, we skip over injected events and only
     // change «original» events.
     //
        if (p->vkCode == VK_ESCAPE) {                    // ESC (27) -> CAPS LOCK (20)
            inputs[0].ki.wVk = VK_CAPITAL;
            ret = SendInput(1, inputs, sizeof (INPUT));
            return 1;
        }
        if (p->vkCode == VK_CAPITAL) {                   // CAPS LOCK (20) -> ESC (27)
            inputs[0].ki.wVk = VK_ESCAPE;
            ret = SendInput(1, inputs, sizeof (INPUT));
            return 1;
        }
        if (p->vkCode == VK_RMENU) {                     // Right Alt (165) -> Right Ctrl (163)
            inputs[0].ki.wVk = VK_RCONTROL;
            ret = SendInput(1, inputs, sizeof (INPUT));
            return 1;
        }
    }


    return CallNextHookEx(hook, nCode, wParam, lParam);
}

#ifdef AS_DLL
extern // 2017-07-21 - tsettchabe / CYGWIN
__declspec(dllexport)
#endif
int
#ifdef AS_DLL
__stdcall
#endif
startSwapKeys() {

    MSG messages;

    hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHook, NULL, 0);

    if (hook == NULL) {
        printf("Error %lu\n", GetLastError());
        return 1;
    }

#ifndef AS_DLL

    printf("Waiting for messages ...\n");

    while (GetMessage (&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

#endif
    return 0;

}

#ifndef AS_DLL
int main(int argc, char **argv) {
    return startSwapKeys();
}
#endif
