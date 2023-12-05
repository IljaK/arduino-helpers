#include "SerialTimerResponseHandlerMock.h"

SerialTimerResponseHandlerMock::SerialTimerResponseHandlerMock(Stream *serial): SerialTimerResponseHandler(serial, 2048), BaseSerialMock(serial)
{
	receivedCommand[0] = 0;
	receivedLength = 0;
}

SerialTimerResponseHandlerMock::~SerialTimerResponseHandlerMock()
{
}

void SerialTimerResponseHandlerMock::OnResponseReceived(bool isTimeOut, bool isOverFlow)
{
	if (registeredBytes > 0) {
		serialStream->readBytes(receivedCommand, registeredBytes);
	}
	receivedLength = registeredBytes;
	BaseSerialMock::HandleResponseReceived(isTimeOut, isOverFlow);
	SerialTimerResponseHandler::OnResponseReceived(isTimeOut, isOverFlow);
}

void SerialTimerResponseHandlerMock::Clear() {
	receivedCommand[0] = 0;
	BaseSerialMock::Clear();
}