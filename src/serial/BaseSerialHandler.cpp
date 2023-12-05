#include "BaseSerialHandler.h"

BaseSerialHandler::BaseSerialHandler(Stream * serial):ITimerCallback(), responseTimeoutTimer(this)
{
	this->serial = serial;
}

BaseSerialHandler::~BaseSerialHandler()
{
	StopTimeoutTimer();
}

void BaseSerialHandler::OnTimerComplete(Timer *timer)
{
	if (&responseTimeoutTimer == timer) {
		ResponseDetectedInternal(true, false);
	}
}

void BaseSerialHandler::ResponseDetectedInternal(bool isTimeOut, bool isOverFlow)
{
	StopTimeoutTimer();
	OnResponseReceived(isTimeOut, isOverFlow);
}

void BaseSerialHandler::OnResponseReceived(bool isTimeOut, bool isOverFlow)
{
}

bool BaseSerialHandler::IsBusy()
{
	return responseTimeoutTimer.IsRunning();
}

void BaseSerialHandler::FlushData()
{
    // Waits for the transmission of outgoing serial data to complete. 
    // (Prior to Arduino 1.0, this instead removed any buffered incoming serial data.)
	// serial->flush(); // Do not use this!
    
    u_int32_t count = 0;
    while (serial->available() > 0) {
        count++;
        int val = serial->read();
        if (val <= 0) {
            break;
        }
        // For safe reasons
        if (count > 4096) {
            break;
        }
    }
}

void BaseSerialHandler::Loop()
{
}

void BaseSerialHandler::StartTimeoutTimer(unsigned long microSecTimeOut)
{
	if (microSecTimeOut == 0) return;
	StopTimeoutTimer();
    responseTimeoutTimer.StartMicros(microSecTimeOut);
}
void BaseSerialHandler::StopTimeoutTimer()
{
	responseTimeoutTimer.Stop();
}


void BaseSerialHandler::SetResponseCallBack(SerialResponseCallback responseCallback)
{
    this->responseCallback = responseCallback;
}
