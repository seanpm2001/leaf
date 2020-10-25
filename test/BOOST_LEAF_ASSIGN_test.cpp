// Copyright (c) 2018-2020 Emil Dotchevski and Reverge Studios, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/result.hpp>
#include <boost/leaf/handle_errors.hpp>
#include "lightweight_test.hpp"

namespace leaf = boost::leaf;

struct value
{
    int x;

    value( value const & ) = delete;
    value( value && ) = default;
};

leaf::result<value> f1()
{
    return value { 21 };
}

leaf::result<value> f2()
{
    BOOST_LEAF_ASSIGN(auto a, f1());
    return a; // Doesn't need to be return std::move(a);
}

leaf::result<value> f3()
{
    BOOST_LEAF_ASSIGN(auto a, f2());
    BOOST_LEAF_ASSIGN(auto b, f2()); // Invoking the macro twice in the same scope, testing the temp name generation
    return value { a.x + b.x };
}

int main()
{
    BOOST_TEST_EQ(f3()->x, 42);

    {
        int r = leaf::try_handle_all(
            []() -> leaf::result<int>
            {
                int x = 42;

                leaf::result<int> r1(x);
                BOOST_LEAF_ASSIGN(auto && rx1, r1);
                BOOST_TEST_EQ(r1.value(), rx1);

                leaf::result<int &> r2(x);
                BOOST_LEAF_ASSIGN(auto && rx2, r2);
                BOOST_TEST_EQ(r2.value(), rx2);

                leaf::result<int &> r3(x);
                BOOST_LEAF_ASSIGN(auto & rx3, r3);
                BOOST_TEST_EQ(&r3.value(), &rx3);

                return 0;
            },
            []
            {
                return 1;
            } );
        BOOST_TEST_EQ(r, 0);
    }

    return boost::report_errors();
}
