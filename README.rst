BigInt
======

Arithmetic and logical operation on a long numbers. Implements various arithmetic (addition, substruction, multiplication, division, exponentiation, calculating the root) and logical (logical "and", logical "or", logical "xor", logical negation and bitwise shift) operations.

.. image:: https://img.shields.io/github/license/drobotun/virustotalapi3?style=flat
    :target: https://github.com/drobotun/bigint/blob/master/LICENSE
.. image:: https://img.shields.io/badge/Release-0.1.0-blue
    :target: https://github.com/drobotun/bigint/releases/tag/v0.1.0

Features
""""""""

Arithmetic operations:
++++++++++++++++++++++

* **Addition (+)**
* **Subtraction (-)**
* **Multiplication (*)**

    * native method (less than 50000 bytes)
    * Karatsuba method (more than 50000 bytes)
    * fast Fourier transform (FFT) method (more than 50000 bytes)

Karatsuba multiplication method or FFT multiplication method is selected using the ``setMulMethod()`` function of the ``BigInt`` class. The Karatsuba method is set by default.

* **Division (/)**

    * native method (less than 50000 bytes)
    * Newton-Raphson method (more than 50000 bytes)

* **Getting the remainder of the division (%)**

    * native method (less than 50000 bytes)
    * Newton-Raphson method (more than 50000 bytes)

* **Decrement operator (++)**
* **Increment operator (--)**

Comparison operations:
++++++++++++++++++++++

* **Equally (==)**
* **Not equally (!=)**
* **More (>)**
* **Less (<)**
* **More and Equally (>=)**
* **Less and Equally (<=)**

Logical operations:
+++++++++++++++++++

* **Logical "And" (&)**
* **Logical "Or" (|)**
* **Logical "Xor" (^)**
* **Logical negation (~)**
* **Bitwise left shift (<<)**
* **Bitwise right shift (>>)**

Input output operations:
++++++++++++++++++++++++

* **Output (<<)** - redefined output operation for **BigInt** value 
* **Intput (>>)** - redefined input operation for **BigInt** value 

Mathematical functions:
+++++++++++++++++++++++

* **sqr()** - squaring a number
* **pow()** - exponentiation of a number (binary exponentiation algorithm)
* **powMod()** - exponentiation of a number modulo (binary exponentiation algorithm)
* **sqrt()** - square root of a number
* **root()** - root of a number (binary exponentiation algorithm)
* **abs()** - absolute value of a number (modulus of a number)

Installation
""""""""""""

To use the library, you need to include the header file **bigint.h**

    #include "bigint.h"

Usage
"""""

License
"""""""

MIT Copyright (c) 2025 Evgeny Drobotun, Anna Boykova

Contacts
""""""""

evgeny@drobotun.su
