#include "Util.h"
#include <stdarg.h>
#include <cstring>
//#pragma warning(disable : 4996)
#if __arm__
    extern "C" char* sbrk(int incr);
#endif

Stream *outStream = NULL;

uint8_t reverseByte(uint8_t x)
{
	x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
	x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
	x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
	return x;
}

uint8_t getBitFromByte(uint8_t targetByte, uint8_t index)
{
	return (targetByte >> index) & 1;
}

uint8_t setBitOfByte(uint8_t target, bool value, uint8_t index)
{
    target ^= (-value ^ target) & (1UL << index);
    return target;
}

uint8_t getBitsValue(uint8_t target, uint8_t length, uint8_t start)
{
	uint8_t headerShift = (8u - start - length);
	target = target << headerShift;
	target = target >> (headerShift + start);
	return target;
}

uint16_t getBitsValue(uint16_t target, uint8_t length, uint8_t start)
{
	uint16_t headerShift = ((16u) - start - length);
	target = target << headerShift;
	target = target >> (headerShift + start);
	return target;
}

uint8_t setBitsValue(uint8_t target, uint8_t value, uint8_t length, uint8_t start)
{
	uint8_t headerShift = (8u - start - length);

	uint8_t mask = 0xFF << (start + headerShift);
	mask = mask >> headerShift;
	mask = ~mask;

	// Flush require bits
	target = target & mask;

	// Flush value header + shift to position
	value = (value << (headerShift + start)) >> headerShift;

	// Store bits
	return target | value;

}

uint16_t setBitsValue(uint16_t target, uint16_t value, uint8_t length, uint8_t start)
{
	uint16_t headerShift = ((16u) - start - length);

	uint16_t mask = 0xFFFF << (start + headerShift);
	mask = mask >> headerShift;
	mask = ~mask;

	// Flush require bits
	target = target & mask;

	// Flush value header + shift to position
	value = (value << (headerShift + start)) >> headerShift;

	// Store bits
	return target | value;
}


bool IsByteArraysEqual(uint8_t * byteArray1, int length1, uint8_t * byteArray2, int length2)
{
	if (length1 != length2 || length1 == 0) return false;
	for (int i = 0; i < length1; i++) {
		if (byteArray1[i] != byteArray2[i]) return false;
	}
	return true;
}

void CopyByteArray(uint8_t * source, uint8_t * destination, int size)
{
	if (size == 0) return;
	for (int i = 0; i < size; i++) {
		destination[i] = source[i];
	}
}

size_t SplitString(char *source, uint8_t separator, char **subStrArray, size_t arraySize, bool skipEmpty)
{
	char sep[2] = { (char)separator , 0 };
	return SplitString(source, sep, subStrArray, arraySize, skipEmpty);
}

size_t SplitString(char *source, char *separator, char **subStrArray, size_t arraySize, bool skipEmpty)
{
	if (arraySize == 0) return 0;

	bool inQuotation = false;
	size_t separatorLength = strlen(separator);
	char *pChr = source;

	subStrArray[0] = pChr;
	size_t argumentsAmount = 1;

	if (argumentsAmount >= arraySize) {
		return argumentsAmount;
	}

	while (pChr[0] != 0) {

		if (!(separator[0] != '\"' && separator[1] != 0)) {
			if (inQuotation) {
				if (pChr[0] == '\"') {
					inQuotation = false;
				}
				pChr++;
				continue;
			}
			else {
				if (pChr[0] == '\"') {
					inQuotation = true;
					pChr++;
					continue;
				}
			}
		}

		if (strncmp(pChr, separator, separatorLength) == 0) {
			char *nextArg = pChr + separatorLength;
			pChr[0] = 0;
			pChr = nextArg;

			if (skipEmpty) {
				if (subStrArray[argumentsAmount - 1][0] == 0) {
					argumentsAmount--;
				}
			}

			if (nextArg[0] == 0) {
				break;
			}
			else {
				subStrArray[argumentsAmount] = nextArg;
				argumentsAmount++;
			}

			if (argumentsAmount >= arraySize) {
				break;
			}
			continue;
		}

		pChr++;
	}
	return argumentsAmount;
}

void ShiftQuotations(char **subStrArray, size_t arraySize)
{
	for (size_t i = 0; i < arraySize; i++) {
		if (subStrArray[i] != 0) {
			subStrArray[i] = ShiftQuotations(subStrArray[i]);
		}
	}
}

char *ShiftQuotations(char *quatationString)
{
	size_t length = strlen(quatationString);
	if (quatationString[0] == '\"' && quatationString[length - 1] == '\"')
	{
		quatationString += 1;
		quatationString[length - 2] = 0;
	}
	return quatationString;
}

uint32_t remainRam () {
#if ARDUINO_TEST
	return 0;
#elif __arm__
    char top;
    return &top - reinterpret_cast<char*>(sbrk(0));
#elif ESP32
    return xPortGetFreeHeapSize();
#else
    extern int __heap_start, *__brkval;
    int v;
    return (size_t) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
#endif
}

/*
void outPrintf(const char *format, ...)
{
	if (digitalRead(DEBUG_ON_PIN) == LOW) return;

	char outMessage[96];

	va_list arglist;
	va_start(arglist, format);
	vsnprintf(outMessage, 96, format, arglist);
	va_end(arglist);

	char outPart[SERIAL_RX_BUFFER_SIZE];

	int printPartLength = 0;
	char *printBegin = outMessage;
	int remainLength = strlen(printBegin);

	do {
		if (remainLength >= SERIAL_RX_BUFFER_SIZE) printPartLength = SERIAL_RX_BUFFER_SIZE - 1;
		else printPartLength = remainLength;

		strncpy(outPart, printBegin, printPartLength);
		outPart[printPartLength] = 0;
		outWrite(outPart, printPartLength);

		printBegin += printPartLength;
		remainLength -= printPartLength;

	} while (remainLength > 0);

	outEnd();
}
*/

long readVCC() {
#if defined(ARDUINO_TEST)
	return 5000;
#elif defined(ESP32) || defined(ARDUINO_ARCH_SAMD)
	return 3300;
#else
	long result; // Read 1.1V reference against AVcc 
	ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
	delay(2); // Wait for Vref to settle 
	ADCSRA |= _BV(ADSC); // Convert 
	while (bit_is_set(ADCSRA,ADSC)); 
	result = ADCL; 
	result |= ADCH<<8; 
	result = 1126400L / result; // Back-calculate AVcc in mV
	return result; 
#endif
}

double readAnalogVoltage(uint8_t pin)
{
    double vcc = readVCC() / 1000.0;
    double analog_value = analogRead(pin);
	return ((analog_value * vcc) / ANALOG_UNITS_RESOLUTION_RANGE);
}
#if defined(ESP32)
uint8_t initPullupPin(uint8_t pin, uint8_t mode, voidFuncPtr isrFunc)
#else
uint8_t initPullupPin(uint8_t pin, PinMode mode, voidFuncPtr isrFunc)
#endif
{
    pinMode(pin, INPUT);
    uint8_t val = digitalRead(pin);
    pinMode(pin, mode);
    attachInterrupt(digitalPinToInterrupt(pin), isrFunc, CHANGE);
    return val;
}

inline uint8_t fromHexOctetToChar(uint8_t val, char a = 'A') {
    return (char)(val > 9 ? a + val - 10 : '0' + val);
}

size_t encodeToHex(uint8_t *inArray, size_t length, char *outBuffer, bool lowerCase)
{
    char a = 'A';
    if (lowerCase) {
        a = 'a';
    }
    for (size_t i = 0; i < length; i++) {
        uint8_t b = inArray[i];
        outBuffer[0] = fromHexOctetToChar((b >> 4) & 0x0f, a);
        outBuffer[1] = fromHexOctetToChar((b & 0x0f), a);
        outBuffer += 2;
    }
    size_t result = length * 2;
    // outBuffer is shifted by (length - 1) * 2
    outBuffer[0] = 0;
    return result;
}

inline uint8_t fromCharToHexOctet(uint8_t val) {
    if (val > '9') {
        val = (val - 'A') + 10;
    } else {
        val = (val - '0');
    }
    return val;
}

size_t decodeFromHex(char *inBuffer, uint8_t *outArray, size_t bytesLen)
{
    for (size_t i = 0; i < bytesLen; i++) {
        uint8_t n1 = inBuffer[i * 2];
        uint8_t n2 = inBuffer[i * 2 + 1];

        if (n1 == 0 || n2 == 0) {
            return i;
        }
        n1 = fromCharToHexOctet(n1);
        n2 = fromCharToHexOctet(n2);
        outArray[i] = (n1 << 4) | n2;
    }
    return bytesLen;
}


char *makeNewCopy(const char *target)
{
    if (target == NULL) {
        return NULL;
    }
    size_t len = strlen(target);
    char *result = (char*)malloc(len + 1);
    if (result == NULL) {
        // No more memory
        return NULL;
    }
    strncpy(result, target, len);
    result[len] = 0;
    return result;
}
extern bool makeSafeCopy(char *dest, const char *target, size_t copyLength)
{
    if (dest == NULL) return false;
    dest[0] = 0;
    if (target == NULL) return true;
    strncpy(dest, target, copyLength);
    dest[copyLength] = 0;
    return true;
}

/*
#ifndef ESP32
char * dtostrf(double number, signed char width, unsigned char prec, char *s) {
    bool negative = false;

    if (isnan(number)) {
        strcpy(s, "nan");
        return s;
    }
    if (isinf(number)) {
        strcpy(s, "inf");
        return s;
    }

    char* out = s;

    int fillme = width; // how many cells to fill for the integer part
    if (prec > 0) {
        fillme -= (prec+1);
    }

    // Handle negative numbers
    if (number < 0.0) {
        negative = true;
        fillme--;
        number = -number;
    }

    // Round correctly so that print(1.999, 2) prints as "2.00"
    // I optimized out most of the divisions
    double rounding = 2.0;
    for (uint8_t i = 0; i < prec; ++i)
        rounding *= 10.0;
    rounding = 1.0 / rounding;

    number += rounding;

    // Figure out how big our number really is
    double tenpow = 1.0;
    int digitcount = 1;
    while (number >= 10.0 * tenpow) {
        tenpow *= 10.0;
        digitcount++;
    }

    number /= tenpow;
    fillme -= digitcount;

    // Pad unused cells with spaces
    while (fillme-- > 0) {
        *out++ = ' ';
    }

    // Handle negative sign
    if (negative) *out++ = '-';

    // Print the digits, and if necessary, the decimal point
    digitcount += prec;
    int8_t digit = 0;
    while (digitcount-- > 0) {
        digit = (int8_t)number;
        if (digit > 9) digit = 9; // insurance
        *out++ = (char)('0' | digit);
        if ((digitcount == prec) && (prec > 0)) {
            *out++ = '.';
        }
        number -= digit;
        number *= 10.0;
    }

    // make sure the string is terminated
    *out = 0;
    return s;
}
#endif
*/


size_t PrintNumber(char *in, size_t inSize, unsigned long n, uint8_t base)
{
    if (inSize <= 1) {
        return 0;
    }
    char buf[8 * sizeof(n) + 1]; // Assumes 8-bit chars plus zero byte.
    char *str = &buf[sizeof(buf) - 1];

    *str = '\0';

    // prevent crash if called with base == 1
    if(base < 2) {
        base = 10;
    }

    do {
        char c = n % base;
        n /= base;

        *--str = c < 10 ? c + '0' : c + 'A' - 10;
    } while (n);

    size_t len = strlen(str);
    
    if (len >= inSize) {
        len = inSize - 1;
    }

    memcpy(in, str, len);
    in[len] = 0;
    return len;
}

size_t PrintNumber(char *in, size_t inSize, long n, uint8_t base)
{
    if (inSize <= 1) {
        return 0;
    }
    if (base == 10 && n < 0) {
        //t = print('-');
        in[0] = '-';
        in[1] = 0;
        n = -n;
    }
    return PrintNumber(in+1, inSize-1, static_cast<unsigned long>(n), base);
}