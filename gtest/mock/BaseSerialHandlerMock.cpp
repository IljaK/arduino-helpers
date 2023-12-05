#include <gtest/gtest.h>
#include "BaseSerialHandlerMock.h"

BaseSerialHandlerMock::BaseSerialHandlerMock(Stream *serial):BaseSerialHandler(serial), BaseSerialMock(serial)
{
}

BaseSerialHandlerMock::~BaseSerialHandlerMock()
{
}

void BaseSerialHandlerMock::OnResponseReceived(bool isTimeOut, bool isOverFlow)
{
	BaseSerialMock::HandleResponseReceived(isTimeOut, isOverFlow);
	BaseSerialHandler::OnResponseReceived(isTimeOut, isOverFlow);
}

