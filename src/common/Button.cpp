#include "Button.h"

Button::Button(ClickFuncPtr clickCallBack):ITimerCallback(), multiClickTimer(this)
{
    this->clickCallBack = clickCallBack;
}
void Button::Start(uint8_t pin, voidFuncPtr func)
{
    pinState = initPullupPin(pin, INPUT_PULLUP, func);
    btnState = pinState;
}

void Button::Loop()
{
    if (btnState != pinState) {
        if (pinState == LOW) {
            OnBtnPressed();
        } else {
            OnBtnReleased();
        }
    }
}
void Button::OnBtnReleased()
{
    multiClicks++;
    multiClickTimer.StartMicros(MULTI_CLICK_WAIT);
}
void Button::ProcessClick()
{
    uint8_t clicks = multiClicks;
    if (clicks >= MAX_MULTI_CLICK) {
        clicks = MAX_MULTI_CLICK;
    }
    multiClicks = 0;
    if (clickCallBack != NULL) {
        clickCallBack(clicks);
    }
}
void Button::OnBtnPressed()
{
    multiClickTimer.Stop();
}

bool Button::IsPressed()
{
    return btnState == LOW;
}

void Button::OnTimerComplete(Timer *timer)
{
    if (timer == &multiClickTimer) {
        ProcessClick();
    }
}
