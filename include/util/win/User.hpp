#pragma once

#include <string_view>

#include <Windows.h>

namespace z3lx::util {
enum class MessageBoxButton : long {
    AbortRetryIgnore = MB_ABORTRETRYIGNORE,
    CancelTryContinue = MB_CANCELTRYCONTINUE,
    Help = MB_HELP,
    Ok = MB_OK,
    OkCancel = MB_OKCANCEL,
    RetryCancel = MB_RETRYCANCEL,
    YesNo = MB_YESNO,
    YesNoCancel = MB_YESNOCANCEL
};

enum class MessageBoxIcon : long {
    Warning = MB_ICONWARNING,
    Information = MB_ICONINFORMATION,
    Error = MB_ICONERROR,
    Question = MB_ICONQUESTION
};

enum class MessageBoxDefaultButton : long {
    Button1 = MB_DEFBUTTON1,
    Button2 = MB_DEFBUTTON2,
    Button3 = MB_DEFBUTTON3,
    Button4 = MB_DEFBUTTON4
};

enum class MessageBoxResult : int {
    Abort = IDABORT,
    Cancel = IDCANCEL,
    Continue = IDCONTINUE,
    Ignore = IDIGNORE,
    No = IDNO,
    Ok = IDOK,
    Retry = IDRETRY,
    TryAgain = IDTRYAGAIN,
    Yes = IDYES
};

MessageBoxResult ShowMessageBox(
    std::string_view title,
    std::string_view message,
    MessageBoxIcon icon = MessageBoxIcon::Information,
    MessageBoxButton button = MessageBoxButton::Ok,
    MessageBoxDefaultButton defaultButton = MessageBoxDefaultButton::Button1
);

MessageBoxResult ShowMessageBox(
    std::wstring_view title,
    std::wstring_view message,
    MessageBoxIcon icon = MessageBoxIcon::Information,
    MessageBoxButton button = MessageBoxButton::Ok,
    MessageBoxDefaultButton defaultButton = MessageBoxDefaultButton::Button1
);
} // namespace z3lx::util
