#include <windows.h>
#include <tchar.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include "resource.h"

#define ID_EDIT 1001
#define ID_BUTTON 1002
#define ID_RESULT 1003
#define ID_STATUS 1004
#define ID_IMAGE 1005
// #define IDI_ICON1 1006
// #define IDB_BASEBALL 1007


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void GenerateAnswer(TCHAR[]);
void CheckGuess(HWND, const TCHAR[], TCHAR[]);

TCHAR answer[4];
int tryCount = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    srand((unsigned)time(0));
    GenerateAnswer(answer);

    const TCHAR CLASS_NAME[] = _T("NumberBaseballWindowClass");

    WNDCLASS wc = { };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = CreateSolidBrush(RGB(0, 100, 150)); // 청록색 배경
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    if (!wc.hIcon) {
        MessageBox(NULL, _T("아이콘 로드 실패! icon.ico 확인"), _T("오류"), MB_OK | MB_ICONERROR);
    }
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        CLASS_NAME,
        _T("NumBaseball"),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 270, 250, // 창 크기 증가
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        MessageBox(NULL, _T("창 생성 실패!"), _T("오류"), MB_OK | MB_ICONERROR);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hEdit, hButton, hResult, hStatus, hImage;

    switch (msg) {
    case WM_CREATE: {
        // 입력창
        hEdit = CreateWindow(_T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            20, 20, 140, 20, hwnd, (HMENU)ID_EDIT, NULL, NULL);

        // 확인 버튼
        hButton = CreateWindow(_T("BUTTON"), _T("확인"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            170, 20, 60, 20, hwnd, (HMENU)ID_BUTTON, NULL, NULL);

        // 결과 표시 영역 (ListBox)
        hResult = CreateWindow(_T("LISTBOX"), _T(""), WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER,
            20, 50, 210, 120, hwnd, (HMENU)ID_RESULT, NULL, NULL);

        // 상태바 (시도 횟수)
        hStatus = CreateWindow(_T("STATIC"), _T("시도 횟수: 0"), WS_CHILD | WS_VISIBLE,
            20, 170, 100, 20, hwnd, (HMENU)ID_STATUS, NULL, NULL);

        // 이미지 표시 (STATIC, SS_BITMAP)
        hImage = CreateWindow(_T("STATIC"), NULL, WS_CHILD | WS_VISIBLE | SS_BITMAP,
            130, 170, 100, 20, hwnd, (HMENU)ID_IMAGE, NULL, NULL);

        // Windows 95 스타일 폰트 설정
        HFONT hFont = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, _T("MS Sans Serif"));
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hResult, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(hStatus, WM_SETFONT, (WPARAM)hFont, TRUE);

        // 비트맵 로드 및 설정
        HBITMAP hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BASEBALL), IMAGE_BITMAP, 100, 20, 0);
        if (hBitmap) {
            // LRESULT result = SendMessage(hImage, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);
            // if (!result) {
            //     // MessageBox(hwnd, _T("STM_SETIMAGE 실패!"), _T("오류"), MB_OK | MB_ICONERROR);
            // }
            HBITMAP hPrev = (HBITMAP)SendMessage(hImage, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
            if (hPrev == NULL && hBitmap == NULL) {
                MessageBox(hwnd, _T("STM_SETIMAGE 실패!"), _T("오류"), MB_OK | MB_ICONERROR);
            }
            InvalidateRect(hImage, NULL, TRUE);
            UpdateWindow(hImage);
        } else {
           TCHAR error[256];
            _stprintf_s(error, _T("이미지 로드 실패! 코드: %d"), GetLastError());
            MessageBox(hwnd, error, _T("오류"), MB_OK | MB_ICONERROR);
        }
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BUTTON) {
            TCHAR guess[4];
            GetWindowText(hEdit, guess, 4);
            CheckGuess(hwnd, guess, answer);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void GenerateAnswer(TCHAR answer[]) {
    int digits[3];
    for (int i = 0; i < 3; i++) {
        int num;
        do {
            num = rand() % 10;
        } while (std::find(digits, digits + i, num) != digits + i);
        digits[i] = num;
    }
    _stprintf_s(answer, 4, _T("%d%d%d"), digits[0], digits[1], digits[2]);
}

void CheckGuess(HWND hwnd, const TCHAR guess[], TCHAR answer[]) {
    HWND hResult = GetDlgItem(hwnd, ID_RESULT);
    HWND hStatus = GetDlgItem(hwnd, ID_STATUS);
    HWND hEdit = GetDlgItem(hwnd, ID_EDIT);
    HWND hButton = GetDlgItem(hwnd, ID_BUTTON);

    if (_tcslen(guess) != 3 || !_istdigit(guess[0]) || !_istdigit(guess[1]) || !_istdigit(guess[2]) ||
        guess[0] == guess[1] || guess[1] == guess[2] || guess[0] == guess[2]) {
        SendMessage(hResult, LB_ADDSTRING, 0, (LPARAM)_T("유효한 3자리 숫자를 입력!!(중복x)"));
        return;
    }

    tryCount++;
    TCHAR statusText[20];
    _stprintf_s(statusText, _T("시도 횟수: %d"), tryCount);
    SetWindowText(hStatus, statusText);

    int strikes = 0, balls = 0;
    for (int i = 0; i < 3; i++) {
        if (guess[i] == answer[i]) {
            strikes++;
        }
        else if (_tcschr(answer, guess[i])) {
            balls++;
        }
    }

    TCHAR result[50];
    _stprintf_s(result, _T("%s: %d 스트라이크, %d 볼"), guess, strikes, balls);
    SendMessage(hResult, LB_ADDSTRING, 0, (LPARAM)result);

    if (strikes == 3) {
        TCHAR winMsg[50];
        _stprintf_s(winMsg, _T("축하합니다! %d번 만에 성공!"), tryCount);
        SendMessage(hResult, LB_ADDSTRING, 0, (LPARAM)winMsg);
        EnableWindow(hEdit, FALSE);
        EnableWindow(hButton, FALSE);
    }

    SetWindowText(hEdit, _T(""));
}
/*
---

### 3. **코드 변경 사항**
- **창 크기**:
  - `CreateWindow`에서 창 크기를 `(260, 250)` → `(300, 280)`으로 늘려 이미지와 레이아웃 공간 확보.
- **결과창 크기**:
  - `hResult` (ListBox) 크기를 `(200, 100)` → `(240, 120)`으로 조정해 창에 맞춤.
- **상태바와 이미지 위치**:
  - `hStatus` (시도 횟수): `(20, 180)`, 크기 `100x20`.
  - `hImage` (이미지): `(130, 180)`, 크기 `100x28` (BMP 크기에 맞춤).
- **이미지 로드**:
  - `LoadImage`에서 절대 경로 `C:\\NumberBaseball\\baseball.bmp` 사용.
  - 크기 명시: `100, 28`.
  - 오류 메시지에 `GetLastError()` 코드 추가.

---

### 4. **VS Code에서 컴파일 및 실행**
Visual Studio Community의 MSVC 컴파일러를 사용해 빌드합니다.

#### 4.1. **tasks.json**
`.vscode/tasks.json` 확인:
```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "type": "cppbuild",
      "label": "C/C++: cl.exe build active file",
      "command": "cl.exe",
      "args": [
        "/Zi",
        "/Fe${fileDirname}\\NumberBaseball.exe",
        "${file}",
        "/link",
        "/SUBSYSTEM:WINDOWS",
        "user32.lib",
        "gdi32.lib",
        "winmm.lib"
      ],
      "options": {
        "cwd": "${fileDirname}"
      },
      "problemMatcher": ["$msCompile"],
      "group": {
        "kind": "build",
        "isDefault": true
      },
      "detail": "Task for building with MSVC"
    }
  ]
}
```

#### 4.2. **launch.json**
`.vscode/launch.json` 확인:
```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "(Windows) Launch",
      "type": "cppvsdbg",
      "request": "launch",
      "program": "${fileDirname}\\NumberBaseball.exe",
      "args": [],
      "stopAtEntry": false,
      "cwd": "${fileDirname}",
      "environment": [],
      "console": "integratedTerminal",
      "preLaunchTask": "C/C++: cl.exe build active file"
    }
  ]
}
```

#### 4.3. **빌드 및 실행**
1. **파일 준비**:
   - `baseball.bmp`를 `C:\NumberBaseball\baseball.bmp`에 저장.
   - BMP 파일이 24비트 또는 8비트 포맷인지 확인 (MS Paint로 저장).
2. **빌드**:
   - VS Code에서 `main.cpp` 열기.
   - **Terminal > Run Build Task** (`Ctrl+Shift+B`).
   - `C:\NumberBaseball\NumberBaseball.exe` 생성 확인.
3. **실행**:
   - `NumberBaseball.exe` 더블클릭 또는 **Run > Start Debugging** (`F5`).
   - 창이 열리며 시도 횟수 오른쪽에 100x28 이미지 표시 확인.

---

### 5. **문제 해결**
- **이미지 안 보임**:
  - **파일 경로**: `C:\NumberBaseball\baseball.bmp` 존재 확인.
    - 경로 문제 시 상대 경로 `_T("baseball.bmp")`로 변경하고, `.exe`와 동일 폴더에 배치.
  - **BMP 포맷**: MS Paint로 열어 "24-bit Bitmap"으로 다시 저장.
  - **크기 불일치**: `hImage` 크기가 `100x28`인지 확인.
  - **LoadImage 실패**:
    ```cpp
    if (!hBitmap) {
        TCHAR error[256];
        _stprintf_s(error, _T("이미지 로드 실패! 코드: %d"), GetLastError());
        MessageBox(hwnd, error, _T("오류"), MB_OK | MB_ICONERROR);
    }
    ```
    - 오류 코드 확인 (예: 2 = 파일 없음, 1816 = 잘못된 형식).
- **cl.exe 인식 안 됨**:
  - **Developer Command Prompt** 실행:
    ```bash
    "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"
    ```
  - VS Code 터미널에서 실행 후 빌드 재시도.
- **이미지 잘림**:
  - 창 크기 `(300, 280)`과 `hImage` 위치 `(130, 180)` 확인.
  - ListBox 크기 `(240, 120)` 조정.
- **컴파일 오류**:
  - `user32.lib`, `gdi32.lib` 누락: `tasks.json`의 `args` 확인.
  - `unresolved external symbol WinMain`: `/SUBSYSTEM:WINDOWS` 확인.

---

### 6. **테스트**
- **UI 확인**:
  - 창: 300x280, 청록색 배경, Windows 95 스타일.
  - 시도 횟수: `(20, 180)`, 크기 `100x20`.
  - 이미지: `(130, 180)`, 크기 `100x28`, `baseball.bmp` 표시.
- **게임 플레이**:
  - 3자리 숫자 입력 (예: `456`).
  - "확인" 버튼 클릭 시 결과창에 스트라이크/볼 표시.
  - 유효하지 않은 입력 시 경고 메시지.
  - 시도 횟수 업데이트 및 이미지 유지 확인.
  - 3 스트라이크 시 승리 메시지.
- **이미지 확인**:
  - 시도 횟수 오른쪽에 100x28 `baseball.bmp` 표시.
  - 이미지 로드 실패 시 오류 메시지.

---

### 7. **추가 개선 (선택 사항)**
- **리소스 포함**:
  - `.bmp`를 리소스로 포함:
    1. `C:\NumberBaseball\resource.rc` 생성:
       ```rc
       IDB_BASEBALL BITMAP "baseball.bmp"
       ```
    2. `main.cpp`에서:
       ```cpp
       HBITMAP hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BASEBALL), IMAGE_BITMAP, 100, 28, 0);
       ```
    3. MSVC로 리소스 컴파일:
       ```bash
       rc.exe resource.rc
       ```
       `tasks.json`에 추가:
       ```json
       "args": [
         "/Zi",
         "/Fe${fileDirname}\\NumberBaseball.exe",
         "${file}",
         "${fileDirname}\\resource.res",
         "/link",
         "/SUBSYSTEM:WINDOWS",
         "user32.lib",
         "gdi32.lib",
         "winmm.lib"
       ]
       ```
- **사운드 추가**:
  - 승리 시:
    ```cpp
    PlaySound(_T("C:\\Windows\\Media\\tada.wav"), NULL, SND_ASYNC);
    ```
  - `winmm.lib` 포함 확인.
- **배포**:
  - `NumberBaseball.exe`와 `baseball.bmp`를 함께 배포.
  - Inno Setup으로 설치 프로그램:
    ```ini
    [Setup]
    AppName=Number Baseball
    AppVersion=1.0
    DefaultDirName={pf}\NumberBaseball
    OutputBaseFilename=NumberBaseballInstaller
    [Files]
    Source: "C:\NumberBaseball\NumberBaseball.exe"; DestDir: "{app}"
    Source: "C:\NumberBaseball\baseball.bmp"; DestDir: "{app}"
    [Icons]
    Name: "{commonprograms}\NumberBaseball"; Filename: "{app}\NumberBaseball.exe"
    ```

---

### 8. **다음 단계**
- `baseball.bmp` 파일을 공유하거나 포맷/내용 설명 시 추가 디버깅 가능.
- 오류 메시지(예: `GetLastError()` 코드)나 스크린샷 제공 시 정확한 문제 해결 가능.
- 창 크기나 이미지 위치 추가 조정 필요 시 요청 주세요.

`NumberBaseball.exe` 실행 후 이미지 표시 확인되면 알려주시고, 추가 수정(예: 이미지 애니메이션, 다른 UI 요소) 필요하면 말씀해주세요!
*/