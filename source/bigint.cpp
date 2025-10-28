#include "bigint.h"

BigIntException::BigIntException(const std::string &message) :
    m_message{message}
{

}

BigIntException::BigIntException(const ExceptionCode code) :
    m_code{code}
{

}

BigIntException::BigIntException(const std::string &message,
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

std::vector<std::complex<double>>getFft(const std::vector<std::complex<double>> &value,
                                         const bool invert)
{
    std::vector<std::complex<double>> result = value;
    int resultSize = static_cast<int>(value.size());
    if(resultSize == 1) {
        return std::vector<std::complex<double>>(1, value[0]);
    }
    std::vector<std::complex<double>> notEvenValue(resultSize / 2);
    std::vector<std::complex<double>> evenValue(resultSize / 2);
    for(int i = 0, j = 0; i < resultSize; i += 2, ++j) {
        notEvenValue[j] = value[i];
        evenValue[j] = value[i + 1];
    }
    notEvenValue = getFft(notEvenValue, invert);
    evenValue = getFft(evenValue, invert);
    double angle = 2 * PI / resultSize * (invert ? -1 : 1);
    std::complex<double> root_1(1);
    std::complex<double> root_n(std::cos(angle), std::sin(angle));
    for(int i = 0; i < resultSize / 2; ++i) {
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

std::string binstrToAddition(const std::string &strValue)
{
    int carry = 1;
    std::string result = strValue;
    for(size_t i = 0; i < result.length(); i++) {
        result[i] = result[i] == '0' ? '1' : '0';
    }
    for(size_t i = 0; i < result.length(); i++) {
        result[result.length() - 1 - i] = ((result[result.length() - 1 - i] - '0') + carry) + '0';
        if(result[result.length() - 1 - i] == '2') {
            result[result.length() - 1 - i] = '0';
            carry = 1;
        } else {
            carry = 0;
        }
    }
    if(carry != 0) {
        result.insert(result.begin(), '1');
    }
    return result;
}

BigInt::BigInt()
{
    this->m_digits.push_back(0);
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

BigInt::BigInt(const std::string &strValue, const size_t radix) : BigInt()
{
    size_t beginDigits = 0;
    if(strValue[0] == '+' || strValue[0] == '-') {
        beginDigits = 1;
    }
    if(radix == 10 || radix < 2 || radix > 36) {
        for(size_t i = beginDigits; i < strValue.length(); i++) {
            if(std::isdigit(strValue[i])) {
                this->m_digits.push_back(strValue[i] - 48);
            } else {
                break;
            }
        }
        this->normalize();
    } else {
        BigInt rawValue;
        BigInt bigIntRadix(radix);
        BigInt digit;
        BigInt degreFactor;
        for(size_t i = beginDigits; i < strValue.length(); i++) {
            digit_t innerDigit;
            if(std::isdigit(strValue[i])) {
                innerDigit = strValue[i] - 48;
                if(innerDigit >= radix) {
                    break;
                }
            } else if(std::isupper(strValue[i]) && radix > 10) {
                innerDigit = strValue[i] - 55;
                if(innerDigit >= radix) {
                    break;
                }
            } else if(std::islower(strValue[i]) && radix > 10) {
                innerDigit = strValue[i] - 87;
                if(innerDigit >= radix) {
                    break;
                }
            } else {
                break;
            }
            rawValue.m_digits.push_back(innerDigit);
        }
        rawValue.normalize();
        std::reverse(rawValue.m_digits.begin(), rawValue.m_digits.end());
        if(rawValue != 0) {
            for(size_t i = 0; i < rawValue.size(); i++) {
                degreFactor = bigIntRadix.powBase(i);
                digit = rawValue.m_digits[i];
                digit = digit.mulBase(degreFactor);
                *this = this->addBase(digit);
            }
        } else {
            *this = 0;
        }
    }
    this->m_sign = strValue[0] == '-';
    if(this->m_digits[0] == 0) {
        this->m_sign = false;
    }
}

BigInt::BigInt(const char *strValue, const size_t radix) : BigInt(std::string(strValue), radix)
{

}

BigInt::BigInt(const BigInt &other)
{
    this->m_digits = other.m_digits;
    this->m_sign = other.m_sign;
}

#if __cplusplus >= 201103L
BigInt::BigInt(BigInt &&other)
{
    this->m_digits = std::move(other.m_digits);
    this->m_sign = std::move(other.m_sign);
    other.m_digits.clear();
    other.m_sign = false;
}
#endif

BigInt::~BigInt()
{

}

BigInt& BigInt::operator = (const BigInt &other)
{
    if(this != &other) {
        this->m_digits = other.m_digits;
        this->m_sign = other.m_sign;
    }
    return *this;
}

#if __cplusplus >= 201103L
BigInt& BigInt::operator = (BigInt &&other)
{
    if(this != &other) {
        this->m_digits = std::move(other.m_digits);
        this->m_sign = std::move(other.m_sign);
        other.m_digits.clear();
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

size_t BigInt::size() const
{
    return this->m_digits.size();
}

bool BigInt::isEven() const
{
    return ~this->m_digits[this->size() - 1] & 1;
}

unsigned long long BigInt::numBits()
{
    if(*this == 0) {
        return 0;
    }
    BigInt innerValue(*this);
    unsigned long long result = 0;
    while(innerValue != 0) {
        result++;
        innerValue = innerValue.divBase(BigInt(2));
    }
    return result;
}

std::string BigInt::toString(const size_t radix, const bool upper) const
{
    std::string result;
    if(radix == 10 || radix < 2 || radix > 36) {
        for(size_t i = 0; i < this->size(); i++) {
            result.push_back(this->m_digits[i] + '0');
        }
    } else {
        BigInt innerValue(*this);
        innerValue.m_sign = false;
        char charDigit;
        int intDigit;
        do {
            intDigit = (innerValue.remBase(radix)).toInt();
            innerValue = innerValue.divBase(radix);
            if(intDigit < 10) {
                charDigit = intDigit + 48;
            } else {
                if(upper) {
                    charDigit = intDigit + 55;
                } else {
                    charDigit = intDigit + 87;
                }
            }
            result.push_back(charDigit);
        } while(innerValue != 0);
        std::reverse(result.begin(), result.end());
    }
    if(this->m_sign) {
        result.insert(result.begin(), '-');
    }
    return result;
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
    for(size_t j = 0; j < shift; j++) {
        uint8_t carry = 0;
        for(long long i = result.size() - 1; i >= 0; --i) {
            uint8_t current = result.m_digits[i] * 2 + carry;
            result.m_digits[i] = current % 10;
            carry = current / 10;
        }
        if(carry > 0) {
            result.m_digits.insert(result.m_digits.begin(), carry);
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
    for(size_t j = 0; j < shift; j++) {
        uint8_t carry = 0;
        for(size_t i = 0; i < result.size(); i++) {
            uint8_t current = carry * 10 + result.m_digits[i];
            result.m_digits[i] = current / 2;
            carry = current % 2;
        }
        while(result.size() > 1 && result.m_digits[0] == 0) {
            result.m_digits.erase(result.m_digits.begin());
        }
    }
    return result;
}

BigInt BigInt::shlDigitsBase(const size_t shift) const
{
    BigInt result(*this);
    result.m_digits.resize(result.size() + shift, 0);
    return result;
}

BigInt BigInt::shrDigitsBase(const size_t shift) const
{
    if(shift >= this->size()) {
        return 0;
    }
    BigInt result(*this);
    result.m_digits.resize(result.size() - shift, 0);
    return result;
}

BigInt BigInt::orBase(const BigInt &value) const
{
    if(abs(*this) <= LLONG_MAX && abs(value) <= LLONG_MAX) {
        return this->toLongLong() | value.toLongLong();
    } else {
        std::string strValue_1(this->toString(2));
        std::string strValue_2(value.toString(2));
        std::string strResult;
        bool resultSign = this->m_sign | value.m_sign;
        if(this->m_sign) {
            strValue_1.erase(strValue_1.begin());
        }
        if(value.m_sign) {
            strValue_2.erase(strValue_2.begin());
        }
        if(strValue_1.length() > strValue_2.length()) {
            strValue_2.insert(strValue_2.begin(), strValue_1.length() - strValue_2.length(), '0');
        } else if(strValue_1.length() < strValue_2.length()) {
            strValue_1.insert(strValue_1.begin(), strValue_2.length() - strValue_1.length(), '0');
        }
        if(this->m_sign) {
            strValue_1 = binstrToAddition(strValue_1);
        }
        if(value.m_sign) {
            strValue_2 = binstrToAddition(strValue_2);
        }
        if(strValue_1.length() > strValue_2.length()) {
            strValue_2.insert(strValue_2.begin(), strValue_1.length() - strValue_2.length(), '0');
        } else if(strValue_1.length() < strValue_2.length()) {
            strValue_1.insert(strValue_1.begin(), strValue_2.length() - strValue_1.length(), '0');
        }
        for(size_t i = 0; i < strValue_1.length(); i++) {
            strResult.push_back(((strValue_1[i] - '0') | (strValue_2[i] - '0')) + '0');
        }
        if(resultSign) {
            strResult = binstrToAddition(strResult);
            strResult.insert(strResult.begin(), '-');
        }
        return strToBigInt(strResult, 2);
    }
}

BigInt BigInt::andBase(const BigInt &value) const
{
    if(abs(*this) <= LLONG_MAX && abs(value) <= LLONG_MAX) {
        return this->toLongLong() & value.toLongLong();
    } else {
        std::string strValue_1(this->toString(2));
        std::string strValue_2(value.toString(2));
        std::string strResult;
        bool resultSign = this->m_sign & value.m_sign;
        if(this->m_sign) {
            strValue_1.erase(strValue_1.begin());
        }
        if(value.m_sign) {
            strValue_2.erase(strValue_2.begin());
        }
        if(strValue_1.length() > strValue_2.length()) {
            strValue_2.insert(strValue_2.begin(), strValue_1.length() - strValue_2.length(), '0');
        } else if(strValue_1.length() < strValue_2.length()) {
            strValue_1.insert(strValue_1.begin(), strValue_2.length() - strValue_1.length(), '0');
        }
        if(this->m_sign) {
            strValue_1 = binstrToAddition(strValue_1);
        }
        if(value.m_sign) {
            strValue_2 = binstrToAddition(strValue_2);
        }
        if(strValue_1.length() > strValue_2.length()) {
            strValue_2.insert(strValue_2.begin(), strValue_1.length() - strValue_2.length(), '0');
        } else if(strValue_1.length() < strValue_2.length()) {
            strValue_1.insert(strValue_1.begin(), strValue_2.length() - strValue_1.length(), '0');
        }
        for(size_t i = 0; i < strValue_1.length(); i++) {
            strResult.push_back(((strValue_1[i] - '0') & (strValue_2[i] - '0')) + '0');
        }
        if(resultSign) {
            strResult = binstrToAddition(strResult);
            strResult.insert(strResult.begin(), '-');
        }
        return strToBigInt(strResult, 2);
    }
}

BigInt BigInt::xorBase(const BigInt &value) const
{
    if(abs(*this) <= LLONG_MAX && abs(value) <= LLONG_MAX) {
        return this->toLongLong() ^ value.toLongLong();
    } else {
        std::string strValue_1(this->toString(2));
        std::string strValue_2(value.toString(2));void setMulMethod(MulMethod mul_mwthod);
        std::string strResult;
        bool resultSign = this->m_sign ^ value.m_sign;
        if(this->m_sign) {
            strValue_1.erase(strValue_1.begin());
        }
        if(value.m_sign) {
            strValue_2.erase(strValue_2.begin());
        }
        if(strValue_1.length() > strValue_2.length()) {
            strValue_2.insert(strValue_2.begin(), strValue_1.length() - strValue_2.length(), '0');
        } else if(strValue_1.length() < strValue_2.length()) {
            strValue_1.insert(strValue_1.begin(), strValue_2.length() - strValue_1.length(), '0');
        }
        if(this->m_sign) {
            strValue_1 = binstrToAddition(strValue_1);
        }
        if(value.m_sign) {
            strValue_2 = binstrToAddition(strValue_2);
        }
        if(strValue_1.length() > strValue_2.length()) {
            strValue_2.insert(strValue_2.begin(), strValue_1.length() - strValue_2.length(), '0');
        } else if(strValue_1.length() < strValue_2.length()) {
            strValue_1.insert(strValue_1.begin(), strValue_2.length() - strValue_1.length(), '0');
        }
        for(size_t i = 0; i < strValue_1.length(); i++) {
            strResult.push_back(((strValue_1[i] - '0') ^ (strValue_2[i] - '0')) + '0');
        }
        if(resultSign) {
            strResult = binstrToAddition(strResult);
            strResult.insert(strResult.begin(), '-');
        }
        return strToBigInt(strResult, 2);
    }
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

BigInt BigInt::addBase(const BigInt &value) const
{
    if(*this == 0 && value == 0) {
        return 0;
    }
    size_t resultSize = std::max(this->size(), value.size()) + 1;
    if(resultSize <= LLONG_SIZE) {
        return this->toLongLong() + value.toLongLong();
    }
    BigInt result;
    if(this->m_sign == value.m_sign) {
        digit_t digitRight, digitLeft, carry = 0;
        std::vector<digit_t> digitsRight = this->m_digits;
        std::vector<digit_t> digitsLeft = value.m_digits;
        result.m_digits.resize(resultSize);
        result.m_sign = this->m_sign;
        for(size_t i = 0; i < resultSize; i++) {

            digitsRight.size() >= i + 1 ? digitRight = digitsRight[digitsRight.size() - (i + 1)] : digitRight = 0;
            digitsLeft.size() >= i + 1 ? digitLeft = digitsLeft[digitsLeft.size() - (i + 1)] : digitLeft = 0;
            result.m_digits[resultSize - (i + 1)] = (digitRight + digitLeft + carry) % 10;
            carry = (digitRight + digitLeft + carry) / 10;
        }
    } else {
        result = this->m_sign ? value.subBase(-*this) : this->subBase(-value);
    }
    result.normalize();
    return result;
}

BigInt BigInt::subBase(const BigInt &value) const
{
    if(*this == value) {
        return 0;
    }
    size_t resultSize = std::max(this->size(), value.size()) + 1;
    if(resultSize <= LLONG_SIZE) {
        return this->toLongLong() - value.toLongLong();
    }
    BigInt result;
    if(this->m_sign == value.m_sign) {
        int digitRight, digitLeft, loan = 0;
        std::vector<digit_t> digitsRight;
        std::vector<digit_t> digitsLeft;
        result.m_digits.resize(resultSize);
        if(!this->m_sign && !value.m_sign)  {
            if(*this < value) {
                result.m_sign = true;
                digitsRight = this->m_digits;
                digitsLeft = value.m_digits;
            } else {
                result.m_sign = false;
                digitsRight = value.m_digits;
                digitsLeft = this->m_digits;
            }
        }
        if(this->m_sign && value.m_sign) {
            if(*this < value) {
                result.m_sign = true;
                digitsRight = this->m_digits;
                digitsLeft = value.m_digits;
            } else {
                result.m_sign = false;
                digitsRight = value.m_digits;
                digitsLeft = this->m_digits;
            }
        }
        for(size_t i = 0; i < resultSize - 1; i++) {
            digitLeft = digitsLeft[digitsLeft.size() - (i + 1)] - loan;
            digitsRight.size() >= i + 1 ? digitRight = digitsRight[digitsRight.size() - (i + 1)] : digitRight = 0;
            digitLeft < digitRight ? loan = 1 : loan = 0;
            result.m_digits[resultSize - (i + 1)] = (digitLeft + 10 * loan) - digitRight;
        }
    } else {
        result = this->addBase(-value);
    }
    result.normalize();
    return result;
}

BigInt BigInt::mulNative(const BigInt &value) const
{
    size_t digitsLeftSize = this->size();
    size_t digitsRightSize = value.size();
    size_t resultSize = digitsLeftSize + digitsRightSize + 1;
    BigInt result;
    result.m_digits.resize(resultSize);
    result.m_sign = this->m_sign ^ value.m_sign;
    std::vector<digit_t> digitsRight(resultSize), digitsLeft(resultSize);
    for(size_t i = 0; i < resultSize; i++) {
        digitsLeft[i] = i < digitsLeftSize ? this->m_digits[digitsLeftSize - 1 - i] : 0;
        digitsRight[i] = i < digitsRightSize ? value.m_digits[digitsRightSize - 1 - i] : 0;
    }
    for(size_t i = 0; i < digitsLeftSize; i++) {
        for(size_t j = 0; j < digitsRightSize; j++) {
            result.m_digits[resultSize - 1 - (i + j)] += digitsLeft[i] * digitsRight[j];
            result.m_digits[resultSize - 1 - (i + j + 1)] += result.m_digits[resultSize - 1 - (i + j)] / 10;
            result.m_digits[resultSize - 1 - (i + j)] %= 10;
        }
    }
    result.normalize();
    return result;
}

BigInt BigInt::mulKaratsuba(const BigInt &value) const
{
    bool resultSign = this->m_sign ^ value.m_sign;
    std::vector<digit_t> digits_1 = this->m_digits;
    std::vector<digit_t> digits_2 = value.m_digits;
    BigInt value_1 = *this;
    BigInt value_2 = value;
    size_t sizeDigits_1 = digits_1.size();
    size_t sizeDigits_2 = digits_2.size();
    size_t maxSize = std::max(sizeDigits_1, sizeDigits_2);
    if(sizeDigits_1 == 1 || sizeDigits_2 == 1) {
            return value_1.mulNative(value_2);
    }
    maxSize += maxSize % 2;
    size_t halfSize = maxSize / 2;
    BigInt value_1_right;
    sizeDigits_1 > halfSize ? value_1_right.m_digits.assign(digits_1.end() - halfSize, digits_1.end()) :
                                value_1_right.m_digits.assign(digits_1.begin(), digits_1.end());
     BigInt value_2_right;
     sizeDigits_2 > halfSize ? value_2_right.m_digits.assign(digits_2.end() - halfSize, digits_2.end()) :
                                 value_2_right.m_digits.assign(digits_2.begin(), digits_2.end());
     BigInt value_1_left(value_1.shrDigitsBase(halfSize));
     BigInt value_2_left(value_2.shrDigitsBase(halfSize));
     BigInt P1 = value_1_left.mulKaratsuba(value_2_left);
     BigInt P2 = value_1_right.mulKaratsuba(value_2_right);
     BigInt P3 = value_1_left.addBase(value_1_right).mulKaratsuba(value_2_left.addBase(value_2_right));
     BigInt result = P1.shlDigitsBase(maxSize).addBase((P3.subBase(P2).subBase(P1).shlDigitsBase(halfSize)).addBase(P2));
     result.m_sign = resultSign;
     return result;
}

BigInt BigInt::mulFft(const BigInt &value) const
{
    BigInt result;
    result.m_digits.clear();
    result.m_sign = this->m_sign ^ value.m_sign;
    std::vector<std::complex<double>> complexValue_1(this->m_digits.begin(), this->m_digits.end()),
            complexValue_2(value.m_digits.begin(), value.m_digits.end());
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
        carry = resultDigit / 10;
        resultDigit = resultDigit % 10;
        result.m_digits.push_back(resultDigit);
    }
    result.m_digits.push_back(carry);
    std::reverse(result.m_digits.begin(), result.m_digits.end());
    result.normalize();
    return result;
}

BigInt BigInt::mulBase(const BigInt &value) const
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
    if(this->size() + value.size() + 1 <= LLONG_SIZE) {
        return this->toLongLong() * value.toLongLong();
    }
    if(this->size() + value.size() + 1 > 20000) {
        if(m_mulMethod == MulMethod::Karatsuba) {
            return this->mulKaratsuba(value);
        } else if(m_mulMethod == MulMethod::FastFourierTransform) {
            return this->mulFft(value);
        }
    }
    return this->mulNative(value);
}

BigInt BigInt::divNative(const BigInt &value) const
{
    BigInt divident(abs(*this));
    BigInt divider(abs(value));
    BigInt result;
    BigInt subResult;
    size_t dividentSize = divident.size();
    size_t index = 0;
    long long llDivider = divider > LONG_LONG_MAX ? 0 : divider.toLongLong();
    while(subResult < divider && index < dividentSize) {
        subResult.m_digits.push_back(divident.m_digits[index++]);
    }
    subResult.normalize();
    do {
        int count = 0;
        if (divider > LONG_LONG_MAX) {
            BigInt mod = subResult;
            while(mod >= divider) {
                mod = mod.subBase(divider);
                count++;
            }
            subResult = mod;
        } else {
            long long mod = subResult.toLongLong();
            count = mod / llDivider;
            subResult = mod % llDivider;
        }
        result.m_digits.push_back(count);
        if(index <= dividentSize) {
            subResult.m_digits.push_back(divident.m_digits[index++]);
        }
    } while(index <= dividentSize);
    result.m_sign = this->m_sign ^ value.m_sign;
    result.normalize();
    return result;
}

BigInt BigInt::divNewtonRaphson(const BigInt &value) const
{
    BigInt divident(*this);
    BigInt divider(value);
    size_t shift = (divident.size() + divider.size());
    BigInt powBase_10(2);
    powBase_10 = powBase_10.shlDigitsBase(shift);
    BigInt current = divident.subBase(divider);
    BigInt last;
    while(last != current) {
        last = current;
        current = (current.mulBase(powBase_10.subBase(current.mulBase(divider)))).shrDigitsBase(shift);
    }
    BigInt result = divident.mulBase(current);
    result = result.shrDigitsBase(shift);
    BigInt remainder = divident.subBase(result.mulBase(divider));
    if(remainder >= divider) {
        result = result.addBase(1);
    }
    result.m_sign = this->m_sign ^ value.m_sign;
    return result;
}

BigInt BigInt::divBase(const BigInt &value) const
{
    if(value == 0) {
        throw BigIntException("Divizion by zero",
                              ExceptionCode::DivisionByZero);
    }
    if(value > *this) {
        return 0;
    }
    if(value == *this) {
        return 1;
    }
    if(value == 1) {
        return *this;
    }
    size_t numZero = 0;
    while(value.m_digits[numZero] == 0) {
        numZero++;
    }
    if(abs(*this) <= LLONG_MAX && abs(value) <= LLONG_MAX) {
        return this->shrDigitsBase(numZero).toLongLong() / value.shrDigitsBase(numZero).toLongLong();
    } else if(this->size() < 1000 && value.size() < 1000) {
        return this->shrDigitsBase(numZero).divNative(value.shrDigitsBase(numZero));
    } else {
        return this->shrDigitsBase(numZero).divNewtonRaphson(value.shrDigitsBase(numZero));
    }
}

BigInt BigInt::remBase(const BigInt &value) const
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
    if(this->size() * exp + exp <= LLONG_SIZE) {
        return static_cast<long long>(std::pow(this->toLongLong(), exp));
    }
    BigInt result;
    if(exp & 1) {
        result = this->powBase(exp - 1);
        return result.mulBase(*this);
    } else {
        result = this->powBase(exp / 2);
        return result.mulBase(result);
    }
}

BigInt BigInt::powBase(const BigInt &exp) const
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
    if(BigInt(this->size()).mulBase(exp).addBase(exp) <= LLONG_SIZE) {
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

BigInt BigInt::powModBase(const long long exp, const BigInt &mod) const
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
    if(this->size() * exp + exp <= LLONG_SIZE && abs(mod) <= LLONG_MAX) {
        return static_cast<long long>(std::pow(this->toLongLong(), exp)) % mod.toLongLong();
    }
    BigInt result;
    if(exp & 1) {
        result = this->powModBase(exp - 1, mod);
        return result.mulBase(*this).remBase(mod);
    } else {
        result = this->powModBase(exp / 2, mod);
        return result.mulBase(result).remBase(mod);
    }
}

BigInt BigInt::powModBase(const BigInt &exp, const BigInt &mod) const
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
    if(BigInt(this->size()).mulBase(exp).addBase(exp) <= LLONG_SIZE && abs(mod) <= LLONG_MAX) {
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
    if(this->m_sign && (exp & 1) == 0) {
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
    size_t resultSize = (this->size() + 1) / 2;
    size_t index = 0;
    BigInt result;
    result.m_digits.resize(resultSize);
    while(index < resultSize) {
        result.m_digits[index] = 9;
        while(result.powBase(exp) > abs(*this) && result.m_digits[index] > 0) {
            result.m_digits[index]--;
        }
        index++;
    }
    result.normalize();
    result.m_sign = this->m_sign;
    return result;
}

BigInt BigInt::rootBase(const BigInt &exp) const
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
    size_t resultSize = (this->size() + 1) / 2;
    size_t index = 0;
    BigInt result;
    result.m_digits.resize(resultSize);
    while(index < resultSize) {
        result.m_digits[index] = 9;

        while(result.powBase(exp) > abs(*this) && result.m_digits[index] > 0) {
            result.m_digits[index]--;
        }
        index++;
    }
    result.normalize();
    result.m_sign = this->m_sign;
    return result;
}

void BigInt::normalize()
{
    while(this->m_digits[0] == 0 && this->size() > 1) {
        this->m_digits.erase(this->m_digits.begin());
    }
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

BigInt BigInt::operator - () const &
{
    BigInt result(*this);
    result.m_sign = !this->m_sign;
    return result;
}

BigInt BigInt::operator + () const &
{
    return *this;
}

bool BigInt::operator == (const BigInt &other) const
{
    return (this->m_digits == other.m_digits) && (this->m_sign == other.m_sign);
}

bool BigInt::operator != (const BigInt &other) const
{
    return !(*this == other);
}

bool BigInt::operator < (const BigInt &other) const
{
    size_t digitsLeftSize = this->size();
    size_t digitsRightSize = other.size();
    if(this->m_sign == other.m_sign) {
        if(digitsLeftSize != digitsRightSize) {
            return (digitsLeftSize < digitsRightSize) ^ this->m_sign;
        }
        return (this->m_digits < other.m_digits) ^ this->m_sign;
    }
    return this->m_sign;
}

bool BigInt::operator > (const BigInt &other) const
{
    return !(*this < other || *this == other);
}

bool BigInt::operator >= (const BigInt &other) const
{
    return *this > other || *this == other;
}

bool BigInt::operator <= (const BigInt &other) const
{
    return *this < other || *this == other;
}

bool BigInt::operator == (const long long other) const
{
    return (this->m_digits == BigInt(other).m_digits) && (this->m_sign == BigInt(other).m_sign);
}

bool BigInt::operator != (const long long other) const
{
    return !(*this == BigInt(other));
}

bool BigInt::operator < (const long long other) const
{
    size_t digitsLeftSize = this->size();
    size_t digitsRightSize = BigInt(other).size();
    if(this->m_sign == BigInt(other).m_sign) {
        if(digitsLeftSize != digitsRightSize) {
            return (digitsLeftSize < digitsRightSize) ^ this->m_sign;
        }
        return (this->m_digits < BigInt(other).m_digits) ^ this->m_sign;
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

BigInt BigInt::operator << (const unsigned long long shift) const
{
    return this->shlBitsBase(shift);
}

BigInt BigInt::operator >> (const unsigned long long shift) const
{
    return this->shrBitsBase(shift);
}

BigInt BigInt::operator + (const BigInt &other) const
{
    return this->addBase(other);
}

BigInt BigInt::operator - (const BigInt &other) const
{
    return this->subBase(other);
}

BigInt BigInt::operator * (const BigInt &other) const
{
    return this->mulBase(other);
}

BigInt BigInt::operator / (const BigInt &other) const
{
    return this->divBase(other);
}

BigInt BigInt::operator % (const BigInt &other) const
{
    return this->remBase(other);
}

BigInt BigInt::operator | (const BigInt &other) const
{
    return this->orBase(other);
}

BigInt BigInt::operator & (const BigInt &other) const
{
    return this->andBase(other);
}

BigInt BigInt::operator ^ (const BigInt &other) const
{
    return this->xorBase(other);
}

BigInt BigInt::operator ~ () const &
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

BigInt BigInt::operator & (const long long other) const
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

BigInt& BigInt::operator += (const BigInt &other)
{
    *this = this->addBase(other);
    return *this;
}

BigInt& BigInt::operator -= (const BigInt &other)
{
    *this = this->subBase(other);
    return *this;
}

BigInt& BigInt::operator *= (const BigInt &other)
{
    *this = this->mulBase(other);
    return *this;
}

BigInt& BigInt::operator /= (const BigInt &other)
{
    *this = this->divBase(other);
    return *this;
}

BigInt& BigInt::operator %=(const BigInt &other)
{
    *this = this->remBase(other);
    return *this;
}

BigInt& BigInt::operator |=(const BigInt &other)
{
    *this = this->orBase(other);
    return *this;
}

BigInt& BigInt::operator &=(const BigInt &other)
{
    *this = this->andBase(other);
    return *this;
}

BigInt& BigInt::operator ^=(const BigInt &other)
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

BigInt& BigInt::operator %=(const long long other)
{
    *this = this->remBase(other);
    return *this;
}

BigInt& BigInt::operator |=(const long long other)
{
    *this = this->orBase(other);
    return *this;
}

BigInt& BigInt::operator &=(const long long other)
{
    *this = this->andBase(other);
    return *this;
}

BigInt& BigInt::operator ^=(const long long other)
{
    *this = this->xorBase(other);
    return *this;
}

BigInt strToBigInt(const std::string &strValue, const size_t radix)
{
    return BigInt(strValue, radix);
}

BigInt strToBigInt(const char *strValue, size_t radix)
{
    return strToBigInt(std::string(strValue), radix);
}

std::ostream& operator << (std::ostream &out, const BigInt &value)
{
    std::ios_base::fmtflags baseFlag = out.flags() &
            (std::ios_base::hex | std::ios_base::dec | std::ios_base::oct);
    std::ios_base::fmtflags showBaseFlag = out.flags() & (std::ios_base::showbase);
    std::ios_base::fmtflags showPosFlag = out.flags() & (std::ios_base::showpos);
    std::ios_base::fmtflags uppercaseFlag = out.flags() & (std::ios_base::uppercase);
    size_t width = out.width();
    char fillChar = out.fill();
    std::string result;
    switch(baseFlag) {
    case std::ios_base::hex:
        result = value.toString(16, static_cast<bool>(uppercaseFlag));
        if(showBaseFlag) {
            result.insert(result.begin(), 'x');
            result.insert(result.begin(), '0');
        }
        break;
    case std::ios_base::dec:
        result = value.toString(10);
        break;
    case std::ios_base::oct:
        if(showBaseFlag) {
            result = value.toString(8);
            result.insert(result.begin(), '0');
        }
        break;
    default:
        result = value.toString(10);
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

std::istream& operator >> (std::istream &in, BigInt &value)
{
    std::ios_base::fmtflags baseFlag = std::cin.flags() &
            (std::ios_base::hex | std::ios_base::dec | std::ios_base::oct);
    std::string strValue;
    in >> strValue;
    switch(baseFlag) {
    case std::ios_base::hex:
        value = strToBigInt(strValue, 16);
        if((strValue[0] == '+' || strValue[0] == '-') &&
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
        value = strToBigInt(strValue, 10);
        if((strValue[0] == '+' || strValue[0] == '-') &&
                ((isdigit(strValue[1]) && strValue[1] - 48 <= 10))) {
            in.clear(std::ios::goodbit);
        } else if(isdigit(strValue[0]) && strValue[1] - 48 <= 10) {
            in.clear(std::ios::goodbit);
        } else {
            in.clear(std::ios::failbit);
        }
        break;
    case std::ios_base::oct:
        value = strToBigInt(strValue, 8);
        if((strValue[0] == '+' || strValue[0] == '-') &&
                ((isdigit(strValue[1]) && strValue[1] - 48 <= 8))) {
            in.clear(std::ios::goodbit);
        } else if(isdigit(strValue[0]) && strValue[1] - 48 <= 8) {
            in.clear(std::ios::goodbit);
        } else {
            in.clear(std::ios::failbit);
        }
        break;
    default:
        if((strValue[0] == '+' || strValue[0] == '-') &&
                ((isdigit(strValue[1]) && strValue[1] - 48 <= 10))) {
            in.clear(std::ios::goodbit);
        } else if(isdigit(strValue[0]) && strValue[1] - 48 <= 10) {
            in.clear(std::ios::goodbit);
        } else {
            in.clear(std::ios::failbit);
        }
        value = strToBigInt(strValue, 10);
        break;
    }
    return in;
}

BigInt shr(const BigInt &value, const size_t shift)
{
    return value.shrBitsBase(shift);
}

BigInt shl(const BigInt &value, const size_t shift)
{
    return value.shlBitsBase(shift);
}

BigInt sqr(const BigInt &value)
{
    return value.powBase(2);
}

BigInt pow(const BigInt &value, const long long exp)
{
    return value.powBase(exp);
}

BigInt pow(const BigInt &value, const BigInt exp)
{
    return value.powBase(exp);
}

BigInt powMod(const BigInt &value, const long long exp, const BigInt &mod)
{
    return value.powModBase(exp, mod);
}

BigInt powMod(const BigInt &value, const BigInt exp, const BigInt &mod)
{
    return value.powModBase(exp, mod);
}

BigInt sqrt(const BigInt &value)
{
    return value.rootBase(2);
}

BigInt root(const BigInt &value, const long long exp)
{
    return value.rootBase(exp);
}

BigInt root(const BigInt &value, const BigInt &exp)
{
    return value.rootBase(exp);
}

BigInt abs(const BigInt &value)
{
   if(value.m_sign) {
       return value.mulBase(-1);
   }
   return value;
}

BigInt gcd(const BigInt &value_1, const BigInt &value_2)
{
    BigInt innerValue_1 = std::max(value_1, value_2);
    BigInt innerValue_2 = std::min(value_1, value_2);
    while(innerValue_2 > 0) {
        innerValue_1 = innerValue_1.remBase(innerValue_2);
        std::swap(innerValue_1, innerValue_2);
    }
    return innerValue_1;
}

BigInt gcdExt(const BigInt &value_1, const BigInt &value_2,
               BigInt &factor_1, BigInt &factor_2)
{
    BigInt innerValue_1 = std::max(value_1, value_2);
    BigInt innerValue_2 = std::min(value_1, value_2);
    if(innerValue_2 == 0) {
        factor_1 = 1;
        factor_2 = 0;
        return innerValue_1;
    }
    BigInt q, r, x_1, x_2(1), y_1(1), y_2;
    while(innerValue_2 > 0) {
        q = innerValue_1.divBase(innerValue_2);
        r = innerValue_1.subBase(q.mulBase(innerValue_2));
        factor_1 = x_2.subBase(q.mulBase(x_1));
        factor_2 = y_2.subBase(q.mulBase(y_1));
        innerValue_1 = innerValue_2;
        innerValue_2 = r;
        x_2 = x_1;
        x_1 = factor_1;
        y_2 = y_1;
        y_1 = factor_2;
    }
    factor_1 = x_2;
    factor_2 = y_2;
    return innerValue_1;
}

int getLegendreSymbol(const BigInt &aValue, const BigInt &pValue)
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

int getJacobiSymbol(const BigInt &aValue, const BigInt &pValue)
{
    if(pValue.isEven() || pValue < 1) {
        throw BigIntException("Invalid value of function parameters",
                               ExceptionCode::InvalidValue);
    }
    if(pValue == 1) {
        return 1;
    }
    BigInt innerAValue(aValue);
    BigInt innerPValue(pValue);
    if(gcd(innerAValue, innerPValue) != 1) {
        return 0;
    }
    int result = 1;
    if(innerAValue < 0) {
        innerAValue = -innerAValue;
        if(innerPValue.remBase(4) == 3) {
            result = -result;
        }
    }
    while(innerAValue != 0) {
        BigInt tempValue_1(0);
        while(innerAValue.isEven()) {
            tempValue_1 = tempValue_1.addBase(1);
            innerAValue = innerAValue.divBase(2);
        }
        if(!tempValue_1.isEven() && ((innerPValue.remBase(8) == 3) || (innerPValue.remBase(8) == 5))) {
            result = -result;
        }
        if((innerAValue.remBase(4) == innerPValue.remBase(4)) && (innerPValue.remBase(4) == 3)) {
            result = -result;
        }
        BigInt tempValue_2 = innerAValue;
        innerAValue = innerPValue.remBase(tempValue_2);
        innerPValue = tempValue_2;
    }
    return result;
}
