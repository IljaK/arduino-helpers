#pragma once
#include <Stream.h>
#include "serial/BaseSerialHandler.h"
#include "BaseSerialMock.h"

class BaseSerialHandlerMock: public BaseSerialHandler, public BaseSerialMock
{
public:
	BaseSerialHandlerMock(Stream *serial);
	~BaseSerialHandlerMock();

	void OnResponseReceived(bool isTimeOut, bool isOverFlow = false) override;
};

