#include "SerialCharResponseHandler.h"

SerialCharResponseHandler::SerialCharResponseHandler(const size_t bufferSize, const char *separator, Stream * serial):
    BaseSerialHandler(serial),
    bufferSize(bufferSize)
{
    buffer = (char *)malloc(bufferSize);
	buffer[0] = 0;
	this->separator = makeNewCopy(separator);
	separatorLength = separator == NULL ? 0 : strlen(separator);
}

SerialCharResponseHandler::~SerialCharResponseHandler()
{
    free(separator);
    separatorLength = 0;
    free(buffer);
}

void SerialCharResponseHandler::Loop()
{
	if (serial) {
        while (serial->available() > 0) {
            int val = serial->read();
            if (val <= 0) {
                return;
            }
            if (LoadSymbolFromBuffer(val)) {
                return;
            }
        }
        if (IsLimitReached()) {
            ResponseDetectedInternal(false, true);
        }
	}

	BaseSerialHandler::Loop();
}

bool SerialCharResponseHandler::LoadSymbolFromBuffer(uint8_t symbol)
{
	// sepparator symbol match
	if (separator[separatorMatchedLength] == symbol) {
		separatorMatchedLength++;

		if (separatorMatchedLength >= separatorLength) {
			separatorMatchedLength = 0;
			ResponseDetectedInternal(false, false);
			return true;
		}
		return false;
	}

	// Append skipped matched sepparator characters
	if (separatorMatchedLength > 0) {

		// Append characters to buffer + recheck on separator match
		for (int i = 0; i < (int)separatorMatchedLength; i++) {

			if (!AppendSymbolToBuffer(separator[i])) {
				ResponseDetectedInternal(false, true);
				return true;
			}

			// Сheck if las symbol of buff + remain separator chars + new symbol match separator
			if (IsSeparatorRemainMatch(separatorMatchedLength - i - 1, symbol)) {
				separatorMatchedLength = separatorMatchedLength - i;
				return false;
			}
		}
	}
	if (!AppendSymbolToBuffer(symbol)) {
		ResponseDetectedInternal(false, true);
		return true;
	}
	return false;
}


bool SerialCharResponseHandler::IsSeparatorRemainMatch(int remainSeparatorLength, uint8_t symbol)
{
	char temp[sizeof(separator) + 1];
	temp[0] = buffer[bufferLength - 1];
	for (int i = 0; i < remainSeparatorLength; i++) {
		temp[i + 1] = separator[i];
	}
	temp[remainSeparatorLength] = symbol;
	temp[remainSeparatorLength + 1] = 0;

	return strncmp(temp, separator, remainSeparatorLength + 1) == 0;
}

bool SerialCharResponseHandler::AppendSymbolToBuffer(uint8_t symbol)
{
	if (bufferLength + 1 >= bufferSize) {
		return false;
	}

	buffer[bufferLength] = symbol;
	bufferLength++;
	buffer[bufferLength] = 0;

	return true;
}

void SerialCharResponseHandler::ResponseDetectedInternal(bool IsTimeOut, bool isOverFlow) {
	BaseSerialHandler::ResponseDetectedInternal(IsTimeOut, isOverFlow);
	ResetBuffer();
}

bool SerialCharResponseHandler::IsBusy()
{
	return BaseSerialHandler::IsBusy();
}

void SerialCharResponseHandler::FlushData()
{
	ResetBuffer();
	BaseSerialHandler::FlushData();
}

void SerialCharResponseHandler::ResetBuffer() {
	separatorMatchedLength = 0;
	buffer[0] = 0;
	bufferLength = 0;
}

bool SerialCharResponseHandler::IsLimitReached()
{
	return bufferLength == bufferSize;
}

void SerialCharResponseHandler::OnResponseReceived(bool IsTimeOut, bool isOverFlow)
{
    if (responseCallback != NULL) {
        responseCallback((uint8_t *)buffer, bufferLength, IsTimeOut, isOverFlow);
    }
}

size_t SerialCharResponseHandler::write(uint8_t symbol) {
    if (serial == NULL) return 0;
    return serial->write(symbol);
}
size_t SerialCharResponseHandler::write(const uint8_t *buffer, size_t size) {
    if (buffer == NULL || serial == NULL) return 0;
    return serial->write((const uint8_t *)buffer, size);
}

int SerialCharResponseHandler::availableForWrite() { 
    if (serial == NULL) return 0;
    return serial->availableForWrite();
}
