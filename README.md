<!--
// The MIT License (MIT)
//
// Copyright (c) 2026 Bit Factory Software GmbH
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// -->
[![MIT Licence](http://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/license/mit)
[![CI](https://github.com/bitfactory-software/anyxx/actions/workflows/ci.yml/badge.svg)](https://github.com/bitfactory-software/anyxx/actions/workflows/ci.yml)
[![Static Badge](https://img.shields.io/badge/docs-Any%2B%2B-blue)](https://www.alexweb.io/anyxx/)

<img width="454" height="355" alt="Any++logo-small" src="https://github.com/user-attachments/assets/e4766dd4-7a21-486d-9dd0-8725149e0754" />

**Any++** enhances the well-known *boost::any* and *std::any* with:

- **Flexible choice of underlying storage mechanisms**
  - Type-erased
  - Concrete
  - Variant
- **Extensibility**
  - Ability to add functions and operators using Rust-like **traits** (including templates)
  - Support for **open (multi-)dispatch**
  - Runtime support for **downcasts and cross-casts**

To allow easy **separation of concerns**, the library adds utilities for:
- Factories
- Hooks
- Load-time extendable members

The performance of **dynamic dispatch is on par with virtual functions**. 

The combination of static and dynamic dispatch enables 
- a hybrid usage of `std::variant` and **type erasure** to balance performance and coupling.
- C++0x-like concept maps for static and dynamic customization points and default behavior.

Docs and Online Examples are available at [https://www.alexweb.io/anyxx/](https://www.alexweb.io/anyxx/)
