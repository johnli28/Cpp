#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <atomic>

using namespace std;

void print(const boost::system::error_code& /*e*/,
	boost::asio::deadline_timer* t, int* count)
{
	(*count)++;
	std::cout << "This is now counting: " << *count << std::endl;

	t->expires_at(t->expires_at() + boost::posix_time::seconds(1));
	t->async_wait(boost::bind(print, boost::asio::placeholders::error, t, count));
}

int main()
{
	boost::asio::io_context io;

	int count = 0;
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(1));
	t.async_wait(boost::bind(print,
		boost::asio::placeholders::error, &t, &count));

	io.run();

	return 0;
}