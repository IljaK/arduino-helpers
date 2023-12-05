#include "SerialCharResponseHandlerMock.h"

SerialCharResponseHandlerMock::SerialCharResponseHandlerMock(const size_t bufferSize, const char *separator, Stream * stream):SerialCharResponseHandler(bufferSize, separator, stream), BaseSerialMock(stream)
{
	Clear();
}

SerialCharResponseHandlerMock::~SerialCharResponseHandlerMock()
{
}

void SerialCharResponseHandlerMock::OnResponseReceived(bool isTimeOut, bool isOverFlow)
{
	if (bufferLength > 0) {
		strcpy(receivedCommand, buffer);
	}
	HandleResponseReceived(isTimeOut, isOverFlow);
	SerialCharResponseHandler::OnResponseReceived(isTimeOut, isOverFlow);
}

void SerialCharResponseHandlerMock::Clear() {
	receivedCommand[0] = 0;
	BaseSerialMock::Clear();
}


int SerialCharResponseHandlerMock::read()
{
    return 0;
}

int SerialCharResponseHandlerMock::available()
{
    return 0;
}