BigInt
======

.. image:: https://img.shields.io/github/license/drobotun/virustotalapi3?style=flat
    :target: https://github.com/drobotun/bigint/blob/master/LICENSE
.. image:: https://img.shields.io/badge/Release-0.1.0-blue
    :target: https://github.com/drobotun/bigint/releases/tag/v0.1.0

Features
""""""""

Addition (+)
------------

    * native method

Subtraction (-)
---------------

    * native method

Multiplication (*)
------------------

    * native method (less than 20000 digits)
    * Karatsuba method (more than 20000 digits)
    * fast Fourier transform (FFT) method (more than 20000 digits)

Karatsuba multiplication method or FFT method is selected when initializing an instance of the **BigInt** class or using the **setMulMethod()** function of the **BigInt** class.

Division (/)
------------

    * native method (less than 20000 digits)
    * Newton-Raphson method (more than 20000 digits)

Installation
""""""""""""

Usage
"""""

License
"""""""

MIT Copyright (c) 2025 Evgeny Drobotun, Anna Boykova
