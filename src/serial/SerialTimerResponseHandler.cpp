#include "SerialTimerResponseHandler.h"

SerialTimerResponseHandler::SerialTimerResponseHandler(Stream * serial, size_t serialRxBufferSize):BaseSerialHandler(serial), messageTimer(this)
{
    this->serialRxBufferSize = serialRxBufferSize;
}

SerialTimerResponseHandler::~SerialTimerResponseHandler()
{
	StopTimer();
}

void SerialTimerResponseHandler::Loop()
{
	BaseSerialHandler::Loop();
	
	if (serial != NULL && registeredBytes < Available()) {
		StartTimer();
        if (registeredBytes >= serialRxBufferSize) {
            ResponseDetectedInternal(false, true);
        }
	}
}

void SerialTimerResponseHandler::OnTimerComplete(Timer *timer)
{
	if (&messageTimer == timer) {
		ResponseDetectedInternal(false, false);
        registeredBytes = Available();
        return;
	}
	BaseSerialHandler::OnTimerComplete(timer);
}

void SerialTimerResponseHandler::StartTimer()
{
	StopTimer();
	registeredBytes = Available();
    //Serial.print("StartTimer ");
    //Serial.println((int)Available());
    messageTimer.StartMicros(ResponseByteTimeOut());
}
void SerialTimerResponseHandler::StopTimer()
{
    //Serial.print("StopTimer ");
    //Serial.println((int)Available());
    messageTimer.Stop();
	StopTimeoutTimer();
}

uint8_t SerialTimerResponseHandler::GetDataBitsAmount()
{
	return 8u;
}
uint8_t SerialTimerResponseHandler::GetStopBitsAmount()
{
	return 1u;
}
uint32_t SerialTimerResponseHandler::GetBaudRate()
{
	return 9600u;
}

unsigned long SerialTimerResponseHandler::ResponseByteTimeOut()
{
	return (unsigned long)(SingleByteTransferDuration() * 2.0);
}

double SerialTimerResponseHandler::SingleByteTransferDuration()
{
	// For baud rate 9600 + 1 start bit + 8 data bit + 1 stop bit
	double bitsPerMessage = 1 + GetDataBitsAmount() + GetStopBitsAmount();
	return ( 1000000.0 / (double)GetBaudRate() * bitsPerMessage);
	//return (((double)bitsPerMessage * 1000.0 / (double)GetBaudRate() * 1000.0));
	//return ((1000.0 / (double)GetBaudRate() * 1000.0));
}

bool SerialTimerResponseHandler::IsBusy()
{
	return !messageTimer.IsRunning() || BaseSerialHandler::IsBusy();
}

void SerialTimerResponseHandler::FlushData()
{
	registeredBytes = 0;
	BaseSerialHandler::FlushData();
}

void SerialTimerResponseHandler::ResponseDetectedInternal(bool isTimeOut, bool isOverFlow)
{
	StopTimer();
	BaseSerialHandler::ResponseDetectedInternal(isTimeOut, isOverFlow);
}

size_t SerialTimerResponseHandler::Available()
{
    if (serial == NULL) return 0;
    int available = serial->available();
    if (available <= 0) return 0;
    return (size_t)available;
}

size_t SerialTimerResponseHandler::Write(const uint8_t data)
{
    if (serial == NULL) return 0;
    return serial->write(data);
}
size_t SerialTimerResponseHandler::Write(const uint8_t *data, size_t size)
{
    if (serial == NULL) return 0;
    if (data == NULL) return 0;
    if (size == 0) return 0;

    return serial->write(data, size);
}

size_t SerialTimerResponseHandler::ReadBytes(uint8_t *data, size_t size)
{
    if (serial == NULL) return 0;
    if (data == NULL) return 0;

    size = min(size, Available());
    if (size == 0) return 0;

    size_t val = serial->readBytes(data, size);
    if (val > registeredBytes) {
        registeredBytes = 0;
    } else {
        registeredBytes -= val;
    }
    return val;
}
int SerialTimerResponseHandler::Read()
{
    if (serial == NULL) return -1;
    return serial->read();
}