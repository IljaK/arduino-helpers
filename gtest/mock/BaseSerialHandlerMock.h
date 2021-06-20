#pragma once
#include "SerialStream.h"
#include "serial/BaseSerialHandler.h"
#include "BaseSerialMock.h"

class BaseSerialHandlerMock: public BaseSerialHandler, public BaseSerialMock
{
public:
	BaseSerialHandlerMock(SerialStream *serial);
	~BaseSerialHandlerMock();

	void OnResponseReceived(bool IsTimeOut, bool isOverFlow = false) override;
};

