#pragma once

#include <Windows.h>

class BizTeleworkColor
{
public:
    static COLORREF ButtonBackground() { return RGB(0x4B, 0x4B, 0x4B); }
    static COLORREF ButtonText() { return RGB(0xFF, 0xFF, 0xFF); }
    static COLORREF DialogBackground() { return RGB(0x13, 0x1C, 0x23); }
    static COLORREF StaticText() { return RGB(0xFF, 0xFF, 0xFF); }
    static COLORREF ButtonDisableColor() { return RGB(0x33, 0x33, 0x33); }
    static COLORREF ErrorDialogBackground() { return RGB(0x00, 0x00, 0x00); }
    static COLORREF InputBackground() { return RGB(0x0B, 0x10, 0x14); }
    static COLORREF InputBorder() { return RGB(0x72, 0x77, 0x7B); }
    static COLORREF InputText() { return RGB(0xFF, 0xFF, 0xFF); }
    static COLORREF ErrorText() { return RGB(0xB0, 0x3C, 0x4F); }
    static COLORREF AuthSuccessfulText() { return RGB(0x2A, 0xA2, 0x74); }
    [[nodiscard]] static COLORREF InputWorkDayTargetBackground() { return RGB(0x0B, 0x10, 0x14); }
};
