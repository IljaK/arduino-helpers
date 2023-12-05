#include "BaseSerialMock.h"

BaseSerialMock::BaseSerialMock(Stream* serial)
{
	this->serialStream = serial;
}

BaseSerialMock::~BaseSerialMock()
{
}

void BaseSerialMock::HandleResponseReceived(bool isTimeOut, bool isOverFlow)
{
	isResponseReceived = true;
	isResponseTimeOut = isTimeOut;
	isResponseOverFlow = isOverFlow;
}

void BaseSerialMock::Clear() {
	isResponseReceived = false;
	isResponseTimeOut = false;
	isResponseOverFlow = false;
}
