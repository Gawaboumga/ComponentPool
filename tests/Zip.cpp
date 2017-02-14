#include "ComponentPool.hpp"
#include "Zip.hpp"
#include <Catch/catch.hpp>

SCENARIO("Zip", "[Zip]")
{
	GIVEN("One component pool")
	{
		int entityID{ 3 };
		ComponentPool<int, float> componentPool{};
		componentPool.insert(1, 1.f);
		componentPool.insert(entityID, 3.f);
		componentPool.insert(4, 6.f);

		WHEN("We iterate")
		{
			int i{ 0 };

			for (auto&& entity : zip(componentPool))
			{
				entity->GetComponent<float>() = 4.f;
				++i;
			}

			THEN("We only get three results")
			{
				REQUIRE(i == 3);
				for (auto&& entity : zip(componentPool))
					REQUIRE(entity->GetComponent<float>() == Approx(4.f));
			}
		}
	}

	GIVEN("Two component pools")
	{
		int entityID{ 3 };
		ComponentPool<int, char> componentPool1{};
		componentPool1.insert(entityID, '5');
		ComponentPool<int, float> componentPool2{};
		componentPool2.insert(1, 1.f);
		componentPool2.insert(entityID, 3.f);
		componentPool2.insert(4, 6.f);

		WHEN("We iterate through them")
		{
			int i{ 0 };
			for (auto&& entity : zip(componentPool1, componentPool2))
			{
				entity->GetComponent<float>() = 4.f;
				++i;
			}

			THEN("We only get one result")
			{
				REQUIRE(i == 1);
				REQUIRE(*componentPool2.find(entityID) == Approx(4.f));
			}
		}
	}

	GIVEN("Three component pools")
	{
		int firstEntity{ 3 };
		int secondEntity{ 30 };
		ComponentPool<int, char> componentPool1{};
		componentPool1.insert(1, 'A');
		componentPool1.insert(firstEntity, 'B');
		componentPool1.insert(15, 'C');
		componentPool1.insert(secondEntity, 'D');
		ComponentPool<int, float> componentPool2{};
		componentPool2.insert(firstEntity, 1.f);
		componentPool2.insert(15, 3.f);
		componentPool2.insert(secondEntity, 6.f);
		ComponentPool<int, int> componentPool3{};
		componentPool3.insert(1, 0.f);
		componentPool3.insert(firstEntity, 1.f);
		componentPool3.insert(13, 3.f);
		componentPool3.insert(secondEntity, 6.f);

		WHEN("We iterate through them")
		{
			int i{ 0 };
			for (auto&& entity : zip(componentPool1, componentPool2, componentPool3))
			{
				entity->GetComponent<float>() = (i == 0) ? 4.f : -1.f;
				++i;
			}

			THEN("We get two results")
			{
				REQUIRE(i == 2);
				REQUIRE(*componentPool2.find(firstEntity) == Approx(4.f));
				REQUIRE(*componentPool2.find(secondEntity) == Approx(-1.f));
			}
		}
	}
}