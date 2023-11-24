#pragma once
#include "../common/Util.h"
#include "BaseSerialHandler.h"

class SerialCharResponseHandler: public BaseSerialHandler, public Print
{
private:
	char *separator;
	size_t separatorLength;
	size_t separatorMatchedLength = 0;

	bool AppendSymbolToBuffer(uint8_t symbol);
	bool IsSeparatorRemainMatch(int remainSeparatorLength, uint8_t symbol);
protected:
	void ResponseDetectedInternal(bool IsTimeOut, bool isOverFlow = false) override;
	virtual void ResetBuffer();
	virtual bool LoadSymbolFromBuffer(uint8_t symbol);
	size_t bufferLength = 0;
	char *buffer = NULL;

    size_t write(uint8_t symbol) override;
    size_t write(const uint8_t *buffer, size_t size) override;

public:
    const size_t bufferSize;

    int availableForWrite() override;
    
	SerialCharResponseHandler(const size_t bufferSize, const char *separator, Stream * serial);
	virtual ~SerialCharResponseHandler();

	void Loop() override;
	bool IsBusy() override;

	void FlushData() override;
    bool IsLimitReached();

    void OnResponseReceived(bool IsTimeOut, bool isOverFlow = false) override;
};

