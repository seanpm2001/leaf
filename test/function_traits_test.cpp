// Copyright (c) 2018-2019 Emil Dotchevski
// Copyright (c) 2018-2019 Second Spectrum, Inc.

// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/detail/function_traits.hpp>
#include <functional>

template <class F>
void check_traits( F )
{
	using namespace boost::leaf::leaf_detail;
	using boost::leaf::leaf_detail_mp11::mp_list;
	static_assert(function_traits<F>::arity==4,"arity");
	static_assert(std::is_same<fn_return_type<F>,double>::value,"return_type");
	static_assert(std::is_same<fn_arg_type<F,0>,int>::value,"arg<0>");
	static_assert(std::is_same<fn_arg_type<F,1>,float>::value,"arg<1>");
	static_assert(std::is_same<fn_arg_type<F,2>,int const &>::value,"arg<2>");
	static_assert(std::is_same<fn_arg_type<F,3>,float &&>::value,"arg<3>");
	static_assert(std::is_same<fn_mp_args<F>,mp_list<int,float,int const &,float &&>>::value,"mp_args");
}

double f1( int, float, int const &, float && )
{
	return 42;
}

int main()
{
	check_traits(&f1);
	check_traits(std::function<double(int const volatile, float const, int const &, float &&)>(f1));
	check_traits( []( int const volatile, float const, int const &, float && ) -> double
		{
			return 42;
		} );
	return 0;
}
