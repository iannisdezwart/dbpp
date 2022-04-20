#include <cstring>
#include <ctime>

#include "table-algorithms.hpp"
#include "on-disk-table.hpp"

struct Customer
{
	int id;
	char name[28];
	char address[28];
	int age;
};

struct Product
{
	int id;
	char name[28];
	int price;
};

struct Order
{
	int id;
	int customer_id;
	int product_id;
	int amount;
};

struct ProductXOrder
{
	int product_id;
	int order_id;
	int customer_id;
	int amount;
	char name[28];
	int price;
};

void
clear()
{
	auto customers = dbpp::OnDiskTable<Customer>::open("db/customers");
	customers.clear();

	auto products = dbpp::OnDiskTable<Product>::open("db/products");
	products.clear();

	auto orders = dbpp::OnDiskTable<Order>::open("db/orders");
	orders.clear();
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
fill()
{
	size_t customers_n;
	std::cout << "How many customers? ";
	std::cin >> customers_n;

	auto customers = dbpp::OnDiskTable<Customer>::open("db/customers");

	for (int i = 0; i < customers_n; i++)
	{
		Customer customer;
		customer.id = i;
		create_rand_str(customer.name, sizeof(customer.name));
		create_rand_str(customer.address, sizeof(customer.address));
		customer.age = rand() % 100;

		customers.insert(customer);
	}

	size_t products_n;
	std::cout << "How many products? ";
	std::cin >> products_n;

	auto products = dbpp::OnDiskTable<Product>::open("db/products");

	for (int i = 0; i < products_n; i++)
	{
		Product product;
		product.id = i;
		create_rand_str(product.name, sizeof(product.name));
		product.price = rand() % 10000;

		products.insert(product);
	}

	size_t orders_n;
	std::cout << "How many orders? ";
	std::cin >> orders_n;

	auto orders = dbpp::OnDiskTable<Order>::open("db/orders");

	for (int i = 0; i < orders_n; i++)
	{
		Order order;
		order.id = i;
		order.customer_id = rand() % customers_n;
		order.product_id = rand() % products_n;
		order.amount = rand() % 5;

		orders.insert(order);
	}
}

void
print_customers()
{
	auto customers = dbpp::OnDiskTable<Customer>::open("db/customers");

	printf("%4s | %28s | %28s | %3s\n",
		"ID", "Name", "Address", "Age");
	std::cout << std::string(80, '-') << std::endl;

	for (const Customer &customer : customers)
	{
		printf("%4d | %28s | %28s | %3d\n",
			customer.id, customer.name, customer.address, customer.age);
	}
}

void
print_products()
{
	auto products = dbpp::OnDiskTable<Product>::open("db/products");

	printf("%4s | %28s | %6s\n",
		"ID", "Name", "Price");
	std::cout << std::string(80, '-') << std::endl;

	for (const Product &product : products)
	{
		printf("%4d | %28s | %6d\n",
			product.id, product.name, product.price);
	}
}

void
print_orders()
{
	auto orders = dbpp::OnDiskTable<Order>::open("db/orders");

	printf("%4s | %11s | %10s | %6s\n",
		"ID", "Customer ID", "Product ID", "Amount");
	std::cout << std::string(80, '-') << std::endl;

	for (const Order &order : orders)
	{
		printf("%4d | %11d | %10d | %6d\n",
			order.id, order.customer_id, order.product_id,
			order.amount);
	}
}

void
join()
{
	auto products = dbpp::OnDiskTable<Product>::open("db/products");
	auto orders = dbpp::OnDiskTable<Order>::open("db/orders");

	auto joined = dbpp::bnl_join_into_disk<ProductXOrder, Product, Order>(
		products, orders,
		[](const Product &product, const Order &order)
		{
			return product.id == order.product_id;
		},
		[](const Product &product, const Order &order)
		{
			ProductXOrder pxo;

			pxo.product_id = product.id;
			pxo.order_id = order.id;
			pxo.customer_id = order.customer_id;
			pxo.amount = order.amount;
			strcpy(pxo.name, product.name);
			pxo.price = product.price;

			return pxo;
		});

	printf("%8s | %11s | %10s | %6s | %28s | %6s\n",
		"Order ID", "Customer ID", "Product ID", "Amount", "Name", "Price");
	std::cout << std::string(80, '-') << std::endl;

	for (const ProductXOrder &pxo : joined)
	{
		printf("%8d | %11d | %10d | %6d | %28s | %6d\n",
			pxo.order_id, pxo.customer_id, pxo.product_id,
			pxo.amount, pxo.name, pxo.price);
	}
}

int
main(int argc, char **argv)
{
	srand(time(nullptr));
	dbpp::io::ensure_dir("db");

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <clear|fill|print-customers"
			"|print-products|print-orders|join\n", argv[0]);
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
	else if (strcmp(argv[1], "print-customers") == 0)
	{
		print_customers();
	}
	else if (strcmp(argv[1], "print-products") == 0)
	{
		print_products();
	}
	else if (strcmp(argv[1], "print-orders") == 0)
	{
		print_orders();
	}
	else if (strcmp(argv[1], "join") == 0)
	{
		join();
	}
	else
	{
		fprintf(stderr, "Unknown command: %s\n", argv[1]);
		return 1;
	}
}