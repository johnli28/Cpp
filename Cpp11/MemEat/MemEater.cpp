#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp> 
#include <atomic>

// Use to convert bytes to KB
#define DIV 1024

using namespace std;

std::atomic_int ai = 0;

int MEM_EAT_PERCENT = 85;

void CheckFreeMemoryPercent(const boost::system::error_code& /*e*/,
	boost::asio::deadline_timer* t, int* count)
{
	MEMORYSTATUSEX statex;

	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	if (statex.dwMemoryLoad < MEM_EAT_PERCENT)
	{
		//std::cout << "The percentage of physical memory in use: " << statex.dwMemoryLoad  << "%" << std::endl;

		t->expires_at(t->expires_at() + boost::posix_time::milliseconds(1));
		t->async_wait(boost::bind(CheckFreeMemoryPercent,
			boost::asio::placeholders::error, t, count));
	}
	else
	{
		std::cout << "Memory percentage in use hits: " << statex.dwMemoryLoad << "%, enter any key to quit." << std::endl;
		ai = 1;
	}
}

void EatMemory()
{
	while(ai == 0)
	{
		new int();
	}
}

int main(int argc, char* argv[])
{
	// Check the number of parameters
	if (argc < 2) 
	{
		// Tell the user how to run the program
		std::cerr << "Usage:  MemEater Percentage" << std::endl;
		std::cerr << "Sample: To eat 95% of system physical memory, run: MemEater 95" << std::endl;
		/* "Usage messages" are a conventional way of telling the user
		* how to run a program if they enter the command incorrectly.
		*/
		return 1;
	}

	int percent = stoi(argv[1]);

	if (percent <= 0 || percent >= 100)
	{
		std::cerr << "Please specify a value between 0 to 100 for Percentage" << std::endl;
		std::cerr << "Sample: To eat 95% of system physical memory, run: MemEater 95" << std::endl;
		return 1;
	}

	MEM_EAT_PERCENT = percent;

	boost::asio::io_context io;

	int count = 0;
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(1));
	t.async_wait(boost::bind(CheckFreeMemoryPercent,
		boost::asio::placeholders::error, &t, &count));

	boost::thread workerThread(EatMemory);

	io.run();

	getchar();

	return 0;
}