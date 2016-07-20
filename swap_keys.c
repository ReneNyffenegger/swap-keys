//
//   http://stackoverflow.com/a/27644207/180275
//
#include <stdio.h>
#include <windows.h>

HHOOK hook;

LRESULT CALLBACK keyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {

    if (nCode < 0) {
    // nCode: A code the hook procedure uses to determine how to process the message.
    // If nCode is less than zero, the hook procedure must pass the message to
    // the CallNextHookEx function without further processing and should return
    // the value returned by CallNextHookEx.
      return CallNextHookEx(hook, nCode, wParam, lParam);
    }

    KBDLLHOOKSTRUCT *p = (KBDLLHOOKSTRUCT *) lParam;
  
    DWORD newVkCode;
    INPUT inputs[1];
    UINT ret;

    char wParamStr[11];
    char vkStr    [10] = "";

    if      (wParam == WM_KEYDOWN)    strcpy(wParamStr, "KEYDOWN");
    else if (wParam == WM_KEYUP)      strcpy(wParamStr, "KEYUP");
    else if (wParam == WM_SYSKEYDOWN) strcpy(wParamStr, "SYSKEYDOWN");
    else if (wParam == WM_SYSKEYUP)   strcpy(wParamStr, "SYSKEYUP");
    else                              strcpy(wParamStr, "UNKNOWN");

    if      (p->vkCode ==   9) strcpy(vkStr, "<TAB>"    );
    else if (p->vkCode ==  10) strcpy(vkStr, "<LF>"     );
    else if (p->vkCode ==  13) strcpy(vkStr, "<CR>"     );
    else if (p->vkCode ==  20) strcpy(vkStr, "<CAPSLCK>");
    else if (p->vkCode ==  27) strcpy(vkStr, "<ESC>"    );
    else if (p->vkCode == 161) strcpy(vkStr, "<L-SHIFT>");
    else if (p->vkCode == 162) strcpy(vkStr, "<L-CTRL>" );
    else if (p->vkCode == 163) strcpy(vkStr, "<R-CTRL>" );
    else if (p->vkCode == 164) strcpy(vkStr, "<L-ALT>"  );
    else if (p->vkCode == 165) strcpy(vkStr, "<R-ALT>"  );
    else if (p->vkCode == 226) strcpy(vkStr, "<R-SHIFT>");
    else vkStr[0] = p->vkCode;

    printf(" %-10s - %3lu | %9s | %3d - %lu\n", wParamStr, p->vkCode, vkStr, p->scanCode, p->time);

    inputs[0].type           = INPUT_KEYBOARD;
    inputs[0].ki.wScan       = 0;
    inputs[0].ki.dwFlags     = 0;
    inputs[0].ki.time        = 0;
    inputs[0].ki.dwExtraInfo = 0;

    if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP) {
        inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    if ((p->flags & LLKHF_INJECTED) == 0) {

      if (p->vkCode == 27) { // ESC -> CAPSLCK
          inputs[0].ki.wVk = 20;
          ret = SendInput(1, inputs, sizeof (INPUT));
          return 1;
      }
      if (p->vkCode == 20) { // CAPSLCK -> ESC
          inputs[0].ki.wVk = 27;
          ret = SendInput(1, inputs, sizeof (INPUT));
          return 1;
      }
      if (p->vkCode == 165) { // Right Alt -> Right Ctrl
          inputs[0].ki.wVk = 163;
          ret = SendInput(1, inputs, sizeof (INPUT));
          return 1;
      }

    }


    return CallNextHookEx(hook, nCode, wParam, lParam);
}

int main(int argc, char **argv) {

    MSG messages;

    hook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHook, NULL, 0);

    if (hook == NULL) {
        printf("Error %d\n", GetLastError());
        return 1;
    }

    printf("Waiting for messages ...\n");

    while (GetMessage (&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }
    return 0;
}