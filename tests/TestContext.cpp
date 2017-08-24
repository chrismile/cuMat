#include <catch/catch.hpp>

#include <thread>
#include <mutex>
#include <set>
#include <vector>

#include <cuMat/src/Context.h>
#include "Barrier.h"

TEST_CASE("single_context", "[context]")
{
	//Test if the context can be created
	cuMat::Context& context = cuMat::Context::current();
	REQUIRE(context.stream() != nullptr);
}

TEST_CASE("muliple_contexts", "[context]")
{
	//Test if multiple contexts can be created
	int count = 8;
	std::mutex mutex1;
	Barrier barrier(count);
	std::set<cudaStream_t> streams;

	//create threads
	std::vector<std::thread> threads;
	for (int i=0; i<count; ++i)
	{
		threads.push_back(std::thread([&]()
		{
			cuMat::Context& context = cuMat::Context::current();
			REQUIRE(context.stream() != nullptr);
			mutex1.lock();
			//std::cout << context.stream() << std::endl;
			streams.insert(context.stream());
			mutex1.unlock();

			barrier.Wait();
		}));
	}

	//wait for threads to terminate
	for (int i=0; i<count; ++i)
	{
		threads[i].join();
	}

	//test if really count-many different streams were created
	REQUIRE(streams.size() == count);
}