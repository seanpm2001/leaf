//Copyright (c) 2018 Emil Dotchevski
//Copyright (c) 2018 Second Spectrum, Inc.

//Distributed under the Boost Software License, Version 1.0. (See accompanying
//file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/leaf/exception_capture.hpp>
#include <boost/leaf/exception.hpp>
#include <boost/leaf/preload.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <vector>
#include <future>
#include <iterator>
#include <algorithm>

namespace leaf = boost::leaf;

struct my_error: std::exception { };

template <int> struct my_info { int value; };

struct
fut_info
	{
	int a;
	int b;
	int result;
	std::future<int> fut;
	};
template <class F>
std::vector<fut_info>
launch_tasks( int task_count, F && f )
	{
	assert(task_count>0);
	std::vector<fut_info> fut;
	std::generate_n( std::inserter(fut,fut.end()), task_count, [&f]
		{
		int const a = rand();
		int const b = rand();
		int const res = (rand()%10) - 5;
		return fut_info { a, b, res, std::async( std::launch::async, [f,a,b,res]
			{
			auto wrapper = leaf::capture_exception<my_info<1>,my_info<2>,my_info<3>>(std::move(f));
			return wrapper( a, b, res );
			} ) };
		} );
	return fut;
	}
template <class F>
void
test( int task_count, F && f ) noexcept
	{
	std::vector<fut_info> fut = launch_tasks( task_count, std::forward<F>(f) );
	for( auto & f : fut )
		{
		using namespace leaf::leaf_detail;
		f.fut.wait();
		leaf::expect<my_info<1>,my_info<2>,my_info<4>> exp;
		try
			{
			int r = leaf::get(f.fut);
			BOOST_TEST(r>=0);
			BOOST_TEST(r==f.result);
			}
		catch(
		my_error const & e )
			{
			handle_error( exp, e, leaf::match<my_info<1>,my_info<2>>( [&f]( int x1, int x2 )
				{
				BOOST_TEST(x1==f.a);
				BOOST_TEST(x2==f.b);
				} ) );
			}
		}
	}
int
main()
	{
	test( 42, [ ]( int a, int b, int res )
		{
		if( res>=0 )
			return res;
		else
			leaf::throw_exception( my_error(), my_info<1>{a}, my_info<2>{b}, my_info<3>{}) ;
		} );
	test( 42, [ ]( int a, int b, int res )
		{
		if( res>=0 )
			return res;
		else
			{
			auto propagate = leaf::preload( my_info<1>{a}, my_info<2>{b}, my_info<3>{} );
			throw my_error();
			}
		} );
	return boost::report_errors();
	}