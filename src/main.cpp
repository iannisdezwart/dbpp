#include <cstring>
#include <ctime>

#include "on-disk-table.hpp"

struct Person
{
	char first_name[30];
	char last_name[30];
	int age;
};

struct PersonFirstName
{
	char first_name[30];

	PersonFirstName() {}

	PersonFirstName(const char first_name[30])
	{
		strncpy(this->first_name, first_name, 30);
	}
};

void
clear()
{
	auto people = dbpp::OnDiskTable<Person>::open("people");
	people.clear();
}

void
fill()
{
	auto people = dbpp::OnDiskTable<Person>::open("people");

	people.insert({ "John", "Howarth", 65 });
	people.insert({ "Kieran", "Mistry", 72 });
	people.insert({ "Louie", "Kent", 23 });
	people.insert({ "Riley", "Craig", 45 });
	people.insert({ "Oliver", "Macdonald", 74 });
	people.insert({ "Scott", "Banks", 69 });
	people.insert({ "Joe", "Dixon", 22 });
	people.insert({ "Bradley", "Jenkins", 33 });
	people.insert({ "Maisie", "Hartley", 43 });
	people.insert({ "Abbie", "Barlow", 55 });

	printf("%30s | %30s | %8s\n",
		"First name", "Last name", "Age");
	printf("------------------------------   ------------------------------"
		"   --------\n");

	for (const Person &person : people)
	{
		printf("%30s | %30s | %8d\n",
			person.first_name, person.last_name, person.age);
	}
}

const static char rand_chars[] = "abcdefghijklmnopqrstuvwxyz";

void
create_rand_str(char *dst, size_t n)
{
	size_t size = rand() % (n - 4) + 3;

	for (size_t i = 0; i < size; i++)
	{
		dst[i] = rand_chars[rand() % sizeof(rand_chars)];
	}

	dst[size] = '\0';
}

void
fill_humungous()
{
	auto people = dbpp::OnDiskTable<Person>::open("people");

	for (size_t i = 0; i < (size_t) 1E6; i++)
	{
		Person p;
		create_rand_str(p.first_name, sizeof(p.first_name));
		create_rand_str(p.last_name, sizeof(p.last_name));
		p.age = rand() % 100;

		people.insert(p);
	}
}

void
filter()
{
	auto people = dbpp::OnDiskTable<Person>::open("people");

	int age;
	std::cout << "Enter max age: ";
	std::cin >> age;

	auto filtered_people = people.filter_into_memory([age](const Person &p)
	{
		return p.age <= age;
	});

	printf("%30s | %30s | %8s\n",
		"First name", "Last name", "Age");
	printf("------------------------------   ------------------------------"
		"   --------\n");

	for (const Person &person : filtered_people)
	{
		printf("%30s | %30s | %8d\n",
			person.first_name, person.last_name, person.age);
	}
}

void
filter_map()
{
	auto people = dbpp::OnDiskTable<Person>::open("people");

	int age;
	std::cout << "Enter max age: ";
	std::cin >> age;

	auto filtered_people = people.filter_map_into_memory<PersonFirstName>(
		[age](const Person &p)
		{
			return p.age <= age;
		},
		[](const Person &p)
		{
			return PersonFirstName(p.first_name);
		});

	printf("%30s\n", "First name");
	printf("------------------------------\n");

	for (const PersonFirstName &person_first_name : filtered_people)
	{
		printf("%30s\n", person_first_name.first_name);
	}
}

void
print()
{
	auto people = dbpp::OnDiskTable<Person>::open("people");

	printf("%30s | %30s | %8s\n",
		"First name", "Last name", "Age");
	printf("------------------------------   ------------------------------"
		"   --------\n");

	for (const Person &person : people)
	{
		printf("%30s | %30s | %8d\n",
			person.first_name, person.last_name, person.age);
	}
}

int
main(int argc, char **argv)
{
	srand(time(nullptr));

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <clear|fill|fill-humungous|filter|filter-map|print>\n", argv[0]);
		return 1;
	}

	if (strcmp(argv[1], "clear") == 0)
	{
		clear();
	}
	else if (strcmp(argv[1], "fill") == 0)
	{
		fill();
	}
	else if (strcmp(argv[1], "fill-humungous") == 0)
	{
		fill_humungous();
	}
	else if (strcmp(argv[1], "filter") == 0)
	{
		filter();
	}
	else if (strcmp(argv[1], "filter-map") == 0)
	{
		filter_map();
	}
	else if (strcmp(argv[1], "print") == 0)
	{
		print();
	}
	else
	{
		fprintf(stderr, "Unknown command: %s\n", argv[1]);
		return 1;
	}
}