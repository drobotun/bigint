# BigInt Library

[![License: MIT](https://img.shields.io/github/license/drobotun/virustotalapi3?style=flat)](https://opensource.org/licenses/MIT)
![Version](https://img.shields.io/badge/Release-0.1.0-blue)

A C++ library for performing arithmetic and logical operations on long (arbitrary width) integers.

Copyright (c) 2025 Evgeny Drobotun, Anna Boykova

## Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Usage Examples](#usage-examples)
- [API Reference](#api-reference)
- [Error Handling](#error-handling)
- [Performance Considerations](#performance-considerations)
- [Constants and Enums](#constants-and-enums)
- [License](#license)
- [Contact](#contact)

## Overview

The `BigInt` library implements arithmetic and logical operations for integers of arbitrary length, overcoming the limitations of standard fixed-size integer types. It supports basic arithmetic, comparisons, bitwise operations, and advanced mathematical functions like modular exponentiation and root extraction.

## Features

### Arithmetic Operations
- **Addition** (`+`)
- **Subtraction** (`-`)
- **Multiplication** (`*`)
  - Native method (for numbers < 20,000 bytes)
  - Karatsuba method (for numbers ≥ 20,000 bytes, **default**)
  - Fast Fourier Transform (FFT) method (for numbers ≥ 20,000 bytes)
- **Division** (`/`)
  - Native method (for numbers < 20,000 bytes)
  - Newton-Raphson method (for numbers ≥ 20,000 bytes)
- **Remainder** (`%`)
  - Native method (for numbers < 20,000 bytes)
  - Newton-Raphson method (for numbers ≥ 20,000 bytes)
- **Increment** (`++`, prefix and postfix)
- **Decrement** (`--`, prefix and postfix)

### Comparison Operations
- Equal (`==`)
- Not equal (`!=`)
- Greater than (`>`)
- Less than (`<`)
- Greater than or equal (`>=`)
- Less than or equal (`<=`)

### Logical & Bitwise Operations
- Bitwise AND (`&`)
- Bitwise OR (`|`)
- Bitwise XOR (`^`)
- Bitwise NOT (`~`)
- Left shift (`<<`)
- Right shift (`>>`)

### Input/Output
- Overloaded stream extraction (`>>`) and insertion (`<<`) operators supporting decimal, hexadecimal, and octal formats with manipulators (`std::hex`, `std::oct`, `std::showbase`, etc.)

### Mathematical Functions
- `sqr()` - Square a number
- `pow()` - Exponentiation (binary exponentiation algorithm)
- `powMod()` - Modular exponentiation
- `sqrt()` - Integer square root
- `root()` - Integer n-th root
- `abs()` - Absolute value
- `gcd()` - Greatest common divisor (Euclidean algorithm)
- `gcdExt()` - Extended Euclidean algorithm
- `getLegendreSymbol()` - Legendre symbol
- `getJacobiSymbol()` - Jacobi symbol

## Installation

To use the library in your project:

1. Copy the files `bigint.h` and `bigint.cpp` into your project directory
2. Include the header file where needed `bigint.h`
3. Compile your project together with `bigint.cpp`

No external dependencies are required.

## Usage Examples

### Basic Arithmetic

```cpp

#include "bigint.h"
#include <iostream>

int main() {
    BigInt a = "12345678901234567890";
    BigInt b = "98765432109876543210";

    BigInt sum = a + b;
    BigInt difference = a - b;
    BigInt product = a * b;
    BigInt quotient = a / b;
    BigInt remainder = a % b;

    std::cout << "Sum: " << sum << std::endl;
    std::cout << "Difference: " << difference << std::endl;
    std::cout << "Product: " << product << std::endl;
    std::cout << "Quotient: " << quotient << std::endl;
    std::cout << "Remainder: " << remainder << std::endl;

    return 0;
}
```

### Bitwise Operations

```cpp
#include "bigint.h"
#include <iostream>

int main() {
    BigInt x = 0b1010;  // 10 in decimal
    BigInt y = 0b1100;  // 12 in decimal

    BigInt and_result = x & y;   // 0b1000 (8)
    BigInt or_result  = x | y;   // 0b1110 (14)
    BigInt xor_result = x ^ y;   // 0b0110 (6)
    BigInt not_result = ~x;      // Bitwise NOT
    BigInt left_shift = x << 2;  // 0b101000 (40)
    BigInt right_shift = x >> 1; // 0b101 (5)

    std::cout << "x & y = " << and_result << std::endl;
    std::cout << "x | y = " << or_result << std::endl;
    std::cout << "x ^ y = " << xor_result << std::endl;
    std::cout << "~x = " << not_result << std::endl;
    std::cout << "x << 2 = " << left_shift << std::endl;
    std::cout << "x >> 1 = " << right_shift << std::endl;

    return 0;
}
```

### Advanced Mathematics

```cpp
#include "bigint.h"
#include <iostream>

int main() {
    // Modular exponentiation
    BigInt base = "2";
    BigInt exponent = "100";
    BigInt modulus = "1000000007";

    BigInt pow_mod_result = powMod(base, exponent, modulus);
    std::cout << "2^100 mod 1000000007 = " << pow_mod_result << std::endl;

    // Square root
    BigInt n = "144";
    BigInt sqrt_n = sqrt(n);
    std::cout << "Square root of 144 = " << sqrt_n << std::endl;

    // n-th root
    BigInt num = "32768";
    BigInt fifth_root = root(num, 5);
    std::cout << "5th root of 32768 = " << fifth_root << std::endl;

    // Power
    BigInt power_result = pow(BigInt("3"), 10);
    std::cout << "3^10 = " << power_result << std::endl;

    // Square
    BigInt square_result = sqr(BigInt("12345"));
    std::cout << "12345^2 = " << square_result << std::endl;

    // Absolute value
    BigInt negative = "-12345";
    BigInt absolute = abs(negative);
    std::cout << "|" << negative << "| = " << absolute << std::endl;

    return 0;
}
```

### Switching Multiplication Methods

```cpp
#include "bigint.h"
#include <iostream>
#include <chrono>

int main() {
    // large1 and large2 must be greater than 20000 digits
    BigInt large1("1234567890123456789012345678901234567890...1234567890123456789012345678901234567890");
    BigInt large2("9876543210987654321098765432109876543210...9876543210987654321098765432109876543210");

    auto start = std::chrono::high_resolution_clock::now();
    BigInt karatsuba_product = large1 * large2;
    auto end = std::chrono::high_resolution_clock::now();
    auto karatsuba_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Karatsuba time: " << karatsuba_time.count() << " microseconds" << std::endl;

    large1.setMulMethod(MulMethod::FastFourierTransform);
    start = std::chrono::high_resolution_clock::now();
    BigInt fft_product = large1 * large2;
    end = std::chrono::high_resolution_clock::now();
    auto fft_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "FFT time: " << fft_time.count() << " microseconds" << std::endl;
    std::cout << "Results are equal: " << (karatsuba_product == fft_product) << std::endl;

    return 0;
}
```

### Input/Output with Manipulators

```cpp
#include "bigint.h"
#include <iostream>
#include <iomanip>

int main() {
    BigInt num = 255;

    std::cout << std::hex << std::showbase << "Hexadecimal: " << num << std::endl;
    std::cout << std::oct << "Octal: " << num << std::endl;
    std::cout << std::dec << std::noshowbase << "Decimal: " << num << std::endl;
    std::cout << std::hex << std::uppercase << std::showbase << "Uppercase hex: " << num << std::endl;
    std::cout << std::dec << std::setw(10) << std::setfill('0') << "Padded: " << num << std::endl;

    return 0;
}
```

### Number System Conversion

```cpp
#include "bigint.h"
#include <iostream>

int main() {
    BigInt hex_num("1A3F", Radix::Hex);
    BigInt bin_num("1101", Radix::Bin);
    BigInt oct_num("755", Radix::Oct);

    std::cout << "Hex 1A3F = Dec: " << hex_num.toDec() << ", Bin: " << hex_num.toBin() << std::endl;
    std::cout << "Bin 1101 = Dec: " << bin_num.toDec() << ", Hex: " << bin_num.toHex(true) << std::endl;
    std::cout << "Oct 755 = Dec: " << oct_num.toDec() << std::endl;

    return 0;
}
```

### Comparison Operations

```cpp
#include "bigint.h"
#include <iostream>

int main() {
    BigInt big1 = "1000", big2 = "999", big3 = "1000";

    std::cout << big1 << " > " << big2 << ": " << (big1 > big2) << std::endl;
    std::cout << big1 << " == " << big3 << ": " << (big1 == big3) << std::endl;
    std::cout << big1 << " != " << big2 << ": " << (big1 != big2) << std::endl;
    std::cout << big1 << " == 1000: " << (big1 == 1000) << std::endl;

    return 0;
}
```

### GCD and Extended GCD

```cpp
#include "bigint.h"
#include <iostream>

int main() {
    BigInt a = "48", b = "18", x, y;

    std::cout << "GCD(48, 18) = " << gcd(a, b) << std::endl;

    BigInt gcd_ext = gcdExt(a, b, x, y);
    std::cout << "Extended GCD: 48*" << x << " + 18*" << y << " = " << gcd_ext << std::endl;

    return 0;
}
```
### Legendre and Jacobi Symbols

```cpp
#include "bigint.h"
#include <iostream>

int main() {
    std::cout << "Legendre symbol (20000/70000) = " << getLegendreSymbol(20000, 70000) << std::endl;
    std::cout << "Jacobi symbol (506506/2102121021) = " << getJacobiSymbol(506506, 2102121021) << std::endl;
    std::cout << "Legendre symbol (3344/112233) = " << getLegendreSymbol(3344, 112233) << std::endl;

    return 0;
}
```

### Increment and Decrement Operations

```cpp
#include "bigint.h"
#include <iostream>

int main() {
    BigInt num = "1000000000000000000000000000000000000000";
    std::cout << "Original: " << num << std::endl;
    std::cout << "Prefix ++num: " << ++num << std::endl;
    std::cout << "Postfix num++: " << num++ << std::endl;
    std::cout << "After postfix: " << num << std::endl;

    BigInt neg = "-500000000000000000000000000000000000000";
    std::cout << "\nNegative: " << neg << ", ++neg: " << ++neg << std::endl;

    return 0;
}
```

### Shift Operations

```cpp
#include "bigint.h"
#include <iostream>

int main() {
    BigInt num = "12345123451234512345543215432154321";
    std::cout << "Original: " << num << " (bin: " << num.toBin() << ")" << std::endl;
    std::cout << "num << 1 = " << (num << 1) << " (multiply by 2)" << std::endl;
    std::cout << "num >> 1 = " << (num >> 1) << " (divide by 2)" << std::endl;

    BigInt compound = "256";
    compound <<= 2;
    std::cout << "256 <<= 2 = " << compound << std::endl;
    compound >>= 3;
    std::cout << ">>= 3 = " << compound << std::endl;

    return 0;
}
```

### Working with Very Large Numbers

```cpp
#include "bigint.h"
#include <iostream>

int main() {
    BigInt factorial = 1;
    for (int i = 1; i <= 1000000; ++i) {
        factorial *= i;
    }

    std::cout << "1000000! = " << factorial << std::endl;
    std::cout << "Number of digits: " << factorial.toString().length() << std::endl;
    std::cout << "Number of bits: " << factorial.numBits() << std::endl;

    BigInt power = pow(BigInt("2"), 1000);
    std::cout << "\n2^1000 has " << power.toString().length() << " digits" << std::endl;
    std::cout << "Is 2^1000 even? " << (power.isEven() ? "Yes" : "No") << std::endl;

    return 0;
}
```

### Error Handling Example

```cpp
#include "bigint.h"
#include <iostream>

int main() {
    try {
        BigInt result = BigInt("100") / BigInt("0");
    } catch (const BigIntException& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Error code: " << static_cast<int>(e.getExceptionCode()) << std::endl;
    }

    try {
        BigInt sqrt_result = sqrt(BigInt("-16"));
    } catch (const BigIntException& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        std::cerr << "Error code: " << static_cast<int>(e.getExceptionCode()) << std::endl;
    }

    return 0;
}
```

## API Reference

### Constructors

- BigInt() - Default constructor (value = 0)
- BigInt(int/long/long long/unsigned...) - From standard integer types
- BigInt(const std::string&, Radix = Radix::Dec) - From string with specified radix
- BigInt(const char*, Radix = Radix::Dec) - From C-string with specified radix
- BigInt(const BigInt&) - Copy constructor
- BigInt(BigInt&&) - Move constructor

### Core Methods

- size_t length() const - Returns number of bytes
- size_t numBits() const - Returns number of significant bits
- bool isEven() const - Checks if the number is even
- std::string toString(Radix = Radix::Dec, bool upper = false) const - Convert to string in specified base
- void setMulMethod(MulMethod) - Set multiplication algorithm (Karatsuba/FFT)

### Conversion Methods

- int toInt() const - Convert to int (throws on overflow)
- long toLong() const - Convert to long (throws on overflow)
- long long toLongLong() const - Convert to long long (throws on overflow)
- unsigned int toUInt() const - Convert to unsigned int
- unsigned long toULong() const - Convert to unsigned long
- unsigned long long toULongLong() const - Convert to unsigned long long
- std::string toHex(bool upper = false) const - Convert to hexadecimal string
- std::string toDec() const - Convert to decimal string
- std::string toOct() const - Convert to octal string
- std::string toBin() const - Convert to binary string

### Operators Overloaded

- Arithmetic: +, -, *, /, %
- Comparison: ==, !=, <, >, <=, >=
- Bitwise: &, |, ^, ~, <<, >>
- Assignment: =, +=, -=, *=, /=, %=, &=, |=, ^=, <<=, >>=
- Increment/Decrement: ++ (prefix/postfix), -- (prefix/postfix)
- Unary: +, -

### Friend Functions

- std::ostream& operator<<(std::ostream&, const BigInt&) - Stream output
- std::istream& operator>>(std::istream&, BigInt&) - Stream input
- BigInt shr(const BigInt&, size_t) - Right shift
- BigInt shl(const BigInt&, size_t) - Left shift
- BigInt sqr(const BigInt&) - Square
- BigInt pow(const BigInt&, long long) - Power
- BigInt pow(const BigInt&, const BigInt&) - Power
- BigInt powMod(const BigInt&, long long, const BigInt&) - Modular power
- BigInt powMod(const BigInt&, const BigInt&, const BigInt&) - Modular power
- BigInt sqrt(const BigInt&) - Square root
- BigInt root(const BigInt&, long long) - N-th root
- BigInt root(const BigInt&, const BigInt&) - N-th root
- BigInt abs(const BigInt&) - Absolute value
- BigInt gcd(const BigInt&, const BigInt&) - Greatest common divisor
- BigInt gcdExt(const BigInt&, const BigInt&, BigInt&, BigInt&) - Extended GCD
- int getLegendreSymbol(const BigInt&, const BigInt&) - Legendre symbol
- int getJacobiSymbol(const BigInt&, const BigInt&) - Jacobi symbol

## Error Handling

The library throws `BigIntException` with specific error codes:

| Exception Code | Description |
|----------------|-------------|
| `Unknown` | Unknown or unspecified error |
| `DivisionByZero` | Division or modulo by zero |
| `InvalidRootDegree` | Even root of a negative number |
| `InvalidValue` | Invalid parameter (e.g., negative exponent for `root()`) |
| `LengthError` | Conversion overflow or size limits exceeded |

Example of Error Handling:

```cpp
#include "bigint.h"
#include <iostream>

int main() {
    // Example 1: Division by zero
    try {
        BigInt a = "100";
        BigInt b = "0";
        BigInt result = a / b;
    } catch (const BigIntException& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        std::cerr << "Error code: " << static_cast<int>(e.getExceptionCode()) << std::endl;
    }

    // Example 2: Even root of negative number
    try {
        BigInt negative = "-16";
        BigInt sqrt_result = sqrt(negative);
    } catch (const BigIntException& e) {
        std::cerr << "\nCaught exception: " << e.what() << std::endl;
        std::cerr << "Error code: " << static_cast<int>(e.getExceptionCode()) << std::endl;
    }

    // Example 3: Conversion overflow
    try {
        BigInt large = "99999999999999999999";
        int value = large.toInt();  // Will throw if too large for int
    } catch (const BigIntException& e) {
        std::cerr << "\nCaught exception: " << e.what() << std::endl;
        std::cerr << "Error code: " << static_cast<int>(e.getExceptionCode()) << std::endl;
    }

    // Example 4: Invalid root degree
    try {
        BigInt num = "27";
        BigInt root_result = root(num, -2);  // Negative degree
    } catch (const BigIntException& e) {
        std::cerr << "\nCaught exception: " << e.what() << std::endl;
        std::cerr << "Error code: " << static_cast<int>(e.getExceptionCode()) << std::endl;
    }

    return 0;
}
```

## Performance Considerations

- **Small numbers** (< 64 bits): Operations fall back to native integer types for maximum performance
- **Medium numbers** (up to ~20,000 bytes): Native algorithms are used with good performance characteristics
- **Large numbers** (> 20,000 bytes):
    - Multiplication uses either Karatsuba (default) or FFT. FFT is asymptotically faster for extremely large numbers but has higher constant overhead
    - Division uses Newton-Raphson method for improved performance
- **Memory usage:** Numbers are stored as vectors of bytes, providing efficient memory utilization
- **Optimization:** The library automatically selects the most appropriate algorithm based on number size

## Constants and Enums

### Numeric Constants

- `PI` - Mathematical constant π (3.14159265358979323846)
- `NATIVE_LIMIT` - Limit for native methods (20000 bytes)
- `LLONG_SIZE` - Size of long long in bytes (8)

### Enumerations

**Radix** - Number system bases:

```cpp
enum class Radix {
    Hex,  // Hexadecimal (base 16)
    Dec,  // Decimal (base 10)
    Oct,  // Octal (base 8)
    Bin   // Binary (base 2)
};
```

**MulMethod** - Multiplication algorithms:

```cpp
enum class MulMethod {
    Karatsuba,              // Karatsuba algorithm (default)
    FastFourierTransform    // FFT-based multiplication
};
```

**ExceptionCode** - Error codes:

```cpp
enum class ExceptionCode {
    Unknown,          // Unknown error
    DivisionByZero,   // Division by zero
    InvalidRootDegree,// Invalid root degree
    InvalidValue,     // Invalid parameter value
    LengthError       // Size/length error
};
```

## Thread Safety

The BigInt class is not thread-safe. If you need to use BigInt objects across multiple threads, you must provide appropriate synchronization mechanisms.

## Compatibility

- C++ Standard: Requires C++11 or later
- Compilers: Tested with GCC, Clang
- Platforms: Cross-platform (Windows, Linux, macOS)

## License

MIT Copyright (c) 2025 Evgeny Drobotun, Anna Boykova

## Contacts

evgeny@drobotun.su

## Acknowledgments

- Thanks to all contributors and users of this library
- Special thanks to the open-source community for providing inspiration and algorithms

**Note:** This is version 0.1.0. The API may be subject to change in future releases. Please report any issues or feature requests through the appropriate channels.
