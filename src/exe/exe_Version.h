//-----------------------------------------------------------------------------
// Created on: 26 November 2015
//-----------------------------------------------------------------------------
// Copyright (c) 2017 Sergey Slyadnev
// Code covered by the MIT License
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
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#ifndef exe_Version_h
#define exe_Version_h

//-----------------------------------------------------------------------------
// A-Situs version
//-----------------------------------------------------------------------------

#define ASITUS_VERSION_MAJOR 0 // Major: incremented by major releases
#define ASITUS_VERSION_MINOR 1 // Minor: incremented by minor releases
#define ASITUS_VERSION_PATCH 6 // Patch: incremented with bug-fix releases

//-----------------------------------------------------------------------------

// Cumulative version: 8 bits for each version number
#define ASITUS_VERSION_HEX \
( (ASITUS_VERSION_MAJOR << 16) | (ASITUS_VERSION_MINOR << 8) | ASITUS_VERSION_PATCH )

#define ASITUS_VERSION_STRING "0.1.6"
#define ASITUS_APP_NAME "Analysis Situs"

#endif
