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

const char* BigIntException::what() const noexcept {
    return m_message.c_str();
}

ExceptionCode BigIntException::getExceptionCode() const noexcept {
    return m_code;
}

std::vector<std::complex<double>>getFft(const std::vector<std::complex<double>>& value,
                                        const bool invert) {
    std::vector<std::complex<double>> result = value;
    size_t resultSize = (value.size());
    if(resultSize == 1) {
        return std::vector<std::complex<double>>(1, value[0]);
    }
    std::vector<std::complex<double>> oddValue(resultSize / 2);
    std::vector<std::complex<double>> evenValue(resultSize / 2);
    for(size_t i = 0, j = 0; i < resultSize; i += 2, ++j) {
        oddValue[j] = value[i];
        evenValue[j] = value[i + 1];
    }
    oddValue = getFft(oddValue, invert);
    evenValue = getFft(evenValue, invert);
    double angle = 2 * PI / resultSize * (invert ? -1 : 1);
    std::complex<double> root_1(1, 0);
    std::complex<double> root_n(std::cos(angle), std::sin(angle));
    for(size_t i = 0; i < resultSize / 2; ++i) {
        result[i] = oddValue[i] + root_1 * evenValue[i];
        result[i + resultSize / 2] = oddValue[i] - root_1 * evenValue[i];
        if(invert) {
            result[i] /= 2;
            result[i + resultSize / 2] /= 2;
        }
        root_1 *= root_n;
    }
    return result;
}

std::vector<uint8_t> normalise(std::vector<uint8_t>&& value) {
    auto nonZero = std::find_if(value.rbegin(), value.rend(),
                                [](uint8_t x) {return x != 0;});
    if(nonZero == value.rend()) {
        return {0};
    }
    value.resize(std::distance(value.begin(), nonZero.base()));
    return std::move(value);
}

std::vector<uint8_t> mulByte(const std::vector<uint8_t>& value, uint8_t byte) {
    if (byte == 0) {
        return {0};
    }
    std::vector<uint8_t> result(value.size() + 1, 0);
    uint16_t carry = 0;
    for (size_t i = 0; i < value.size(); ++i) {
        uint16_t resultByte = static_cast<uint16_t>(value[i]) * byte + carry;
        result[i] = static_cast<uint8_t>(resultByte & 0xFF);
        carry = resultByte >> 8;
    }
    if (carry) {
        result[value.size()] = static_cast<uint8_t>(carry);
    }
    return normalise(std::move(result));
}

std::vector<uint8_t> subBytes(const std::vector<uint8_t>& leftValue,
                              const std::vector<uint8_t>& rightValue) {
    std::vector<uint8_t> result = leftValue;
    int borrow{0};
    for (size_t i = 0; i < result.size(); i++) {
        int resultByte = result[i] - borrow - (i < rightValue.size() ? rightValue[i] : 0);
        if (resultByte < 0)
        {
            resultByte += 256;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result[i] = static_cast<uint8_t>(resultByte & 0xff);
    }
    return normalise(std::move(result));
}

int compareBytes(const std::vector<uint8_t>& leftValue,
                 const std::vector<uint8_t>& rightValue) {
    if(leftValue.size() != rightValue.size()) {
        return leftValue.size() < rightValue.size() ? -1 : 1;
    }
    for(size_t i = leftValue.size(); i-- > 0; )
        if(leftValue[i] != rightValue[i]) {
            return leftValue[i] < rightValue[i] ? -1 : 1;
        }
    return 0;
}

int charToInt(const uint8_t value) {
    if(value >= '0' && value <= '9') {
        return value - '0';
    }
    if(value >= 'a' && value <= 'f') {
        return value - 87;
    }
    return value - 55;
}

std::string byteToBinaryString(const uint8_t byte) {
    std::string result;
    uint8_t _byte = byte;
    for(uint8_t i = 0; i < 8; i++) {
        result.insert(result.begin(), '0' + (_byte & 1));
        _byte = _byte >> 1;
    }
    return result;
}

uint8_t dividedByBase(std::string& value) {
    int remainder{0};
    std::string result;
    for(char digit : value) {
        int current = remainder * 10 + (digit - '0');
        int quotient = current >> 8;
        remainder = current & 0xff;
        if(!result.empty() || quotient != 0) {
            result.push_back(quotient + '0');
        }
    }
    value = result.empty() ? "0" : result;
    return static_cast<uint8_t>(remainder & 0xff);
}

template <typename T>
std::vector<uint8_t> toBytes(const T value) {
    std::vector<uint8_t> result(sizeof(T));
    std::memcpy(result.data(), &value, sizeof(T));
    return normalise(std::move(result));
}

template <typename T>
T bytesToInteger(const std::vector<uint8_t>& value) {
    T result = 0;
    for (size_t i = value.size(); i-- > 0; ) {
        result = (result << 8) | value[i];
    }
    return result;
}

std::vector<uint8_t> hexadecimalStringToBytesVector(const std::string& value) {
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
    return result;
}

std::vector<uint8_t> decimalStringToBytesVector(const std::string& value) {
    std::vector<uint8_t> result;
    if(value == "0") {
        result.push_back(0);
        return result;
    }
    std::string _value = value;
    while (_value != "0") {
        uint8_t byte = dividedByBase(_value);
        result.push_back(byte);
    }
    return result;
}

std::vector<uint8_t> octalStringToBytesVector(const std::string& value) {
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
            byte = ((*it - '0') >> (3 - bitCount)) & ((1 << bitCount) - 1);
        }
    }
    if(bitCount > 0) {
        result.push_back(byte);
    }
    return result;
}

std::vector<uint8_t> binaryStringToBytesVector(const std::string& value) {
    std::vector<uint8_t> result((value.length() + 7) >> 3, 0);
    size_t i{0};
    for(auto it = value.rbegin(); it != value.rend(); it++) {
        size_t byteIndex = i >> 3;
        size_t bitIndex = i & 7;
        if(*it == '1') {
            result[byteIndex] |= (1 << bitIndex);
        }
        i++;
    }
    return result;
}

std::string bytesVectorToHexadecimalString(const std::vector<uint8_t>& value, const bool upper) {
    auto nonZero = std::find_if(value.rbegin(), value.rend(),
                                [](uint8_t x) { return x != 0; });
    if(nonZero == value.rend()) {
        return "0";
    }
    std::string result;
    static constexpr char hexUpper[] = "0123456789ABCDEF";
    static constexpr char hexLower[] = "0123456789abcdef";
    for(auto it = value.rbegin(); it != value.rend(); ++it) {
        uint8_t byte = *it;
        if(upper) {
            result.push_back(hexUpper[byte >> 4]);
            result.push_back(hexUpper[byte & 0xf]);
        } else {
            result.push_back(hexLower[byte >> 4]);
            result.push_back(hexLower[byte & 0xf]);
        }
    }
    if(*(result.begin()) == '0') {
        result.erase(result.begin());
    }
    return result;
}

std::string bytesVectorToDecimalString(const std::vector<uint8_t>& value) {
    auto nonZero = std::find_if(value.rbegin(), value.rend(),
                                [](uint8_t x) { return x != 0; });
    if(nonZero == value.rend()) {
        return "0";
    }
    std::vector<uint8_t> bytes(value.begin(), nonZero.base());
    std::vector<uint32_t> words((bytes.size() + 3) >> 2, 0);
    for (size_t i = 0; i < bytes.size(); ++i) {
        size_t wordIdx = i >> 2;
        size_t bytePos = i & 3;
        words[wordIdx] |= static_cast<uint32_t>(bytes[i]) << (bytePos << 3);
    }
    const uint32_t base = 1000000000;
    std::vector<uint32_t> resultDigits;
    while (words.size() > 1 || words[0] != 0) {
        uint64_t remainder = 0;
        for (size_t i = words.size(); i-- > 0; ) {
            uint64_t current = (remainder << 32) | words[i];
            words[i] = static_cast<uint32_t>(current / base);
            remainder = current % base;
        }
        resultDigits.push_back(static_cast<uint32_t>(remainder));
        while (words.size() > 1 && words.back() == 0) {
            words.pop_back();
        }
    }
    std::string result;
    if (!resultDigits.empty()) {
        result = std::to_string(resultDigits.back());
        resultDigits.pop_back();
    }
    for (auto it = resultDigits.rbegin(); it != resultDigits.rend(); ++it) {
        char buffer[10];
        snprintf(buffer, sizeof(buffer), "%09u", *it);
        result += buffer;
    }
    return result;
}

std::string bytesVectorToOctalString(const std::vector<uint8_t>& value) {
    auto nonZero = std::find_if(value.rbegin(), value.rend(),
                                [](uint8_t x) { return x != 0; });
    if (nonZero == value.rend()) {
        return "0";
    }
    std::vector<uint8_t> bytes(value.begin(), nonZero.base());
    std::vector<uint32_t> words((bytes.size() + 3) >> 2, 0);
    for (size_t i = 0; i < bytes.size(); ++i) {
        size_t wordIdx = i >> 2;
        size_t bytePos = i & 3;
        words[wordIdx] |= static_cast<uint32_t>(bytes[i]) << (8 * bytePos);
    }
    const uint32_t base = 1073741824;
    std::vector<uint32_t> resultDigits;
    while (!(words.size() == 1 && words[0] == 0)) {
        uint64_t remainder = 0;
        for (size_t i = words.size(); i-- > 0; ) {
            uint64_t current = (remainder << 32) | words[i];
            words[i] = static_cast<uint32_t>(current / base);
            remainder = current % base;
        }
        resultDigits.push_back(static_cast<uint32_t>(remainder));
        while (words.size() > 1 && words.back() == 0) {
            words.pop_back();
        }
    }
    std::string result;
    if (resultDigits.empty()) return "0";
    char buffer[12];
    snprintf(buffer, sizeof(buffer), "%o", resultDigits.back());
    result = buffer;
    for (auto it = resultDigits.rbegin() + 1; it != resultDigits.rend(); ++it) {
        snprintf(buffer, sizeof(buffer), "%010o", *it);
        result += buffer;
    }
    return result;
}

std::string bytesVectorToBinaryString(const std::vector<uint8_t>& value) {
    std::string result;
    result.reserve(value.size() << 3);
    for(auto it = value.rbegin(); it != value.rend(); ++it) {
        uint8_t byte = *it;
        result += byteToBinaryString(byte);
    }
    while(result[0] == '0' && result.length() > 1) {
        result.erase(result.begin());
    }
    return result;
}

std::vector<uint8_t> bytesVectorToTwosComplement(const std::vector<uint8_t>& value) {
    std::vector<uint8_t> result = value;
    for(size_t i = 0; i < result.size(); i++) {
        result[i] = ~result[i];
    }
    uint16_t carry = 1;
    for(size_t i = 0; i < result.size() && carry > 0; i++) {
        uint16_t sum = result[i] + carry;
        result[i] = static_cast<uint8_t>(sum & 0xff);
        carry = sum >> 8;
    }
    return result;
}

std::vector<uint8_t> bytesVectorToTwosComplement(std::vector<uint8_t>&& value) {
    for(size_t i = 0; i < value.size(); i++) {
        value[i] = ~value[i];
    }
    uint16_t carry = 1;
    for(size_t i = 0; i < value.size() && carry > 0; i++) {
        uint16_t sum = value[i] + carry;
        value[i] = static_cast<uint8_t>(sum & 0xff);
        carry = sum >> 8;
    }
    return std::move(value);
}

BigInt::BigInt() {
    m_bytes.push_back(0);
}

BigInt::BigInt(const int value) : BigInt() {
    m_sign = value < 0;
    m_bytes = toBytes(std::abs(value));
}

BigInt::BigInt(const long value) : BigInt() {
    m_sign = value < 0;
    m_bytes = toBytes(std::abs(value));
}

BigInt::BigInt(const long long value) : BigInt() {
    m_sign = value < 0;
    m_bytes = toBytes(std::abs(value));
}

BigInt::BigInt(const unsigned int value) : BigInt() {
    m_bytes = toBytes(value);
}

BigInt::BigInt(const unsigned long value) : BigInt() {
    m_bytes = toBytes(value);
}

BigInt::BigInt(const unsigned long long value) : BigInt() {
    m_bytes = toBytes(value);
}

BigInt::BigInt(const std::string& value, const Radix radix) : BigInt() {
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

BigInt::BigInt(const char *strValue, const Radix radix) : BigInt(std::string(strValue), radix) {

}

BigInt::BigInt(const BigInt& other) :
    m_bytes(other.m_bytes),
    m_sign(other.m_sign)
{

}

BigInt::BigInt(BigInt&& other) noexcept :
    m_bytes(std::move(other.m_bytes)),
    m_sign(other.m_sign)
{
    other.m_bytes.clear();
    other.m_bytes.push_back(0);
    other.m_sign = false;
}

BigInt& BigInt::operator = (const BigInt& other) {
    if(this !=& other) {
        m_bytes = other.m_bytes;
        m_sign = other.m_sign;
    }
    return *this;
}

BigInt& BigInt::operator = (BigInt&& other) noexcept {
    if(this !=& other) {
        m_bytes = std::move(other.m_bytes);
        m_sign = other.m_sign;
        other.m_bytes.clear();
        other.m_bytes.push_back(0);
        other.m_sign = false;
    }
    return *this;
}

BigInt& BigInt::operator = (const long long value) {
    *this = BigInt(value);
    return *this;
}

size_t BigInt::length() const {
    return m_bytes.size();
}

bool BigInt::isEven() const {
    return (m_bytes[0] & 1) == 0;
}

size_t BigInt::numBits() const {
    if(*this == 0) {
        return 0;
    }
    size_t bits = (m_bytes.size() - 1) << 3;
    uint8_t byte = m_bytes.back();
    while(byte) {
        bits++;
        byte >>= 1;
    }
    return bits;
}

std::string BigInt::toString(const Radix radix, const bool upper) const {
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
    if(m_sign) {
        result.insert(result.begin(), '-');
    }
    return result;
}

std::string BigInt::toHex(const bool upper) const {
    return toString(Radix::Hex, upper);
}

std::string BigInt::toDec() const {
    return toString();
}

std::string BigInt::toOct() const {
    return toString(Radix::Oct);
}

std::string BigInt::toBin() const {
    return toString(Radix::Bin);
}

int BigInt::toInt() const {
    if(length() > sizeof (int)) {
        throw BigIntException("Length error", ExceptionCode::LengthError);
    }
    int result = bytesToInteger<int>(m_bytes);
    return m_sign ? -result : result;
}

long BigInt::toLong() const {
    if(length() > sizeof (long)) {
        throw BigIntException("Length error", ExceptionCode::LengthError);
    }
    long result = bytesToInteger<long>(m_bytes);
    return m_sign ? -result : result;
}

long long BigInt::toLongLong() const {
    if(length() > sizeof (long long)) {
        throw BigIntException("Length error", ExceptionCode::LengthError);
    }
    long long result = bytesToInteger<long long>(m_bytes);
    return m_sign ? -result : result;
}

unsigned int BigInt::toUInt() const {
    return static_cast<unsigned int>(toInt());
}

unsigned long BigInt::toULong() const {
    return static_cast<unsigned long>(toLong());
}

unsigned long long BigInt::toULongLong() const {
    return static_cast<unsigned long long>(toLongLong());
}

void BigInt::setMulMethod(MulMethod mulMethod) {
    m_mulMethod = mulMethod;
}

BigInt BigInt::shlBitsBase(const size_t shift) const {
    if(shift == 0) {
        return *this;
    }
    BigInt result(*this);
    size_t byteShift = shift >> 3;
    size_t bitShift = shift & 7;
    if(byteShift + length() > m_bytes.max_size()){
        throw BigIntException("Length error", ExceptionCode::LengthError);
    }
    if(byteShift > 0) {
        result.m_bytes.insert(result.m_bytes.begin(), byteShift, 0);
    }
    if(bitShift > 0) {
        uint8_t carry{0};
        for(size_t i = 0; i < result.length(); i++) {
            uint8_t current = (result.m_bytes[i] << bitShift) | carry;
            carry = result.m_bytes[i] >> (8 - bitShift);
            result.m_bytes[i] = current;
        }
        if(carry > 0) {
            result.m_bytes.push_back(carry);
        }
    }
    result.m_bytes = normalise(std::move(result.m_bytes));
    return result;
}

BigInt BigInt::shrBitsBase(const size_t shift) const {
    if(shift == 0) {
        return *this;
    }
    BigInt result(*this);
    size_t byteShift = shift >> 3;
    size_t bitShift = shift & 7;
    if(length() <= byteShift) {
        return 0;
    }
    if(byteShift > 0) {
        result.m_bytes.erase(result.m_bytes.begin(), result.m_bytes.begin() + byteShift);
    }
    if(bitShift > 0) {
        uint8_t carry{0};
        for(size_t i = result.length(); i-- > 0;) {
            uint8_t current = (result.m_bytes[i] >> bitShift) | carry;
            carry = result.m_bytes[i] << (8 - bitShift);
            result.m_bytes[i] = current;
        }
    }
    result.m_bytes = normalise(std::move(result.m_bytes));
    return result;
}

BigInt BigInt::shlBytesBase(const size_t shift) const {
    if(shift == 0) {
        return *this;
    }
    if(shift + length() > m_bytes.max_size()){
        throw BigIntException("Length error", ExceptionCode::LengthError);
    }
    BigInt result(*this);
    result.m_bytes.insert(result.m_bytes.begin(), shift, 0);
    return result;
}

BigInt BigInt::shrBytesBase(const size_t shift) const {
    if(shift == 0) {
        return *this;
    }
    if(length() <= shift) {
        return 0;
    }
    BigInt result(*this);
    result.m_bytes.erase(result.m_bytes.begin(), result.m_bytes.begin() + shift);
    return result;
}

template<typename Op>
BigInt BigInt::bitwiseOperation(const BigInt& value, Op operation, bool isAnd) const {
    if(isAnd && (*this == 0 || value == 0)) {
        return 0;
    }
    if(!isAnd && (*this == 0)) {
        return value;
    }
    if(!isAnd && (value == 0)) {
        return *this;
    }
    const size_t maxLength = std::max(length(), value.length());
    BigInt result;
    result.m_bytes.clear();
    result.m_bytes.reserve(maxLength);
    result.m_sign = isAnd ? (m_sign & value.m_sign) : (m_sign | value.m_sign) ^ (isAnd ? 0 : 0);
    if(!m_sign && !value.m_sign) {
        for (size_t i = 0; i < maxLength; ++i) {
            uint8_t a = (i < length()) ? m_bytes[i] : 0;
            uint8_t b = (i < value.length()) ? value.m_bytes[i] : 0;
            result.m_bytes.push_back(operation(a, b));
        }
        result.m_bytes = normalise(std::move(result.m_bytes));
        return result;
    }
    std::vector<uint8_t> leftBytes = m_bytes;
    std::vector<uint8_t> rightBytes = value.m_bytes;
    leftBytes.resize(maxLength, 0);
    rightBytes.resize(maxLength, 0);
    if(m_sign) {
        leftBytes = bytesVectorToTwosComplement(std::move(leftBytes));
    }
    if(value.m_sign) {
        rightBytes = bytesVectorToTwosComplement(std::move(rightBytes));
    }

    for(size_t i = 0; i < maxLength; ++i) {
        result.m_bytes.push_back(operation(leftBytes[i], rightBytes[i]));
    }

    if(result.m_sign) {
        result.m_bytes = bytesVectorToTwosComplement(std::move(result.m_bytes));
    }
    result.m_bytes = normalise(std::move(result.m_bytes));
    return result;
}

BigInt BigInt::orBase(const BigInt& value) const {
    if(abs(*this) <= LLONG_MAX && abs(value) <= LLONG_MAX) {
        return toLongLong() | value.toLongLong();
    }
    return bitwiseOperation(value, [](uint8_t leftByte, uint8_t rightByte) { return leftByte | rightByte; });
}

BigInt BigInt::andBase(const BigInt& value) const {
    if(abs(*this) <= LLONG_MAX && abs(value) <= LLONG_MAX) {
        return toLongLong() & value.toLongLong();
    }
    return bitwiseOperation(value, [](uint8_t leftByte, uint8_t rightByte) { return leftByte & rightByte; }, true);
}

BigInt BigInt::xorBase(const BigInt& value) const {
    if(abs(*this) <= LLONG_MAX && abs(value) <= LLONG_MAX) {
        return toLongLong() ^ value.toLongLong();
    }
    return bitwiseOperation(value, [](uint8_t leftByte, uint8_t rightByte) { return leftByte ^ rightByte; });
}

BigInt BigInt::notBase() const {
    if(abs(*this) <= LLONG_MAX) {
        return ~toLongLong();
    } else {
        if(m_sign) {
            return abs(*this).subBase(1);
        } else {
            return -(addBase(1));
        }
    }
}

BigInt BigInt::addBase(const BigInt& value) const {
    if(*this == 0 && value == 0) {
        return 0;
    }
    if(*this == 0) {
        return value;
    }
    if(value == 0) {
        return *this;
    }
    if(std::max(length(), value.length()) + 1 <= LLONG_SIZE) {
        return toLongLong() + value.toLongLong();
    }
    const std::vector<uint8_t>& leftValue = (length() > value.length()) ? m_bytes : value.m_bytes;
    const std::vector<uint8_t>& rightValue = (length() > value.length()) ? value.m_bytes : m_bytes;
    BigInt result;
    result.m_bytes.clear();
    if(m_sign == value.m_sign) {
        result.m_sign = m_sign;
        uint16_t leftByte, rightByte, carry{0};
        for(size_t i = 0; i < rightValue.size(); i++) {
            leftByte = static_cast<uint16_t>(leftValue[i]);
            rightByte = static_cast<uint16_t>(rightValue[i]);
            result.m_bytes.push_back(static_cast<uint8_t>((rightByte + leftByte + carry) & 0Xff));
            carry = (rightByte + leftByte + carry) >> 8;
        }
        for(size_t i = rightValue.size(); i < leftValue.size(); i++) {
            leftByte = leftValue[i];
            result.m_bytes.push_back(static_cast<uint8_t>((leftByte + carry) & 0Xff));
            carry = (leftByte + carry) >> 8;
            if(carry == 0) {
                result.m_bytes.insert(result.m_bytes.end(), leftValue.begin() + i + 1, leftValue.end());
                break;
            }
        }
        if(carry) {
            result.m_bytes.push_back(carry);
        }
    } else {
        result = m_sign ? value.subBase(-*this) : subBase(-value);
    }
    result.m_bytes = normalise(std::move(result.m_bytes));
    return result;
}

BigInt BigInt::subBase(const BigInt& value) const {
    if(*this == value) {
        return 0;
    }
    if(*this == 0) {
        return -value;
    }
    if(value == 0) {
        return *this;
    }
    if(std::max(length(), value.length()) + 1 <= LLONG_SIZE) {
        return toLongLong() - value.toLongLong();
    }
    BigInt result;
    result.m_bytes.clear();
    if(m_sign == value.m_sign) {
        int rightByte, resultByte, leftByte, borrow{0};
        const std::vector<uint8_t>& leftValue = (abs(*this) > abs(value)) ? m_bytes : value.m_bytes;
        const std::vector<uint8_t>& rightValue = (abs(*this) > abs(value)) ? value.m_bytes : m_bytes;
        result.m_sign = (*this > value) ? false : true;
        size_t i{0};
        for(; i < rightValue.size(); i++) {
            leftByte = static_cast<int>(leftValue[i]);
            rightByte = static_cast<int>(rightValue[i]);
            resultByte = leftByte - borrow - rightByte;
            if(resultByte < 0) {
                resultByte += 256;
                borrow = 1;
            } else {
                borrow = 0;
            }
            result.m_bytes.push_back(static_cast<uint8_t>(resultByte & 0xff));
        }
        for (; i < leftValue.size(); ++i) {
            resultByte = static_cast<int>(leftValue[i]) - borrow;
            if (resultByte < 0) {
                resultByte += 256;
                borrow = 1;
            } else {
                borrow = 0;
            }
            result.m_bytes.push_back(static_cast<uint8_t>(resultByte & 0xff));
        }
    } else {
        result = addBase(-value);
    }
    result.m_bytes = normalise(std::move(result.m_bytes));
    return result;
}

BigInt BigInt::mulNative(const BigInt& value) const {
    BigInt result;
    result.m_bytes.resize(length() + value.length(), 0);
    result.m_sign = m_sign ^ value.m_sign;
    for(size_t i = 0; i < value.length(); i++) {
        uint32_t carry{0};
        for(size_t j = 0; j < length(); j++) {
            uint32_t byte = static_cast<uint32_t>(m_bytes[j]) *
                            static_cast<uint32_t>(value.m_bytes[i]) +
                            static_cast<uint32_t>(result.m_bytes[i + j]) + carry;
            carry = byte >> 8;
            result.m_bytes[i + j] = static_cast<uint8_t>(byte & 0xff);
        }
        if (carry > 0) {
            size_t position = i + length();
            while (carry > 0 && position < result.length()) {
                uint32_t byte = result.m_bytes[position] + carry;
                result.m_bytes[position] = static_cast<uint8_t>(byte & 0xff);
                carry = byte >> 8;
                position++;
            }
            if (carry > 0) {
                result.m_bytes.push_back(static_cast<uint8_t>(carry & 0xff));
            }
        }
    }
    result.m_bytes = normalise(std::move(result.m_bytes));
    return result;
}

BigInt BigInt::mulKaratsuba(const BigInt& value) const {
    if(length() == 1 || value.length() == 1) {
        return mulNative(value);
    }
    size_t maxSize = std::max(length(), value.length());
    size_t halfSize = (maxSize + 1) >> 1;
    BigInt leftValueLow;
    leftValueLow.m_bytes.assign(m_bytes.begin(),
                                m_bytes.begin() + std::min(halfSize,
                                length()));
    leftValueLow.m_bytes.resize(halfSize, 0);
    BigInt rightValueLow;
    rightValueLow.m_bytes.assign(value.m_bytes.begin(),
                                 value.m_bytes.begin() + std::min(halfSize,
                                 value.length()));
    rightValueLow.m_bytes.resize(halfSize, 0);
    BigInt leftValueHigh;
    if (length() > halfSize) {
        leftValueHigh.m_bytes.assign(m_bytes.begin() + halfSize,
                                     m_bytes.end());
        leftValueHigh.m_bytes = normalise(std::move(leftValueHigh.m_bytes));
    }
    BigInt rightValueHigh;
    if (value.length() > halfSize) {
        rightValueHigh.m_bytes.assign(value.m_bytes.begin() + halfSize,
                                      value.m_bytes.end());
        rightValueHigh.m_bytes = normalise(std::move(rightValueHigh.m_bytes));
    }
    BigInt p_1 = leftValueHigh.mulBase(rightValueHigh);
    BigInt p_2 = leftValueLow.mulBase(rightValueLow);
    BigInt p_3 = leftValueHigh.addBase(leftValueLow).mulBase(rightValueHigh.addBase(rightValueLow));
    BigInt result = p_1.shlBytesBase(halfSize << 1).addBase((p_3.subBase(p_2).subBase(p_1).shlBytesBase(halfSize)).addBase(p_2));

    result.m_sign = m_sign ^ value.m_sign;
    result.m_bytes = normalise(std::move(result.m_bytes));
    return result;
}

BigInt BigInt::mulFft(const BigInt& value) const {
    BigInt result;
    result.m_bytes.clear();
    result.m_sign = m_sign ^ value.m_sign;
    std::vector<std::complex<double>> complexValue_1(m_bytes.begin(), m_bytes.end()),
            complexValue_2(value.m_bytes.begin(), value.m_bytes.end());
    size_t resultSize = 1;
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
    uint32_t carry = 0;
    uint32_t resultDigit;
    for(size_t i = 0; i < resultSize; ++i) {
        resultDigit = static_cast<unsigned int>(complexValue_1[i].real() + 0.5) + carry;
        carry = resultDigit >> 8;
        result.m_bytes.push_back(static_cast<uint8_t>(resultDigit & 0xff));
    }
    result.m_bytes.push_back(carry);
    result.m_bytes = normalise(std::move(result.m_bytes));
    return result;
}

BigInt BigInt::mulBase(const BigInt& value) const {
    if(*this == 0 || value == 0) {
        return 0;
    }
    if(*this == 1) {
        return value;
    }
    if(value == 1) {
        return *this;
    }
    if(length() + value.length() + 1 <= LLONG_SIZE) {
        return toLongLong() * value.toLongLong();
    }
    if(length() + value.length() + 1 > NATIVE_LIMIT) {
        if(m_mulMethod == MulMethod::Karatsuba) {
            return mulKaratsuba(value);
        } else if(m_mulMethod == MulMethod::FastFourierTransform) {
            return mulFft(value);
        }
    }
    return mulNative(value);
}

BigInt BigInt::divNative(const BigInt& value) const {
    const std::vector<uint8_t>& dividend = m_bytes;
    const std::vector<uint8_t>& divisor = value.m_bytes;
    std::vector<uint8_t> quotient;
    std::vector<uint8_t> remainder;
    for(size_t i = dividend.size(); i-- > 0;) {
        remainder.insert(remainder.begin(), dividend[i]);
        remainder = normalise(std::move(remainder));
        uint8_t byte{0};
        if (remainder.size() >= divisor.size()) {
            uint32_t remainderHigh{0}, divisorHigh{0};
            if (remainder.size() > divisor.size()) {
                remainderHigh = (static_cast<uint32_t>(remainder.back()) << 8) |
                               (remainder.size() > 1 ? remainder[remainder.size()-2] : 0);
            } else {
                remainderHigh = remainder.back();
            }
            divisorHigh = divisor.back();
            byte = static_cast<uint8_t>(remainderHigh / divisorHigh);
            if (byte > 255) {
                byte = 255;
            }
            uint8_t lowByte{0}, highByte{255};
            while (lowByte < highByte) {
                uint8_t midByte = (lowByte + highByte + 1) >> 1;
                if (compareBytes(mulByte(divisor, midByte), remainder) <= 0) {
                    lowByte = midByte;
                } else {
                    highByte = midByte - 1;
                }
            }
            byte = lowByte;
        }
        quotient.push_back(byte);
        if (byte > 0) {
            std::vector<uint8_t> resultByte = mulByte(divisor, byte);
            remainder = subBytes(remainder, resultByte);
            remainder = normalise(std::move(remainder));
        }
    }
    std::reverse(quotient.begin(), quotient.end());
    BigInt result;
    result.m_sign = m_sign ^ value.m_sign;
    result.m_bytes = normalise(std::move(quotient));
    return result;
}

BigInt BigInt::divNewtonRaphson(const BigInt& value) const {
    size_t shift = length() + value.length();
    BigInt powBase{2};
    powBase = powBase.shlBytesBase(shift);
    BigInt current{1};
    current = current.shlBytesBase(length());
    BigInt last;
    while(last != current) {
        last = current;
        current = (current.mulBase(powBase.subBase(current.mulBase(value)))).shrBytesBase(shift);
    }
    BigInt result = mulBase(current);
    result = result.shrBytesBase(shift);
    BigInt remainder = subBase(result.mulBase(value));
    if(remainder >= value) {
        result = result.addBase(1);
    }
    result.m_sign = m_sign ^ value.m_sign;
    result.m_bytes = normalise(std::move(result.m_bytes));
    return result;
}

BigInt BigInt::divBase(const BigInt& value) const {
    if(value == 0) {
        throw BigIntException("Divizion by zero", ExceptionCode::DivisionByZero);
    }

    if(abs(value) > abs(*this)) {
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
        return toLongLong() / value.toLongLong();
    } else if(length() < NATIVE_LIMIT && value.length() < NATIVE_LIMIT) {
        return shrBytesBase(numberOfZero).divNative(value.shrBytesBase(numberOfZero));
    } else {
        return shrBytesBase(numberOfZero).divNewtonRaphson(value.shrBytesBase(numberOfZero));
    }
}

BigInt BigInt::remBase(const BigInt& value) const {
    if(abs(value) > abs(*this)) {
        return *this;
    }
    if(value == *this) {
        return 0;
    }
    return subBase(value.mulBase(divBase(value)));
}

BigInt BigInt::powBase(long long exp) const {
    if(exp < 0) {
        return 0;
    }
    if(exp == 0) {
          return 1;
    }
    if(exp == 1) {
        return *this;
    }
    if(length() * exp + exp <= LLONG_SIZE) {
        return static_cast<long long>(std::pow(toLongLong(), exp));
    }
    BigInt result;
    if(exp&  1) {
        result = powBase(exp - 1);
        return result.mulBase(*this);
    } else {
        result = powBase(exp / 2);
        return result.mulBase(result);
    }
}

BigInt BigInt::powBase(const BigInt& exp) const {
    if(exp.m_sign) {
        return 0;
    }
    if(exp == 0) {
        return 1;
    }
    if(exp == 1) {
        return *this;
    }
    if(BigInt(length()).mulBase(exp).addBase(exp) <= LLONG_SIZE) {
        return static_cast<long long>(std::pow(toLongLong(), exp.toLongLong()));
    }
    BigInt result;
    if(exp.isEven()) {
        result = powBase(exp.divBase(2));
        return result.mulBase(result);
    } else {
        result = powBase(exp.subBase(1));
        return result.mulBase(*this);
    }
}

BigInt BigInt::powModBase(const long long exp, const BigInt& mod) const {
    if(exp < 0) {
        return 0;
    }
    if(exp == 0) {
        return 1;
    }
    if(exp == 1) {
        return *this;
    }
    if(length() * exp + exp <= LLONG_SIZE && abs(mod) <= LLONG_MAX) {
        return static_cast<long long>(std::pow(toLongLong(), exp)) % mod.toLongLong();
    }
    BigInt result;
    if(exp&  1) {
        result = powModBase(exp - 1, mod);
        return result.mulBase(*this).remBase(mod);
    } else {
        result = powModBase(exp / 2, mod);
        return result.mulBase(result).remBase(mod);
    }
}

BigInt BigInt::powModBase(const BigInt& exp, const BigInt& mod) const {
    if(exp < 0) {
        return 0;
    }
    if(exp == 0) {
        return 1;
    }
    if(exp == 1) {
        return *this;
    }
    if(BigInt(length()).mulBase(exp).addBase(exp) <= LLONG_SIZE && abs(mod) <= LLONG_MAX) {
        return static_cast<long long>(std::pow(toLongLong(), exp.toLongLong())) % mod.toLongLong();
    }
    BigInt result;
    if(exp.isEven()) {
        result = powModBase(exp.subBase(1), mod);
        return result.mulBase(*this).remBase(mod);
    } else {
        result = powModBase(exp.divBase(2), mod);
        return result.mulBase(result).remBase(mod);
    }
}

BigInt BigInt::rootBase(const long long exp) const {
    if(exp == 0) {
        throw BigIntException("Division by zero", ExceptionCode::DivisionByZero);
    }
    if(m_sign && (exp & 1) == 0) {
        throw BigIntException("Even root of negative number", ExceptionCode::InvalidRootDegree);
    }
    if(exp < 0) {
        throw BigIntException("Negative root degree", ExceptionCode::InvalidValue);
    }
    if(exp == 1) {
        return *this;
    }
    if(*this == 0 || *this == 1) {
        return *this;
    }
    size_t resultSize = (numBits() + exp - 1) / exp;
    resultSize = (resultSize + 7) / 8;
    if (resultSize == 0) resultSize = 1;
    BigInt result;
    result.m_bytes.resize(resultSize);
    for(size_t i = resultSize; i-- > 0;) {
        result.m_bytes[i] = 255;
        while (result.powBase(exp) > abs(*this) && result.m_bytes[i] > 0) {
            result.m_bytes[i]--;
        }
        if (result.m_bytes[i] == 0 && result.powBase(exp) > abs(*this)) {
            result.m_bytes[i] = 0;
        }
    }

    result.m_sign = m_sign;
    return result;
}

BigInt BigInt::rootBase(const BigInt& exp) const {
    if(exp == 0) {
        throw BigIntException("Division by zero", ExceptionCode::DivisionByZero);
    }
    if(m_sign && (exp & 1) == 0) {
        throw BigIntException("Even root of negative number", ExceptionCode::InvalidRootDegree);
    }
    if(exp < 0) {
        throw BigIntException("Negative root degree", ExceptionCode::InvalidValue);
    }
    if(exp == 1) {
        return *this;
    }
    if(*this == 0 || *this == 1) {
        return *this;
    }
    size_t resultSize = (numBits() + exp.toLongLong() - 1) / exp.toLongLong();
    resultSize = (resultSize + 7) / 8;
    if (resultSize == 0) resultSize = 1;
    BigInt result;
    result.m_bytes.resize(resultSize);
    for(size_t i = resultSize; i-- > 0;) {
        result.m_bytes[i] = 255;
        while (result.powBase(exp) > abs(*this) && result.m_bytes[i] > 0) {
            result.m_bytes[i]--;
        }
        if (result.m_bytes[i] == 0 && result.powBase(exp) > abs(*this)) {
            result.m_bytes[i] = 0;
        }
    }
    result.m_sign = m_sign;
    return result;
}

BigInt BigInt::operator -- () {
    *this = subBase(1);
    return *this;
}

BigInt BigInt::operator ++ () {
    *this = addBase(1);
    return *this;
}

BigInt BigInt::operator -- (int) {
    BigInt result(*this);
    *this = subBase(1);
    return result;
}

BigInt BigInt::operator ++ (int) {
    BigInt result(*this);
    *this = addBase(1);
    return result;
}

BigInt BigInt::operator - () const& {
    if(*this == 0) {
        return 0;
    }
    BigInt result(*this);
    result.m_sign = !m_sign;
    return result;
}

BigInt BigInt::operator + () const& {
    return *this;
}

bool BigInt::operator == (const BigInt& value) const {
    return (m_bytes == value.m_bytes) && (m_sign == value.m_sign);
}

bool BigInt::operator != (const BigInt& value) const {
    return !(*this == value);
}

bool BigInt::operator < (const BigInt& value) const {
    if (m_sign != value.m_sign) {
        return m_sign;
    }
    if(m_sign == value.m_sign) {
        if(length() != value.length()) {
            return (length() < value.length()) ^ m_sign;
        }
        for (size_t i = length(); i-- > 0; ) {
            if (m_bytes[i] != value.m_bytes[i]) {
                return (m_bytes[i] < value.m_bytes[i]) ^ m_sign;
            }
        }
    }
    return false;
}

bool BigInt::operator > (const BigInt& value) const {
    return !(*this < value || *this == value);
}

bool BigInt::operator >= (const BigInt& value) const {
    return *this > value || *this == value;
}

bool BigInt::operator <= (const BigInt& value) const {
    return *this < value || *this == value;
}

bool BigInt::operator == (const long long value) const {
    return (m_bytes == BigInt(value).m_bytes) && (m_sign == BigInt(value).m_sign);
}

bool BigInt::operator != (const long long value) const {
    return !(*this == value);
}

bool BigInt::operator < (const long long value) const {
    return *this < BigInt(value);
}

bool BigInt::operator > (const long long value) const {
    return !(*this < value || *this == value);
}

bool BigInt::operator >= (const long long value) const {
    return *this > value || *this == value;
}

bool BigInt::operator <= (const long long value) const {
    return *this < value || *this == value;
}

BigInt BigInt::operator << (const size_t shift) const {
    return shlBitsBase(shift);
}

BigInt BigInt::operator >> (const size_t shift) const {
    return shrBitsBase(shift);
}

BigInt BigInt::operator + (const BigInt& value) const {
    return addBase(value);
}

BigInt BigInt::operator - (const BigInt& value) const {
    return subBase(value);
}

BigInt BigInt::operator * (const BigInt& value) const {
    return mulBase(value);
}

BigInt BigInt::operator / (const BigInt& value) const {
    return divBase(value);
}

BigInt BigInt::operator % (const BigInt& value) const {
    return remBase(value);
}

BigInt BigInt::operator | (const BigInt& value) const {
    return orBase(value);
}

BigInt BigInt::operator&  (const BigInt& value) const {
    return andBase(value);
}

BigInt BigInt::operator ^ (const BigInt& value) const {
    return xorBase(value);
}

BigInt BigInt::operator ~ () const& {
    return notBase();
}

BigInt BigInt::operator + (const long long value) const {
    return addBase(value);
}

BigInt BigInt::operator - (const long long value) const {
    return subBase(value);
}

BigInt BigInt::operator * (const long long value) const {
    return mulBase(value);
}

BigInt BigInt::operator / (const long long value) const {
    return divBase(value);
}

BigInt BigInt::operator % (const long long value) const {
    return remBase(value);
}

BigInt BigInt::operator | (const long long value) const {
    return orBase(value);
}

BigInt BigInt::operator&  (const long long value) const {
    return andBase(value);
}

BigInt BigInt::operator ^ (const long long value) const {
    return xorBase(value);
}

BigInt& BigInt::operator <<=(const size_t shift) {
    *this = shlBitsBase(shift);
    return *this;
}

BigInt& BigInt::operator >>= (const size_t shift) {
    *this = shrBitsBase(shift);
    return *this;
}

BigInt& BigInt::operator += (const BigInt& value) {
    *this = addBase(value);
    return *this;
}

BigInt& BigInt::operator -= (const BigInt& value) {
    *this = subBase(value);
    return *this;
}

BigInt& BigInt::operator *= (const BigInt& value) {
    *this = mulBase(value);
    return *this;
}

BigInt& BigInt::operator /= (const BigInt& value) {
    *this = divBase(value);
    return *this;
}

BigInt& BigInt::operator %=(const BigInt& value) {
    *this = remBase(value);
    return *this;
}

BigInt& BigInt::operator |=(const BigInt& value) {
    *this = orBase(value);
    return *this;
}

BigInt& BigInt::operator &=(const BigInt& value) {
    *this = andBase(value);
    return *this;
}

BigInt& BigInt::operator ^=(const BigInt& value) {
    *this = xorBase(value);
    return *this;
}

BigInt& BigInt::operator += (const long long value) {
    *this = addBase(value);
    return *this;
}

BigInt& BigInt::operator -= (const long long value) {
    *this = subBase(value);
    return *this;
}

BigInt& BigInt::operator *= (const long long value) {
    *this = mulBase(value);
    return *this;
}

BigInt& BigInt::operator /= (const long long value) {
    *this = divBase(value);
    return *this;
}

BigInt& BigInt::operator %= (const long long value) {
    *this = remBase(value);
    return *this;
}

BigInt& BigInt::operator |= (const long long value) {
    *this = orBase(value);
    return *this;
}

BigInt& BigInt::operator &= (const long long value) {
    *this = andBase(value);
    return *this;
}

BigInt& BigInt::operator ^= (const long long value) {
    *this = xorBase(value);
    return *this;
}

bool operator == (const long long leftValue, const BigInt& rightValue) {
    return rightValue == leftValue;
}
bool operator != (const long long leftValue, const BigInt& rightValue) {
    return rightValue != leftValue;
}
bool operator < (const long long leftValue, const BigInt& rightValue) {
    return rightValue > leftValue;
}
bool operator > (const long long leftValue, const BigInt& rightValue) {
    return rightValue < leftValue;
}
bool operator <= (const long long leftValue, const BigInt& rightValue) {
    return rightValue >= leftValue;
}
bool operator >= (const long long leftValue, const BigInt& rightValue) {
    return rightValue <= leftValue;
}

BigInt operator + (const long long leftValue, const BigInt& rightValue) {
    return rightValue + leftValue;
}
BigInt operator - (const long long leftValue, const BigInt& rightValue) {
    return BigInt(leftValue) - rightValue;
}
BigInt operator * (const long long leftValue, const BigInt& rightValue) {
    return rightValue * leftValue;
}
BigInt operator / (const long long leftValue, const BigInt& rightValue) {
    return BigInt(leftValue) / rightValue;
}

BigInt operator & (const long long leftValue, const BigInt& rightValue) {
    return rightValue & leftValue;
}
BigInt operator | (const long long leftValue, const BigInt& rightValue) {
    return rightValue | leftValue;
}
BigInt operator ^ (const long long leftValue, const BigInt& rightValue) {
    return rightValue ^ leftValue;
}

BigInt strToBigInt(const std::string& strValue, const Radix radix) {
    return BigInt(strValue, radix);
}

BigInt strToBigInt(const char* strValue, const Radix radix) {
    return strToBigInt(std::string(strValue), radix);
}

std::ostream& operator << (std::ostream& out, const BigInt& value) {
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
    if(showPosFlag && !value.m_sign) {
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

std::istream& operator >> (std::istream& in, BigInt& value) {
    std::ios_base::fmtflags baseFlag = in.flags()&
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

BigInt shr(const BigInt& value, const size_t shift) {
    return value.shrBitsBase(shift);
}

BigInt shl(const BigInt& value, const size_t shift) {
    return value.shlBitsBase(shift);
}

BigInt sqr(const BigInt& value) {
    return value.powBase(2);
}

BigInt pow(const BigInt& value, const long long exp) {
    return value.powBase(exp);
}

BigInt pow(const BigInt& value, const BigInt exp) {
    return value.powBase(exp);
}

BigInt powMod(const BigInt& value, const long long exp, const BigInt& mod) {
    return value.powModBase(exp, mod);
}

BigInt powMod(const BigInt& value, const BigInt exp, const BigInt& mod) {
    return value.powModBase(exp, mod);
}

BigInt sqrt(const BigInt& value) {
    return value.rootBase(2);
}

BigInt root(const BigInt& value, const long long exp) {
    return value.rootBase(exp);
}

BigInt root(const BigInt& value, const BigInt& exp) {
    return value.rootBase(exp);
}

BigInt abs(const BigInt& value) {
   if(value.m_sign) {
       return -value;
   }
   return value;
}

BigInt gcd(const BigInt& value_1, const BigInt& value_2) {
    BigInt _value_1 = std::max(abs(value_1), abs(value_2));
    BigInt _value_2 = std::min(abs(value_1), abs(value_2));
    while(_value_2 > 0) {
        _value_1 = _value_1.remBase(_value_2);
        std::swap(_value_1, _value_2);
    }
    return _value_1;
}

BigInt gcdExt(const BigInt& value_1, const BigInt& value_2,
               BigInt& factor_1, BigInt& factor_2) {
    BigInt _value_1 = std::max(abs(value_1), abs(value_2));
    BigInt _value_2 = std::min(abs(value_1), abs(value_2));
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

int getLegendreSymbol(const BigInt& aValue, const BigInt& pValue) {
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

int getJacobiSymbol(const BigInt& aValue, const BigInt& pValue) {
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
