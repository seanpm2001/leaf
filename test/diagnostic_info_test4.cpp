// Copyright 2018-2022 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef BOOST_LEAF_TEST_SINGLE_HEADER
#   include "leaf.hpp"
#else
#   include <boost/leaf/config.hpp>
#   include <boost/leaf/handle_errors.hpp>
#   include <boost/leaf/result.hpp>
#endif

#if BOOST_LEAF_CFG_STD_STRING
#   include <sstream>
#endif
#include <iostream>
#include <vector>

#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

struct my_error
{
    std::vector<int> vec;
    void append(int x)
    {
       vec.push_back(x);
    }
    friend std::ostream & operator<<( std::ostream & os, my_error const & x )
    {
        for( auto const & e : x.vec )
            os << "appended: " << e << std::endl;
        return os;
    }
};

leaf::result<void> f1()
{
    auto ctx_ = leaf::on_error([](my_error & e) {e.append(42);});
    return leaf::new_error("new_error");
}

leaf::result<void> f2()
{
    auto ctx_ = leaf::on_error([](my_error & e) {e.append(43);});
    return f1();
}

int main()
{
    leaf::try_handle_all([]() -> leaf::result<void>
    {
        return f2();
    },
    [](leaf::verbose_diagnostic_info const & e)
    {
#if BOOST_LEAF_CFG_STD_STRING
        std::ostringstream st;
        st << e;
        std::string s = st.str();
#if BOOST_LEAF_CFG_DIAGNOSTICS
        BOOST_TEST_NE(s.find("new_error"), s.npos);
        BOOST_TEST_NE(s.find("appended: 42"), s.npos);
        BOOST_TEST_NE(s.find("appended: 43"), s.npos);
#endif
        std::cout << s;
#endif
    } );

    return boost::report_errors();
}
