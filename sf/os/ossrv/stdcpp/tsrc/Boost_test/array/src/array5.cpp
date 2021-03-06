/* simple example for using class array<>
 * (C) Copyright Nicolai M. Josuttis 2001.
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */
/*
 * � Portions copyright (c) 2006-2007 Nokia Corporation.  All rights reserved.
*/

#include <iostream>
#include <boost/array.hpp>
#ifdef __SYMBIAN32__
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__
#endif

template <typename T>
void test_static_size (const T& cont)
{
    int tmp[T::static_size];
    for (unsigned i=0; i<T::static_size; ++i) {
        tmp[i] = int(cont[i]);
    }
    for (unsigned j=0; j<T::static_size; ++j) {
        std::cout << tmp[j] << ' ';
    }
    std::cout << std::endl;
}

int main()
{
std_log(LOG_FILENAME_LINE,"[Test Case for array5]");
    // define special type name
    typedef boost::array<float,6> Array;

    // create and initialize an array
    const Array a = { { 42.42 } };

    // use some common STL container operations
    std::cout << "static_size: " << a.size() << std::endl;
    std::cout << "size:        " << a.size() << std::endl;
    // Can't use std::boolalpha because it isn't portable
    std::cout << "empty:       " << (a.empty()? "true" : "false") << std::endl;
    std::cout << "max_size:    " << a.max_size() << std::endl;
    std::cout << "front:       " << a.front() << std::endl;
    std::cout << "back:        " << a.back() << std::endl;
    std::cout << "[0]:         " << a[0] << std::endl;
    std::cout << "elems:       ";

    // iterate through all elements
    for (Array::const_iterator pos=a.begin(); pos<a.end(); ++pos) {
        std::cout << *pos << ' ';
    }
    std::cout << std::endl;
    test_static_size(a);

    // check copy constructor and assignment operator
    Array b(a);
    Array c;
    c = a;
    if (a==b && a==c) {
        std::cout << "copy construction and copy assignment are OK"
                  << std::endl;
         std_log(LOG_FILENAME_LINE,"Result : Passed");
    }
    else {
        std::cout << "copy construction and copy assignment are BROKEN"
                  << std::endl;
    std_log(LOG_FILENAME_LINE,"Result : Failed"); 
		assert_failed = true; 
    }

    typedef boost::array<double,6> DArray;
    typedef boost::array<int,6> IArray;
    IArray ia = { { 1, 2, 3, 4, 5, 6 } } ; // extra braces silence GCC warning
    DArray da;
    da = ia;
    da.assign(42);
#ifdef __SYMBIAN32__
	testResultXml("array5");
	close_log_file();
#endif

    return 0;  // makes Visual-C++ compiler happy
}

