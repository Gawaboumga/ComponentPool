#include "ComponentPool.hpp"
#include <Catch/catch.hpp>

SCENARIO("One component pool and insertion", "[ComponentPool]")
{
	GIVEN("One empty component pool")
	{
		ComponentPool<int, char> componentPool{};

		WHEN("We insert one element")
		{
			componentPool.insert(5, '3');

			THEN("We can retrieve it")
			{
				auto it = componentPool.find(5);
				REQUIRE(it != componentPool.end());
				REQUIRE(*it == '3');
			}
		}

		WHEN("We insert two elements")
		{
			componentPool.insert(6, '3');
			componentPool.insert(4, '6');

			THEN("They are sorted")
			{
				REQUIRE(componentPool.size() == 2);
				auto it6 = componentPool.find(6);
				REQUIRE(it6 != componentPool.end());
				REQUIRE(*it6 == '3');
				auto it4 = componentPool.find(4);
				REQUIRE(it4 != componentPool.end());
				REQUIRE(*it4 == '6');
				REQUIRE(++it4 == it6);
			}

			THEN("We add one in the middle, it's still sorted")
			{
				componentPool.insert(5, 'A');

				REQUIRE(componentPool.size() == 3);
				auto it6 = componentPool.find(6);
				REQUIRE(it6 != componentPool.end());
				REQUIRE(*it6 == '3');
				auto it5 = componentPool.find(5);
				REQUIRE(it5 != componentPool.end());
				REQUIRE(*it5 == 'A');
				REQUIRE(++it5 == it6);
				auto it4 = componentPool.find(4);
				REQUIRE(it4 != componentPool.end());
				REQUIRE(*it4 == '6');
				REQUIRE(++it4 == componentPool.find(5));
			}
		}
	}
}

SCENARIO("Two component pools and their move semantics", "[ComponentPool]")
{
	GIVEN("Two component pools with one and two elements")
	{
		ComponentPool<int, char> componentPool1{};
		componentPool1.insert(3, '5');
		ComponentPool<int, char> componentPool2{};
		componentPool2.insert(5, '3');
		componentPool2.insert(4, '6');

		WHEN("We use the move constructor")
		{
			ComponentPool<int, char> owningPool{ std::move(componentPool1) };

			THEN("Moved component pool is empty")
			{
				REQUIRE(componentPool1.empty());
			}

			THEN("Constructed component pool has its content")
			{
				REQUIRE(owningPool.size() == 1);
				auto it = owningPool.find(3);
				REQUIRE(it != owningPool.end());
				REQUIRE(*it == '5');
			}
		}

		WHEN("We use the move assignment")
		{
			componentPool1 = std::move(componentPool2);

			THEN("Moved component pool is empty")
			{
				REQUIRE(componentPool2.empty());
			}

			THEN("Constructed component pool has its content")
			{
				REQUIRE(componentPool1.size() == 2);
				auto it = componentPool1.find(4);
				REQUIRE(it != componentPool1.end());
				REQUIRE(*it == '6');
			}
		}

		WHEN("We swap component pools")
		{
			std::swap(componentPool1, componentPool2);

			THEN("Their content is swapped")
			{
				REQUIRE(componentPool1.size() == 2);
				REQUIRE(componentPool2.size() == 1);
			}
		}
	}
}