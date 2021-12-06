#pragma once

#include <Windows.h>

class BizTeleworkDimension
{
public:
    static LONG DialogMarginRight() { return 16; }
    static LONG DialogMarginBottom() { return 42; }

    static LONG DialogWidth() { return 364; }
    static LONG DialogHeight() { return 144; }

    static LONG ButtonWidth() { return 164; }
    static LONG ButtonHeight() { return 32; }
    static LONG ButtonMarginBottomDialog() { return 16; }
    static LONG SpaceBetween2Buttons() { return 4; }

    static LONG StartButtonMarginRightDialog() { return 16; }

    static LONG ImageDimension() { return 48; }
    static LONG ImageMarginRightDialog() { return 300; }
    static LONG ImageMarginBottomDialog() { return 72; }

    static LONG ImageMarginBottomOnDutyDialog() { return 126; }
    static LONG ImageMarginBottomOnFailedCallApiDialog() { return 90; }

    static LONG StaticCommentWidth() { return 226; }
    static LONG StaticCommentHeight() { return 43; }
    static LONG StaticCommentMarginRightDialog() { return 58; }
    static LONG StaticCommentMarginBottomDialog() { return 77; }

    static LONG ErrorDialogWidth() { return 459; }
    static LONG ErrorDialogHeight() { return 246; }
    static LONG ErrorDialogMarginBottom() { return 202; }

    static LONG ErrorTextLabelWidth() { return 338; }
    static LONG ErrorTextLabelHeight() { return 15; }

    static LONG ErrorTextLabelMarginRightDialog() { return 95; }
    static LONG ErrorTextLabelMarginBottomDialog() { return 207; }

    static LONG ErrorTextContentWidth() { return 405; }
    static LONG ErrorTextContentHeight() { return 180; }

    static LONG ErrorTextContentMarginRightDialog() { return 28; }
    static LONG ErrorTextContentMarginBottomDialog() { return 24; }

    static LONG WidthEllipseRoundedCorner() { return 20; }
    static LONG HeightEllipseRoundedCorner() { return 20; }

    static LONG OffDutyDialogWidth() { return 364; }
    static LONG OffDutyDialogHeight() { return 144; }
    static LONG OffDutyDialogMarginRight() { return 16; }
    static LONG OffDutyDialogMarginBottom() { return 16; }
    static LONG NotYetButtonMarginRight() { return 16; }
    static LONG StartButtonMarginBottom() { return 16; }

    static LONG NotificationWidth() { return 247; }
    static LONG NotificationHeight() { return 43; }
    static LONG NotificationMarginRight() { return 37; }
    static LONG NotificationMarginBottom() { return 77; }

    static LONG OnDutyDialogWidth() { return 364; }
    static LONG OnDutyDialogHeight() { return 198; }
    static LONG OnDutyDialogMarginRight() { return 16; }
    static LONG OnDutyDialogMarginBottom() { return 16; }
    static LONG EndButtonMarginRight() { return 16; }
    static LONG EndButtonMarginBottom() { return 16; }

    static LONG FailedCallApiDialogWidth() { return 364; }
    static LONG FailedCallApiDialogHeight() { return 162; }
    static LONG FailedCallApiDialogMarginRight() { return 16; }
    static LONG FailedCallApiDialogMarginBottom() { return 16; }
    static LONG RetryButtonMarginRight() { return 100; }
    static LONG RetryButtonMarginBottom() { return 16; }
    static LONG NotificationFailedCallApiDialogWidth() { return 193; }
    static LONG NotificationFailedCallApiDialogHeight() { return 67; }
    static LONG NotificationFailedCallApiDialogMarginRight() { return 91; }
    static LONG NotificationFailedCallApiDialogMarginBottom() { return 72; }

    static LONG OperationLabelWidth() { return 56; }
    static LONG OperationLabelHeight() { return 15; }
    static LONG OperationLabelMarginRight() { return 226; }
    static LONG OperationLabelMarginBottom() { return 155; }

    static LONG OperationContentWidth() { return 181; }
    static LONG OperationContentHeight() { return 33; }
    static LONG OperationContentMarginRight() { return 38; }
    static LONG OperationContentMarginBottom() { return 149; }

    static LONG NotificationTerminalTimeWidth() { return 266; }
    static LONG NotificationTerminalTimeHeight() { return 72; }
    static LONG NotificationTerminalTimeMarginRight() { return 16; }
    static LONG NotificationTerminalTimeMarginBottom() { return 72; }

    static LONG HealthStatusDialogWidth() { return 364; }
    static LONG HealthStatusDialogHeight() { return 139; }
    static LONG HealthStatusDialogMarginRight() { return 16; }
    static LONG HealthStatusDialogMarginBottom() { return 16; }

    static LONG FeelingQuestionWidth() { return 190; }
    static LONG FeelingQuestionHeight() { return 43; }
    static LONG FeelingQuestionMarginRight() { return 94; }
    static LONG FeelingQuestionMarginBottom() { return 72; }

    static LONG HealthStatusButtonWidth() { return 108; }
    static LONG HealthStatusButtonHeight() { return 32; }
    static LONG HealthStatusButtonMarginBottom() { return 16; }
    static LONG BadStatusButtonMarginRight() { return 16; }

    static LONG DayOffVerifyDialogWidth() { return 364; }
    static LONG DayOffVerifyDialogHeight() { return 157; }
    static LONG DayOffVerifyDialogMarginRight() { return 16; }
    static LONG DayOffVerifyDialogMarginBottom() { return 13; }

    static LONG TakeBreakQuestionWidth() { return 247; }
    static LONG TakeBreakQuestionHeight() { return 21; }
    static LONG TakeBreakQuestionMarginRight() { return 37; }
    static LONG TakeBreakQuestionMarginBottom() { return 118; }

    static LONG ButtonDoWorkingMarginRight() { return 16; }
    static LONG ButtonOffDayMarginBottom() { return 29; }

    static LONG ActiveDialogWidth() { return 364; }
    static LONG ActiveDialogHeightLess() { return 432; }
    static LONG ActiveDialogHeightMore() { return 498; }
    static LONG CloseWidth() { return 24; }
    static LONG CloseHeight() { return 24; }
    static LONG CloseMarginRight() { return 16; }
    static LONG CloseMarginTop() { return 16; }
    static LONG LoginButtonWidth() { return 164; }
    static LONG LoginButtonHeight() { return 32; }
    static LONG LoginButtonMarginRight() { return 16; }
    static LONG LoginButtonMarginBottom() { return 32; }
    static LONG EditTextWidth() { return 332; }
    static LONG EditTextHeight() { return 32; }
    static LONG EditTextMarginRight() { return 16; }
    static LONG EditCompanyCodeMarginBottomLess() { return 200; }
    static LONG EditActiveCodeMarginBottomLess() { return 144; }
    static LONG EditCompanyCodeMarginBottomMore() { return 248; }
    static LONG EditActiveCodeMarginBottomMore() { return 192; }
    static LONG EditUpdateURLMarginBottomMore() { return 96; }

    static LONG ImageMarginRight() { return 158; }
    static LONG ImageMarginBottomLess() { return 364; }
    static LONG ImageMarginBottomMore() { return 412; }
    static LONG ErrorTextMarginBottomLess() { return 241; }
    static LONG ErrorTextMarginBottomMore() { return 292; }
    static LONG StaticEnterURLWidth() { return 145; }
    static LONG StaticEnterURLHeight() { return 16; }
    static LONG StaticEnterURLMarginRight() { return 115; }
    static LONG StaticEnterURLMarginBottomLess() { return 100; }
    static LONG StaticEnterURLMarginBottomMore() { return 148; }
    static LONG ExpandArrowDimension() { return 20; }
    static LONG ExpandArrowMarginRight() { return 95; }
    static LONG ExpandArrowMarginBottomLess() { return 97; }
    static LONG ExpandArrowMarginBottomMore() { return 145; }
    static LONG ErrorTextWidth() { return 332; }
    static LONG ErrorTextHeight() { return 14; }
    static LONG StaticAuthExplanationWidth() { return 332; }
    static LONG StaticAuthExplanationHeight() { return 88; }
    static LONG StaticAuthExplanationMarginRight() { return 16; }
    static LONG StaticAuthExplanationnBottomLess() { return 264; }
    static LONG StaticAuthExplanationnBottomMore() { return 314; }

    [[nodiscard]] static LONG WorkDayTargetDialogWidth() { return 364; }
    [[nodiscard]] static LONG WorkDayTargetDialogHeight() { return 287; }
    [[nodiscard]] static LONG WorkDayTargetMarginRight() { return 16; }
    [[nodiscard]] static LONG WorkDayTargetMarginBottom() { return 16; }
    [[nodiscard]] static LONG StartWorkingButtonMarginRight() { return 16; }
    [[nodiscard]] static LONG StartWorkingButtonMarginBottom() { return 16; }
    [[nodiscard]] static LONG ImageMarginRightTargetDialog() { return 300; }
    [[nodiscard]] static LONG ImageMarginBottomTargetDialog() { return 215; }
    [[nodiscard]] static LONG StaticDoWhatWidth() { return 245; }
    [[nodiscard]] static LONG StaticDoWhatHeight() { return 19; }
    [[nodiscard]] static LONG StaticDoWhatMarginRight() { return 39; }
    [[nodiscard]] static LONG StaticDoWhatMarginBottom() { return 244; }
    [[nodiscard]] static LONG EditWorkDayTargetWidth() { return 332; }
    [[nodiscard]] static LONG EditWorkDayTargetHeight() { return 127; }
    [[nodiscard]] static LONG EditWorkDayTargetMarginRight() { return 16; }
    [[nodiscard]] static LONG EditWorkDayTargetMarginBottom() { return 64; }
};

