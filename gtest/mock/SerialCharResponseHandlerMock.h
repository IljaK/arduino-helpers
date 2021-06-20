#pragma once
#include "serial/SerialCharResponseHandler.h"
#include "SerialStream.h"
#include "BaseSerialHandlerMock.h"

class SerialCharResponseHandlerMock: public SerialCharResponseHandler, public BaseSerialMock
{
public:
	char receivedCommand[SERIAL_RX_BUFFER_SIZE];

	SerialCharResponseHandlerMock(const size_t bufferSize, const char *separator, SerialStream* stream);
	~SerialCharResponseHandlerMock();

	void OnResponseReceived(bool IsTimeOut, bool isOverFlow = false) override;
	void Clear() override;
};

