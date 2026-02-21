#include "bigint.h"

BigIntException::BigIntException(const std::string& message) :
    m_message{message}
{

}

BigIntException::BigIntException(const ExceptionCode code) :
    m_code{code}
{

}

BigIntException::BigIntException(const std::string& message,
                                 const ExceptionCode code) :
    m_message{message}, m_code{code}
{

}

const char* BigIntException::what() const noexcept
{
    return m_message.c_str();
}

ExceptionCode BigIntException::getExceptionCode() const noexcept
{
    return m_code;
}

std::vector<std::complex<double>>getFft(const std::vector<std::complex<double>>& value,
                                        const bool invert)
{
    std::vector<std::complex<double>> result = value;
    size_t resultSize = (value.size());
    if(resultSize == 1) {
        return std::vector<std::complex<double>>(1, value[0]);
    }
    std::vector<std::complex<double>> notEvenValue(resultSize / 2);
    std::vector<std::complex<double>> evenValue(resultSize / 2);
    for(size_t i = 0, j = 0; i < resultSize; i += 2, ++j) {
        notEvenValue[j] = value[i];
        evenValue[j] = value[i + 1];
    }
    notEvenValue = getFft(notEvenValue, invert);
    evenValue = getFft(evenValue, invert);
    double angle = 2 * PI / resultSize * (invert ? -1 : 1);
    std::complex<double> root_1(1);
    std::complex<double> root_n(std::cos(angle), std::sin(angle));
    for(size_t i = 0; i < resultSize / 2; ++i) {
        result[i] = notEvenValue[i] + root_1 * evenValue[i];
        result[i + resultSize / 2] = notEvenValue[i] - root_1 * evenValue[i];
        if(invert) {
            result[i] /= 2;
            result[i + resultSize / 2] /= 2;
        }
        root_1 *= root_n;
    }
    return result;
}

int charToInt(const uint8_t value)
{
    if(value >= '0' && value <= '9') {
        return value - '0';
    }
    if(value >= 'a' && value <= 'f') {
        return value - 87;
    }
    return value - 55;
}

std::string byteToBinaryString(const uint8_t byte)
{
    std::string result;
    uint8_t _byte = byte;
    for(uint8_t i = 0; i < 8; i++) {
        result.insert(result.begin(), '0' + (_byte & 1));
        _byte = _byte >> 1;
    }
    return result;
}

uint8_t dividedBy_256(std::string& value)
{
    int remainder{0};
    std::string result;
    for(char digit : value) {
        int current = remainder * 10 + (digit - '0');
        int quotient = current / 256;
        remainder = current % 256;
        if(!result.empty() || quotient != 0) {
            result.push_back('0' + quotient);
        }
    }
    value = result.empty() ? "0" : result;
    return static_cast<uint8_t>(remainder & 0xff);
}

std::vector<uint8_t> hexadecimalStringToBytesVector(const std::string& value)
{
    std::vector<uint8_t> result;
    if(value == "0") {
        result.push_back(0);
        return result;
    }
    uint8_t byte{0};
    int bitCount{0};
    for(auto it = value.rbegin(); it != value.rend(); it++) {
        byte |= (charToInt(*it) << bitCount);
        bitCount += 4;
        if(bitCount >= 8) {
            result.push_back(byte);
            bitCount -= 8;
            byte = (charToInt(*it) >> (4 - bitCount))&  ((1 << bitCount) - 1);
        }
    }
    if(bitCount > 0) {
        result.push_back(byte);
    }
    std::reverse(result.begin(), result.end());
    return result;
}

std::vector<uint8_t> decimalStringToBytesVector(const std::string& value)
{
    std::vector<uint8_t> result;
    if(value == "0") {
        result.push_back(0);
        return result;
    }
    std::string _value = value;
    while (_value != "0") {
        uint8_t byte = dividedBy_256(_value);
        result.push_back(byte);
    }
    std::reverse(result.begin(), result.end());
    return result;
}

std::vector<uint8_t> octalStringToBytesVector(const std::string& value)
{
    std::vector<uint8_t> result;
    if(value == "0") {
        result.push_back(0);
        return result;
    }
    uint8_t byte{0};
    int bitCount{0};
    for(auto it = value.rbegin(); it != value.rend(); it++) {
        byte |= ((*it - '0') << bitCount);
        bitCount += 3;
        if(bitCount >= 8) {
            result.push_back(byte);
            bitCount -= 8;
            byte = ((*it - '0') >> (3 - bitCount))&  ((1 << bitCount) - 1);
        }
    }
    if(bitCount > 0) {
        result.push_back(byte);
    }
    std::reverse(result.begin(), result.end());
    return result;
}

std::vector<uint8_t> binaryStringToBytesVector(const std::string& value)
{
    std::vector<uint8_t> result((value.length() + 7) / 8, 0);
    size_t i{0};
    for(auto it = value.rbegin(); it != value.rend(); it++) {
        size_t byteIndex = i / 8;
        size_t bitIndex = i % 8;
        if(*it == '1') {
            result[byteIndex] |= (1 << bitIndex);
        }
        i++;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

std::string bytesVectorToHexadecimalString(const std::vector<uint8_t>& value, const bool upper)
{
    std::string result;
    result.reserve(value.size() * 2);
    static constexpr char hexUpper[] = "012345678ABCDEF";
    static constexpr char hexLower[] = "0123456789abcdef";
    for(uint8_t byte : value) {
        if(upper) {
            result += hexUpper[byte >> 4];
            result += hexUpper[byte&  0xf];
        } else {
            result += hexLower[byte >> 4];
            result += hexLower[byte&  0xf];
        }
    }
    return result;
}

std::string bytesVectorToDecimalString(const std::vector<uint8_t>& value)
{
    std::string result{"0"};
    for(uint8_t byte : value) {
        int carry{0};
        for(long long i = static_cast<long long>(result.length() - 1); i >= 0; --i) {
            int digit = (result[i] -'0') * 256 + carry;
            result[i] = (digit % 10) + '0';
            carry = digit / 10;
        }
        while(carry > 0) {
            result.insert(result.begin(), (carry % 10) + '0');
            carry /= 10;
        }
        carry = byte;
        for(long long i = static_cast<long long>(result.length() - 1); i >= 0 && carry > 0; --i) {
            int digit = (result[i] - '0') + carry;
            result[i] = (digit % 10) + '0';
            carry = digit / 10;
        }
        while(carry > 0) {
            result.insert(result.begin(), (carry % 10) + '0');
            carry /= 10;
        }
    }
    return result;
}

std::string bytesVectorToOctalString(const std::vector<uint8_t>& value)
{
    std::string result{"0"};
    for(uint8_t byte : value) {
        int carry{0};
        for(long long i = static_cast<long long>(result.length() - 1); i >= 0; --i) {
            int digit = (result[i] -'0') * 256 + carry;
            result[i] = (digit % 8) + '0';
            carry = digit / 8;
        }
        while(carry > 0) {
            result.insert(result.begin(), (carry % 8) + '0');
            carry /= 8;
        }
        carry = byte;
        for(long long i = static_cast<long long>(result.length() - 1); i >= 0 && carry > 0; --i) {
            int digit = (result[i] - '0') + carry;
            result[i] = (digit % 8) + '0';
            carry = digit / 8;
        }
        while(carry > 0) {
            result.insert(result.begin(), (carry % 8) + '0');
            carry /= 8;
        }
    }
    return result;
}

std::string bytesVectorToBinaryString(const std::vector<uint8_t>& value)
{
    std::string result;
    for(uint8_t byte : value) {
        result += byteToBinaryString(byte);
    }
    while(result[0] == '0' && result.length() > 1) {
        result.erase(result.begin());
    }
    return result;
}

std::vector<uint8_t> bytesVectorToTwosComplement(const std::vector<uint8_t>& value) {
    std::vector<uint8_t> result = value;
    for (uint8_t& byte : result) {
        byte = ~byte;
    }
    for (long long i = static_cast<long long>(result.size() - 1); i >= 0; --i) {
        if(++result[i] != 0) {
            break;
        }
    }
    return result;
}

BigInt::BigInt()
{
    this->m_bytes.push_back(0);
}

BigInt::BigInt(const long long value) : BigInt(std::to_string(value))
{

}

BigInt::BigInt(const int value) : BigInt(std::to_string(value))
{

}

BigInt::BigInt(const size_t value) : BigInt(std::to_string(value))
{

}

BigInt::BigInt(const std::string& value, const Radix radix) : BigInt()
{
    if(value[0] == '-') {
        m_sign = true;
    }
    std::string _value = value;
    if(_value[0] == '+' || _value[0] == '-') {
        _value.erase(_value.begin());
    }
    switch(radix) {
    case Radix::Hex:
        m_bytes = hexadecimalStringToBytesVector(_value);
        break;
    case Radix::Dec:
        m_bytes = decimalStringToBytesVector(_value);
        break;
    case Radix::Oct:
        m_bytes = octalStringToBytesVector(_value);
        break;
    case Radix::Bin:
        m_bytes = binaryStringToBytesVector(_value);
        break;
    }
}

BigInt::BigInt(const char *strValue, const Radix radix) : BigInt(std::string(strValue), radix)
{

}

BigInt::BigInt(const BigInt& other) :
    m_bytes(other.m_bytes),
    m_sign(other.m_sign)
{

}

#if __cplusplus >= 201103L
BigInt::BigInt(BigInt&& other) noexcept :
    m_bytes(std::move(other.m_bytes)),
    m_sign(other.m_sign)
{
    other.m_sign = false;
}
#endif

BigInt::~BigInt()
{

}

BigInt& BigInt::operator = (const BigInt& other)
{
    if(this !=& other) {
        this->m_bytes = other.m_bytes;
        this->m_sign = other.m_sign;
    }
    return *this;
}

#if __cplusplus >= 201103L
BigInt& BigInt::operator = (BigInt&& other) noexcept
{
    if(this !=& other) {
        this->m_bytes = std::move(other.m_bytes);
        this->m_sign = other.m_sign;
        other.m_sign = false;
    }
    return *this;
}
#endif

BigInt& BigInt::operator = (const long long value)
{
    *this = BigInt(value);
    return *this;
}

size_t BigInt::length() const
{
    return this->m_bytes.size();
}

bool BigInt::isEven() const
{
    return ~this->m_bytes[this->length() - 1] & 1;
}

size_t BigInt::numBits()
{
    if(*this == 0) {
        return 0;
    }
    BigInt _value(*this);
    size_t result = 0;
    while(_value != 0) {
        result++;
        _value = _value.divBase(2);
    }
    return result;
}

std::string BigInt::toString(const Radix radix, const bool upper) const
{
    std::string result;
    switch(radix) {
    case Radix::Hex:
        result = bytesVectorToHexadecimalString(m_bytes, upper);
        break;
    case Radix::Dec:
        result = bytesVectorToDecimalString(m_bytes);
        break;
    case Radix::Oct:
        result = bytesVectorToOctalString(m_bytes);
        break;
    case Radix::Bin:
        result = bytesVectorToBinaryString(m_bytes);
        break;
    }
    if(this->m_sign) {
        result.insert(result.begin(), '-');
    }
    return result;
}

std::string BigInt::toHex(const bool upper)
{
    return this->toString(Radix::Hex, upper);
}

std::string BigInt::toDec()
{
    return this->toString();
}

std::string BigInt::toOct()
{
    return this->toString(Radix::Oct);
}

std::string BigInt::toBin()
{
    return this->toString(Radix::Bin);
}

int BigInt::toInt() const
{
    return std::stoi(this->toString().c_str());
}

long BigInt::toLong() const
{
    return std::stol(this->toString().c_str());
}

long long BigInt::toLongLong() const
{
    return std::stoll(this->toString().c_str());
}

unsigned int BigInt::toUInt() const
{
    return static_cast<unsigned int>(std::stoul(this->toString().c_str()));
}

unsigned long BigInt::toULong() const
{
    return std::stoul(this->toString().c_str());
}

unsigned long long BigInt::toULongLong() const
{
    return std::stoull(this->toString().c_str());
}

void BigInt::setMulMethod(MulMethod mulMethod)
{
    this->m_mulMethod = mulMethod;
}

BigInt BigInt::shlBitsBase(const size_t shift) const
{
    if(shift == 0) {
        return *this;
    }
    BigInt result(*this);
    size_t byteShift = shift / 8;
    size_t bitShift = shift % 8;
    if(byteShift > 0) {
        result.m_bytes.resize(result.length() + byteShift, 0);
    }
    if(bitShift > 0) {
        uint8_t carry{0};
        for(size_t i = result.length(); i-- > 0;) {
            uint8_t current = (result.m_bytes[i] << bitShift) | carry;
            carry = result.m_bytes[i] >> (8 - bitShift);
            result.m_bytes[i] = current;
        }
        if(carry > 0) {
            result.m_bytes.insert(result.m_bytes.begin(), carry);
        }
    }
    return result;
}

BigInt BigInt::shrBitsBase(const size_t shift) const
{
    if(shift == 0) {
        return *this;
    }
    BigInt result(*this);
    size_t byteShift = shift / 8;
    size_t bitShift = shift % 8;
    if(byteShift > 0) {
        result.m_bytes.erase(result.m_bytes.end() - byteShift, result.m_bytes.end());
    }
    if(bitShift > 0) {
        uint8_t carry = 0;
        for(size_t i = 0; i < result.length(); i++) {
            uint8_t current = (result.m_bytes[i] >> bitShift) | carry;
            carry = result.m_bytes[i] << (8 - bitShift);
            result.m_bytes[i] = current;
        }
    }
    result.normalise();
    return result;
}

BigInt BigInt::shlBytesBase(const size_t shift) const
{
    if(shift == 0) {
        return *this;
    }
    BigInt result(*this);
    result.m_bytes.resize(result.length() + shift, 0);
    return result;
}

BigInt BigInt::shrBytesBase(const size_t shift) const
{
    if(shift == 0) {
        return *this;
    }
    if(shift >= this->length()) {
        return 0;
    }
    BigInt result(*this);
    result.m_bytes.resize(result.length() - shift, 0);
    return result;
}

BigInt BigInt::orBase(const BigInt& value) const
{
    BigInt result;
    std::vector<uint8_t> rightDigits = this->m_bytes;
    std::vector<uint8_t> leftDigits = value.m_bytes;
    if(abs(*this) <= LLONG_MAX && abs(value) <= LLONG_MAX) {
        result = this->toLongLong() | value.toLongLong();
    } else {
        result.m_sign = this->m_sign | value.m_sign;
        if(rightDigits.size() > leftDigits.size()) {
            leftDigits.insert(leftDigits.begin(), rightDigits.size() - leftDigits.size(), 0);
        } else if(rightDigits.size() < leftDigits.size()) {
            rightDigits.insert(rightDigits.begin(), leftDigits.size() - rightDigits.size(), 0);
        }
        if(this->m_sign) {
            rightDigits = bytesVectorToTwosComplement(rightDigits);
        }
        if(value.m_sign) {
            leftDigits = bytesVectorToTwosComplement(leftDigits);
        }
    }
    for(size_t i = 0; i < rightDigits.size(); i++) {
        result.m_bytes.push_back(rightDigits[i] | leftDigits[i]);
    }
    result.normalise();
    if(result.m_sign) {
        result.m_bytes = bytesVectorToTwosComplement(result.m_bytes);
    }
    return result;
}

BigInt BigInt::andBase(const BigInt& value) const
{
    BigInt result;
    std::vector<uint8_t> rightDigits = this->m_bytes;
    std::vector<uint8_t> leftDigits = value.m_bytes;
    if(abs(*this) <= LLONG_MAX && abs(value) <= LLONG_MAX) {
        result = this->toLongLong()&  value.toLongLong();
    } else {
        result.m_sign = this->m_sign&  value.m_sign;
        if(rightDigits.size() > leftDigits.size()) {
            leftDigits.insert(leftDigits.begin(), rightDigits.size() - leftDigits.size(), 0);
        } else if(rightDigits.size() < leftDigits.size()) {
            rightDigits.insert(rightDigits.begin(), leftDigits.size() - rightDigits.size(), 0);
        }
        if(this->m_sign) {
            rightDigits = bytesVectorToTwosComplement(rightDigits);
        }
        if(value.m_sign) {
            leftDigits = bytesVectorToTwosComplement(leftDigits);
        }
    }
    for(size_t i = 0; i < rightDigits.size(); i++) {
        result.m_bytes.push_back(rightDigits[i]&  leftDigits[i]);
    }
    result.normalise();
    if(result.m_sign) {
        result.m_bytes = bytesVectorToTwosComplement(result.m_bytes);
    }
    return result;
}

BigInt BigInt::xorBase(const BigInt& value) const
{
    BigInt result;
    std::vector<uint8_t> rightDigits = this->m_bytes;
    std::vector<uint8_t> leftDigits = value.m_bytes;
    if(abs(*this) <= LLONG_MAX && abs(value) <= LLONG_MAX) {
        result = this->toLongLong() ^ value.toLongLong();
    } else {
        result.m_sign = this->m_sign ^ value.m_sign;
        if(rightDigits.size() > leftDigits.size()) {
            leftDigits.insert(leftDigits.begin(), rightDigits.size() - leftDigits.size(), 0);
        } else if(rightDigits.size() < leftDigits.size()) {
            rightDigits.insert(rightDigits.begin(), leftDigits.size() - rightDigits.size(), 0);
        }
        if(this->m_sign) {
            rightDigits = bytesVectorToTwosComplement(rightDigits);
        }
        if(value.m_sign) {
            leftDigits = bytesVectorToTwosComplement(leftDigits);
        }
    }
    for(size_t i = 0; i < rightDigits.size(); i++) {
        result.m_bytes.push_back(rightDigits[i] ^ leftDigits[i]);
    }
    result.normalise();
    if(result.m_sign) {
        result.m_bytes = bytesVectorToTwosComplement(result.m_bytes);
    }
    return result;
}

BigInt BigInt::notBase() const
{
    if(abs(*this) <= LLONG_MAX) {
        return ~this->toLongLong();
    } else {
        if(this->m_sign) {
            return abs(*this).subBase(1);
        } else {
            return -(this->addBase(1));
        }
    }
}

BigInt BigInt::addBase(const BigInt& value) const
{
    if(*this == 0 && value == 0) {
        return 0;
    }
    if(*this == 0) {
        return value;
    }
    if(value == 0) {
        return *this;
    }
    size_t resultSize = std::max(this->length(), value.length()) + 1;

    if(resultSize <= LLONG_SIZE) {
        return this->toLongLong() + value.toLongLong();
    }
    BigInt result;
    if(this->m_sign == value.m_sign) {
        uint8_t rightByte, leftByte, carry{0};
        std::vector<uint8_t> rightBytes = this->m_bytes;
        std::vector<uint8_t> leftBytes = value.m_bytes;
        result.m_bytes.resize(resultSize);
        result.m_sign = this->m_sign;
        for(size_t i = 0; i < resultSize; i++) {
            rightBytes.size() >= i + 1 ? rightByte = rightBytes[rightBytes.size() - (i + 1)] : rightByte = 0;
            leftBytes.size() >= i + 1 ? leftByte = leftBytes[leftBytes.size() - (i + 1)] : leftByte = 0;
            result.m_bytes[resultSize - (i + 1)] = (rightByte + leftByte + carry) % 256;
            carry = (rightByte + leftByte + carry) / 256;
        }
    } else {
        result = this->m_sign ? value.subBase(-*this) : this->subBase(-value);
    }
    result.normalise();
    return result;
}

BigInt BigInt::subBase(const BigInt& value) const
{
    if(*this == value) {
        return 0;
    }
    if(*this == 0) {
        return -value;
    }
    if(value == 0) {
        return *this;
    }
    size_t resultSize = std::max(this->length(), value.length()) + 1;
    if(resultSize <= LLONG_SIZE) {
        return this->toLongLong() - value.toLongLong();
    }
    BigInt result;
    if(this->m_sign == value.m_sign) {
        int rightByte, leftByte, borrow = 0;
        std::vector<uint8_t> rightBytes;
        std::vector<uint8_t> leftBytes;
        result.m_bytes.resize(resultSize);
        if(!this->m_sign && !value.m_sign)  {
            if(*this < value) {
                result.m_sign = true;
                rightBytes = this->m_bytes;
                leftBytes = value.m_bytes;
            } else {
                result.m_sign = false;
                rightBytes = value.m_bytes;
                leftBytes = this->m_bytes;
            }
        }
        if(this->m_sign && value.m_sign) {
            if(*this < value) {
                result.m_sign = true;
                rightBytes = this->m_bytes;
                leftBytes = value.m_bytes;
            } else {
                result.m_sign = false;
                rightBytes = value.m_bytes;
                leftBytes = this->m_bytes;
            }
        }
        for(size_t i = 0; i < resultSize - 1; i++) {
            leftByte = leftBytes[leftBytes.size() - (i + 1)] - borrow;
            rightBytes.size() >= i + 1 ? rightByte = rightBytes[rightBytes.size() - (i + 1)] : rightByte = 0;
            leftByte < rightByte ? borrow = 1 : borrow = 0;
            result.m_bytes[resultSize - (i + 1)] = (leftByte + 256 * borrow) - rightByte;
        }
    } else {
        result = this->addBase(-value);
    }
    result.normalise();
    return result;
}

BigInt BigInt::mulNative(const BigInt& value) const
{
    BigInt result;
    result.m_bytes.resize(this->length() + value.length());
    result.m_sign = this->m_sign ^ value.m_sign;
    for(long long i = value.length() - 1; i >= 0; --i) {
        uint32_t carry{0};
        for(long long j = this->length() - 1; j >= 0; --j) {
            uint32_t byte = result.m_bytes[i + j + 1] + this->m_bytes[j] * value.m_bytes[i] + carry;
            carry = byte / 256;
            result.m_bytes[i + j + 1] = byte % 256;
        }
        result.m_bytes[i] += carry;
    }
    result.normalise();
    return result;
}

BigInt BigInt::mulKaratsuba(const BigInt& value) const
{

    BigInt leftValue = *this;
    BigInt rightValue = value;
    if(leftValue.length() == 1 || rightValue == 1) {
            return leftValue.mulNative(rightValue);
    }
    size_t maxSize = std::max(leftValue.length(), rightValue.length());
    maxSize += maxSize % 2;
    size_t halfSize = maxSize / 2;
    BigInt leftValueLow;
    leftValue.length() > halfSize ? leftValueLow.m_bytes.assign(leftValue.m_bytes.end() - halfSize, leftValue.m_bytes.end()) :
                                  leftValueLow.m_bytes.assign(leftValue.m_bytes.begin(), leftValue.m_bytes.end());
     BigInt rightValueLow;
     rightValue.length() > halfSize ? rightValueLow.m_bytes.assign(rightValue.m_bytes.end() - halfSize, rightValue.m_bytes.end()) :
                                    rightValueLow.m_bytes.assign(rightValue.m_bytes.begin(), rightValue.m_bytes.end());
     BigInt leftValueHigh = leftValue.shrBytesBase(halfSize);
     BigInt rightValueHigh = rightValue.shrBytesBase(halfSize);
     BigInt p_1 = leftValueHigh.mulBase(rightValueHigh);
     BigInt p_2 = leftValueLow.mulBase(rightValueLow);
     BigInt p_3 = leftValueHigh.addBase(leftValueLow).mulBase(rightValueHigh.addBase(rightValueLow));
     BigInt result = p_1.shlBytesBase(maxSize).addBase((p_3.subBase(p_2).subBase(p_1).shlBytesBase(halfSize)).addBase(p_2));
     result.m_sign = this->m_sign ^ value.m_sign;
     result.normalise();
     return result;
}

BigInt BigInt::mulFft(const BigInt& value) const
{
    BigInt result;
    result.m_bytes.clear();
    result.m_sign = this->m_sign ^ value.m_sign;
    std::vector<std::complex<double>> complexValue_1(this->m_bytes.begin(), this->m_bytes.end()),
            complexValue_2(value.m_bytes.begin(), value.m_bytes.end());
    size_t resultSize = 1;
    std::reverse (complexValue_1.begin(), complexValue_1.end());
    std::reverse (complexValue_2.begin(), complexValue_2.end());
    while(resultSize < std::max(complexValue_1.size(), complexValue_2.size())) {
        resultSize <<= 1;
    }
    resultSize <<= 1;
    complexValue_1.resize(resultSize);
    complexValue_2.resize(resultSize);
    complexValue_1 = getFft(complexValue_1, false);
    complexValue_2 = getFft(complexValue_2, false);
    for(size_t i = 0; i < resultSize; i++) {
        complexValue_1[i] *= complexValue_2[i];
    }
    complexValue_1 = getFft(complexValue_1, true);
    int carry = 0;
    unsigned int resultDigit;
    for(size_t i = 0; i < resultSize; ++i) {
        resultDigit = static_cast<unsigned int>(complexValue_1[i].real() + 0.5) + carry;
        carry = resultDigit / 256;
        resultDigit = resultDigit % 256;
        result.m_bytes.push_back(resultDigit);
    }
    result.m_bytes.push_back(carry);
    std::reverse(result.m_bytes.begin(), result.m_bytes.end());
    result.normalise();
    return result;
}

BigInt BigInt::mulBase(const BigInt& value) const
{
    if(*this == 0 || value == 0) {
        return 0;
    }
    if(*this == 1) {
        return value;
    }
    if(value == 1) {
        return *this;
    }
    if(this->length() + value.length() + 1 <= LLONG_SIZE) {
        return this->toLongLong() * value.toLongLong();
    }
    if(this->length() + value.length() + 1 > 50000) {
        if(m_mulMethod == MulMethod::Karatsuba) {
            return this->mulKaratsuba(value);
        } else if(m_mulMethod == MulMethod::FastFourierTransform) {
            return this->mulFft(value);
        }
    }
    return this->mulNative(value);
}

BigInt BigInt::divNative(const BigInt& value) const
{
    BigInt divident(abs(*this));
    BigInt divider(abs(value));
    BigInt result;
    BigInt subResult;
    size_t index{0};
    while(subResult < divider && index < divident.length()) {
        subResult.m_bytes.push_back(divident.m_bytes[index++]);
    }
    subResult.normalise();
    do {
        int count = 0;
        if(divider > LONG_LONG_MAX) {
            BigInt mod = subResult;
            while(mod >= divider) {
                mod = mod.subBase(divider);
                count++;
            }
            subResult = mod;
        } else {
            long long mod = subResult.toLongLong();
            count = mod / divider.toLongLong();
            subResult = mod % divider.toLongLong();
        }
        result.m_bytes.push_back(count);
        if(index <= divident.length()) {
            subResult.m_bytes.push_back(divident.m_bytes[index++]);
        }
    } while(index <= divident.length());
    result.m_sign = this->m_sign ^ value.m_sign;
    result.normalise();
    return result;
}

BigInt BigInt::divNewtonRaphson(const BigInt& value) const
{
    BigInt divident(*this);
    BigInt divider(value);
    size_t shift = divident.length() + divider.length();
    BigInt powBase_256{2};
    powBase_256 = powBase_256.shlBytesBase(shift);
    BigInt current{1};
    current = current.shlBytesBase(divident.length());
    BigInt last;
    while(last != current) {
        last = current;
        current = (current.mulBase(powBase_256.subBase(current.mulBase(divider)))).shrBytesBase(shift);
    }
    BigInt result = divident.mulBase(current);
    result = result.shrBytesBase(shift);
    BigInt remainder = divident.subBase(result.mulBase(divider));
    if(remainder >= divider) {
        result = result.addBase(1);
    }
    result.m_sign = this->m_sign ^ value.m_sign;
    result.normalise();
    return result;
}

BigInt BigInt::divBase(const BigInt& value) const
{
    if(value == 0) {
        throw BigIntException("Divizion by zero", ExceptionCode::DivisionByZero);
    }
    if(value > *this) {
        return 0;
    }
    if(value == *this) {
        return 1;
    }
    if(value == -(*this)) {
        return -1;
    }
    if(value == 1) {
        return *this;
    }
    if(value == -1) {
        return -(*this);
    }
    size_t numberOfZero = 0;
    while(value.m_bytes[numberOfZero] == 0) {
        numberOfZero++;
    }
    if(abs(*this) <= LLONG_MAX && abs(value) <= LLONG_MAX) {
        return this->shrBytesBase(numberOfZero).toLongLong() / value.shrBytesBase(numberOfZero).toLongLong();
    } else if(this->length() < 50000 && value.length() < 500000) {
        return this->shrBytesBase(numberOfZero).divNative(value.shrBytesBase(numberOfZero));
    } else {
        return this->shrBytesBase(numberOfZero).divNewtonRaphson(value.shrBytesBase(numberOfZero));
    }
}

BigInt BigInt::remBase(const BigInt& value) const
{
    if(value > *this) {
        return *this;
    }
    if(value == *this) {
        return 0;
    }
    BigInt divident(*this);
    BigInt divider(value);
    BigInt quotient = divident.divBase(divider);
    return divident.subBase(divider.mulBase(quotient));
}

BigInt BigInt::powBase(long long exp) const
{
    if(exp < 0) {
        return 0;
    }
    if(exp == 0) {
          return 1;
    }
    if(!exp) {
        return *this;
    }
    if(this->length() * exp + exp <= LLONG_SIZE) {
        return static_cast<long long>(std::pow(this->toLongLong(), exp));
    }
    BigInt result;
    if(exp&  1) {
        result = this->powBase(exp - 1);
        return result.mulBase(*this);
    } else {
        result = this->powBase(exp / 2);
        return result.mulBase(result);
    }
}

BigInt BigInt::powBase(const BigInt& exp) const
{
    if(exp.m_sign) {
        return 0;
    }
    if(exp == 0) {
        return 1;
    }
    if(exp == 1) {
        return *this;
    }
    if(BigInt(this->length()).mulBase(exp).addBase(exp) <= LLONG_SIZE) {
        return static_cast<long long>(std::pow(this->toLongLong(), exp.toLongLong()));
    }
    BigInt result;
    if(exp.isEven()) {
        result = this->powBase(exp.subBase(1));
        return result.mulBase(*this);
    } else {
        result = this->powBase(exp.divBase(2));
        return result.mulBase(result);
    }
}

BigInt BigInt::powModBase(const long long exp, const BigInt& mod) const
{
    if(exp < 0) {
        return 0;
    }
    if(exp == 0) {
        return 1;
    }
    if(exp == 1) {
        return *this;
    }
    if(this->length() * exp + exp <= LLONG_SIZE && abs(mod) <= LLONG_MAX) {
        return static_cast<long long>(std::pow(this->toLongLong(), exp)) % mod.toLongLong();
    }
    BigInt result;
    if(exp&  1) {
        result = this->powModBase(exp - 1, mod);
        return result.mulBase(*this).remBase(mod);
    } else {
        result = this->powModBase(exp / 2, mod);
        return result.mulBase(result).remBase(mod);
    }
}

BigInt BigInt::powModBase(const BigInt& exp, const BigInt& mod) const
{
    if(exp < 0) {
        return 0;
    }
    if(exp == 0) {
        return 1;
    }
    if(exp == 1) {
        return *this;
    }
    if(BigInt(this->length()).mulBase(exp).addBase(exp) <= LLONG_SIZE && abs(mod) <= LLONG_MAX) {
        return static_cast<long long>(std::pow(this->toLongLong(), exp.toLongLong())) % mod.toLongLong();
    }
    BigInt result;
    if(exp.isEven()) {
        result = this->powModBase(exp.subBase(1), mod);
        return result.mulBase(*this).remBase(mod);
    } else {
        result = this->powModBase(exp.divBase(2), mod);
        return result.mulBase(result).remBase(mod);
    }
}

BigInt BigInt::rootBase(const long long exp) const
{
    if(exp == 0) {
        throw BigIntException("Divizion by zero",
                               ExceptionCode::DivisionByZero);
    }
    if(this->m_sign && (exp&  1) == 0) {
        throw BigIntException("Value must be positive",
                               ExceptionCode::InvalidRootDegree);
    }
    if(exp < 0) {
        return 0;
    }
    if(exp == 1) {
        return *this;
    }
    if(abs(*this) >= exp && this->m_sign) {
        return -1;
    }
    if(abs(*this) >= exp && !this->m_sign) {
        return 1;
    }
    size_t resultSize = (this->length() + 1) / 2;
    size_t index = 0;
    BigInt result;
    result.m_bytes.resize(resultSize);
    while(index < resultSize) {
        result.m_bytes[index] = 9;
        while(result.powBase(exp) > abs(*this) && result.m_bytes[index] > 0) {
            result.m_bytes[index]--;
        }
        index++;
    }
    result.normalise();
    result.m_sign = this->m_sign;
    return result;
}

BigInt BigInt::rootBase(const BigInt& exp) const
{
    if(exp == 0) {
        throw BigIntException("Divizion by zero",
                               ExceptionCode::DivisionByZero);
    }
    if(this->m_sign && (exp.isEven()) == 0) {
        throw BigIntException("Value must be positive",
                               ExceptionCode::InvalidRootDegree);
    }
    if(exp < 0) {
        return 0;
    }
    if(exp == 1) {
        return *this;
    }
    if(abs(*this) >= exp && this->m_sign) {
        return -1;
    }
    if(abs(*this) >= exp && !this->m_sign) {
        return 1;
    }
    size_t resultSize = (this->length() + 1) / 2;
    size_t index = 0;
    BigInt result;
    result.m_bytes.resize(resultSize);
    while(index < resultSize) {
        result.m_bytes[index] = 9;

        while(result.powBase(exp) > abs(*this) && result.m_bytes[index] > 0) {
            result.m_bytes[index]--;
        }
        index++;
    }
    result.normalise();
    result.m_sign = this->m_sign;
    return result;
}

void BigInt::normalise()
{
    auto nonZero = std::find_if(this->m_bytes.begin(), this->m_bytes.end(),
                                [](uint8_t x) {return x != 0;});
    this->m_bytes.erase(this->m_bytes.begin(), nonZero);
}

BigInt BigInt::operator -- ()
{
    *this = this->subBase(1);
    return *this;
}

BigInt BigInt::operator ++ ()
{
    *this = this->addBase(1);
    return *this;
}

BigInt BigInt::operator -- (int)
{
    BigInt result(*this);
    *this = this->subBase(1);
    return result;
}

BigInt BigInt::operator ++ (int)
{
    BigInt result(*this);
    *this = this->addBase(1);
    return result;
}

BigInt BigInt::operator - () const&
{
    BigInt result(*this);
    result.m_sign = !this->m_sign;
    return result;
}

BigInt BigInt::operator + () const&
{
    return *this;
}

bool BigInt::operator == (const BigInt& other) const
{
    return (this->m_bytes == other.m_bytes) && (this->m_sign == other.m_sign);
}

bool BigInt::operator != (const BigInt& other) const
{
    return !(*this == other);
}

bool BigInt::operator < (const BigInt& other) const
{
    size_t digitsLeftSize = this->length();
    size_t digitsRightSize = other.length();
    if(this->m_sign == other.m_sign) {
        if(digitsLeftSize != digitsRightSize) {
            return (digitsLeftSize < digitsRightSize) ^ this->m_sign;
        }
        return (this->m_bytes < other.m_bytes) ^ this->m_sign;
    }
    return this->m_sign;
}

bool BigInt::operator > (const BigInt& other) const
{
    return !(*this < other || *this == other);
}

bool BigInt::operator >= (const BigInt& other) const
{
    return *this > other || *this == other;
}

bool BigInt::operator <= (const BigInt& other) const
{
    return *this < other || *this == other;
}

bool BigInt::operator == (const long long other) const
{
    return (this->m_bytes == BigInt(other).m_bytes) && (this->m_sign == BigInt(other).m_sign);
}

bool BigInt::operator != (const long long other) const
{
    return !(*this == BigInt(other));
}

bool BigInt::operator < (const long long other) const
{
    size_t digitsLeftSize = this->length();
    size_t digitsRightSize = BigInt(other).length();
    if(this->m_sign == BigInt(other).m_sign) {
        if(digitsLeftSize != digitsRightSize) {
            return (digitsLeftSize < digitsRightSize) ^ this->m_sign;
        }
        return (this->m_bytes < BigInt(other).m_bytes) ^ this->m_sign;
    }
    return this->m_sign;
}

bool BigInt::operator > (const long long other) const
{
    return !(*this < other || *this == other);
}

bool BigInt::operator >= (const long long other) const
{
    return *this > other || *this == other;
}

bool BigInt::operator <= (const long long other) const
{
    return *this < other || *this == other;
}

BigInt BigInt::operator << (const size_t shift) const
{
    return this->shlBitsBase(shift);
}

BigInt BigInt::operator >> (const size_t shift) const
{
    return this->shrBitsBase(shift);
}

BigInt BigInt::operator + (const BigInt& other) const
{
    return this->addBase(other);
}

BigInt BigInt::operator - (const BigInt& other) const
{
    return this->subBase(other);
}

BigInt BigInt::operator * (const BigInt& other) const
{
    return this->mulBase(other);
}

BigInt BigInt::operator / (const BigInt& other) const
{
    return this->divBase(other);
}

BigInt BigInt::operator % (const BigInt& other) const
{
    return this->remBase(other);
}

BigInt BigInt::operator | (const BigInt& other) const
{
    return this->orBase(other);
}

BigInt BigInt::operator&  (const BigInt& other) const
{
    return this->andBase(other);
}

BigInt BigInt::operator ^ (const BigInt& other) const
{
    return this->xorBase(other);
}

BigInt BigInt::operator ~ () const&
{
    return this->notBase();
}

BigInt BigInt::operator + (const long long other) const
{
    return this->addBase(other);
}

BigInt BigInt::operator - (const long long other) const
{
    return this->subBase(other);
}

BigInt BigInt::operator * (const long long other) const
{
    return this->mulBase(other);
}

BigInt BigInt::operator / (const long long other) const
{
    return this->divBase(other);
}

BigInt BigInt::operator % (const long long other) const
{
    return this->remBase(other);
}

BigInt BigInt::operator | (const long long other) const
{
    return this->orBase(other);
}

BigInt BigInt::operator&  (const long long other) const
{
    return this->andBase(other);
}

BigInt BigInt::operator ^ (const long long other) const
{
    return this->xorBase(other);
}

BigInt& BigInt::operator <<=(const size_t shift)
{
    *this = this->shlBitsBase(shift);
    return *this;
}

BigInt& BigInt::operator >>= (const size_t shift)
{
    *this = this->shrBitsBase(shift);
    return *this;
}

BigInt& BigInt::operator += (const BigInt& other)
{
    *this = this->addBase(other);
    return *this;
}

BigInt& BigInt::operator -= (const BigInt& other)
{
    *this = this->subBase(other);
    return *this;
}

BigInt& BigInt::operator *= (const BigInt& other)
{
    *this = this->mulBase(other);
    return *this;
}

BigInt& BigInt::operator /= (const BigInt& other)
{
    *this = this->divBase(other);
    return *this;
}

BigInt& BigInt::operator %=(const BigInt& other)
{
    *this = this->remBase(other);
    return *this;
}

BigInt& BigInt::operator |=(const BigInt& other)
{
    *this = this->orBase(other);
    return *this;
}

BigInt& BigInt::operator &=(const BigInt& other)
{
    *this = this->andBase(other);
    return *this;
}

BigInt& BigInt::operator ^=(const BigInt& other)
{
    *this = this->xorBase(other);
    return *this;
}

BigInt& BigInt::operator += (const long long other)
{
    *this = this->addBase(other);
    return *this;
}

BigInt& BigInt::operator -= (const long long other)
{
    *this = this->subBase(other);
    return *this;
}

BigInt& BigInt::operator *= (const long long other)
{
    *this = this->mulBase(other);
    return *this;
}

BigInt& BigInt::operator /= (const long long other)
{
    *this = this->divBase(other);
    return *this;
}

BigInt& BigInt::operator %= (const long long other)
{
    *this = this->remBase(other);
    return *this;
}

BigInt& BigInt::operator |= (const long long other)
{
    *this = this->orBase(other);
    return *this;
}

BigInt& BigInt::operator &= (const long long other)
{
    *this = this->andBase(other);
    return *this;
}

BigInt& BigInt::operator ^= (const long long other)
{
    *this = this->xorBase(other);
    return *this;
}

BigInt strToBigInt(const std::string& strValue, const Radix radix)
{
    return BigInt(strValue, radix);
}

BigInt strToBigInt(const char* strValue, const Radix radix)
{
    return strToBigInt(std::string(strValue), radix);
}

std::ostream& operator << (std::ostream& out, const BigInt& value)
{
    std::ios_base::fmtflags baseFlag = out.flags()&
            (std::ios_base::hex | std::ios_base::dec | std::ios_base::oct);
    std::ios_base::fmtflags showBaseFlag = out.flags()&  (std::ios_base::showbase);
    std::ios_base::fmtflags showPosFlag = out.flags()&  (std::ios_base::showpos);
    std::ios_base::fmtflags uppercaseFlag = out.flags()&  (std::ios_base::uppercase);
    size_t width = out.width();
    char fillChar = out.fill();
    std::string result;
    switch(baseFlag) {
    case std::ios_base::hex:
        result = value.toString(Radix::Hex, static_cast<bool>(uppercaseFlag));
        if(showBaseFlag) {
            result.insert(result.begin(), 'x');
            result.insert(result.begin(), '0');
        }
        break;
    case std::ios_base::dec:
        result = value.toString(Radix::Dec);
        break;
    case std::ios_base::oct:
        result = value.toString(Radix::Oct);
        if(showBaseFlag) {
            result.insert(result.begin(), '0');
        }
        break;
    default:
        result = value.toString(Radix::Dec);
        break;
    }
    if(showPosFlag) {
        result.insert(result.begin(), '+');
    }
    if(width > result.length()) {
        for(size_t i = 0; i < width - result.length(); i++) {
            result.insert(result.begin(), fillChar);
        }
    }
    out << result;
    return out;
}

std::istream& operator >> (std::istream& in, BigInt& value)
{
    std::ios_base::fmtflags baseFlag = std::cin.flags()&
            (std::ios_base::hex | std::ios_base::dec | std::ios_base::oct);
    std::string strValue;
    in >> strValue;
    switch(baseFlag) {
    case std::ios_base::hex:
        value = strToBigInt(strValue, Radix::Hex);
        if((strValue[0] == '+' || strValue[0] == '-')&&
                ((isdigit(strValue[1]) && strValue[1] - 48 <= 16) ||
                 (isupper(strValue[1]) && strValue[1] - 55 <= 16) ||
                 (islower(strValue[1]) && strValue[1] - 87 <= 16))) {
                  in.clear(std::ios::goodbit);
        } else if((isdigit(strValue[0]) && strValue[1] - 48 <= 16) ||
                (isupper(strValue[0]) && strValue[1] - 55 <= 16) ||
                (islower(strValue[0]) && strValue[1] - 87 <= 16)) {
                 in.clear(std::ios::goodbit);
        } else {
            in.clear(std::ios::failbit);
        }
        break;
    case std::ios_base::dec:
        value = strToBigInt(strValue, Radix::Dec);
        if((strValue[0] == '+' || strValue[0] == '-')&&
                ((isdigit(strValue[1]) && strValue[1] - 48 <= 10))) {
            in.clear(std::ios::goodbit);
        } else if(isdigit(strValue[0]) && strValue[1] - 48 <= 10) {
            in.clear(std::ios::goodbit);
        } else {
            in.clear(std::ios::failbit);
        }
        break;
    case std::ios_base::oct:
        value = strToBigInt(strValue, Radix::Oct);
        if((strValue[0] == '+' || strValue[0] == '-')&&
                ((isdigit(strValue[1]) && strValue[1] - 48 <= 8))) {
            in.clear(std::ios::goodbit);
        } else if(isdigit(strValue[0]) && strValue[1] - 48 <= 8) {
            in.clear(std::ios::goodbit);
        } else {
            in.clear(std::ios::failbit);
        }
        break;
    default:
        if((strValue[0] == '+' || strValue[0] == '-')&&
                ((isdigit(strValue[1]) && strValue[1] - 48 <= 10))) {
            in.clear(std::ios::goodbit);
        } else if(isdigit(strValue[0]) && strValue[1] - 48 <= 10) {
            in.clear(std::ios::goodbit);
        } else {
            in.clear(std::ios::failbit);
        }
        value = strToBigInt(strValue, Radix::Dec);
        break;
    }
    return in;
}

BigInt shr(const BigInt& value, const size_t shift)
{
    return value.shrBitsBase(shift);
}

BigInt shl(const BigInt& value, const size_t shift)
{
    return value.shlBitsBase(shift);
}

BigInt sqr(const BigInt& value)
{
    return value.powBase(2);
}

BigInt pow(const BigInt& value, const long long exp)
{
    return value.powBase(exp);
}

BigInt pow(const BigInt& value, const BigInt exp)
{
    return value.powBase(exp);
}

BigInt powMod(const BigInt& value, const long long exp, const BigInt& mod)
{
    return value.powModBase(exp, mod);
}

BigInt powMod(const BigInt& value, const BigInt exp, const BigInt& mod)
{
    return value.powModBase(exp, mod);
}

BigInt sqrt(const BigInt& value)
{
    return value.rootBase(2);
}

BigInt root(const BigInt& value, const long long exp)
{
    return value.rootBase(exp);
}

BigInt root(const BigInt& value, const BigInt& exp)
{
    return value.rootBase(exp);
}

BigInt abs(const BigInt& value)
{
   if(value.m_sign) {
       return -value;//value.mulBase(-1);
   }
   return value;
}

BigInt gcd(const BigInt& value_1, const BigInt& value_2)
{
    BigInt _value_1 = std::max(value_1, value_2);
    BigInt _value_2 = std::min(value_1, value_2);
    while(_value_2 > 0) {
        _value_1 = _value_1.remBase(_value_2);
        std::swap(_value_1, _value_2);
    }
    return _value_1;
}

BigInt gcdExt(const BigInt& value_1, const BigInt& value_2,
               BigInt& factor_1, BigInt& factor_2)
{
    BigInt _value_1 = std::max(value_1, value_2);
    BigInt _value_2 = std::min(value_1, value_2);
    if(_value_2 == 0) {
        factor_1 = 1;
        factor_2 = 0;
        return _value_1;
    }
    BigInt q, r, x_1, x_2(1), y_1(1), y_2;
    while(_value_2 > 0) {
        q = _value_1.divBase(_value_2);
        r = _value_1.subBase(q.mulBase(_value_2));
        factor_1 = x_2.subBase(q.mulBase(x_1));
        factor_2 = y_2.subBase(q.mulBase(y_1));
        _value_1 = _value_2;
        _value_2 = r;
        x_2 = x_1;
        x_1 = factor_1;
        y_2 = y_1;
        y_1 = factor_2;
    }
    factor_1 = x_2;
    factor_2 = y_2;
    return _value_1;
}

int getLegendreSymbol(const BigInt& aValue, const BigInt& pValue)
{
    BigInt mod = aValue.remBase(pValue);
    if(mod == 0) {
        return 0;
    }
    if(mod == 1) {
        return 1;
    }
    for(BigInt i = 1; i <= pValue; i++) {
        if(i.mulBase(i).remBase(pValue) == mod) {
            return 1;
        }
    }
    return -1;
}

int getJacobiSymbol(const BigInt& aValue, const BigInt& pValue)
{
    if(pValue.isEven() || pValue < 1) {
        throw BigIntException("Invalid value of function parameters",
                               ExceptionCode::InvalidValue);
    }
    if(pValue == 1) {
        return 1;
    }
    BigInt _aValue(aValue);
    BigInt _pValue(pValue);
    if(gcd(_aValue, _pValue) != 1) {
        return 0;
    }
    int result = 1;
    if(_aValue < 0) {
        _aValue = -_aValue;
        if(_pValue.remBase(4) == 3) {
            result = -result;
        }
    }
    while(_aValue != 0) {
        BigInt tempValue_1(0);
        while(_aValue.isEven()) {
            tempValue_1 = tempValue_1.addBase(1);
            _aValue = _aValue.divBase(2);
        }
        if(!tempValue_1.isEven() && ((_pValue.remBase(8) == 3) || (_pValue.remBase(8) == 5))) {
            result = -result;
        }
        if((_aValue.remBase(4) == _pValue.remBase(4)) && (_pValue.remBase(4) == 3)) {
            result = -result;
        }
        BigInt tempValue_2 = _aValue;
        _aValue = _pValue.remBase(tempValue_2);
        _pValue = tempValue_2;
    }
    return result;
}
