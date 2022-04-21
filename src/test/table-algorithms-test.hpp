#pragma once

#include "test.hpp"
#include "../table-algorithms.hpp"
#include "../in-memory-table.hpp"

#define PRODUCTS_PATH "/tmp/test-products"
#define ORDERS_PATH "/tmp/test-orders"

struct Product
{
	int id;
	char name[28];
	int price;

	bool
	operator==(const Product &other)
	const
	{
		return id == other.id
			&& strcmp(name, other.name) == 0
			&& price == other.price;
	}

	friend std::ostream &
	operator<<(std::ostream &os, const Product &record)
	{
		os << "Product { id = " << record.id << ", name = "
			<< record.name << ", price = " << record.price << " }";
		return os;
	}
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

	ProductXOrder() {}

	ProductXOrder(const Product &product, const Order &order)
	{
		product_id = product.id;
		order_id = order.id;
		customer_id = order.customer_id;
		amount = order.amount;
		strcpy(name, product.name);
		price = product.price;
	}

	static bool
	is_match(const Product &product, const Order &order)
	{
		return order.product_id == product.id;
	}

	bool
	operator==(const ProductXOrder &other)
	const
	{
		return product_id == other.product_id
			&& order_id == other.order_id
			&& customer_id == other.customer_id
			&& amount == other.amount
			&& strcmp(name, other.name) == 0
			&& price == other.price;
	}

	friend std::ostream &
	operator<<(std::ostream &os, const ProductXOrder &record)
	{
		os << "ProductXOrder { product_id = " << record.product_id
			<< ", order_id = " << record.order_id
			<< ", customer_id = " << record.customer_id
			<< ", amount = " << record.amount
			<< ", name = " << record.name
			<< ", price = " << record.price << " }";
		return os;
	}
};

static std::vector<Product> test_products = {
	Product{ .id = 0, .name = "Strawberries", .price = 100 },
	Product{ .id = 1, .name = "Bananas", .price = 200 },
	Product{ .id = 2, .name = "Apples", .price = 300 },
	Product{ .id = 3, .name = "Oranges", .price = 400 },
	Product{ .id = 4, .name = "Grapes", .price = 500 },
	Product{ .id = 5, .name = "Watermelon", .price = 600 },
	Product{ .id = 6, .name = "Pineapple", .price = 700 },
	Product{ .id = 7, .name = "Cherries", .price = 800 },
	Product{ .id = 8, .name = "Pears", .price = 900 },
	Product{ .id = 9, .name = "Kiwi", .price = 1000 },
};

static std::vector<Order> test_orders = {
	Order{ .id = 0, .customer_id = 2, .product_id = 5, .amount = 5 },
	Order{ .id = 1, .customer_id = 1, .product_id = 4, .amount = 2 },
	Order{ .id = 2, .customer_id = 2, .product_id = 4, .amount = 8 },
	Order{ .id = 3, .customer_id = 1, .product_id = 4, .amount = 3 },
	Order{ .id = 4, .customer_id = 3, .product_id = 8, .amount = 3 },
	Order{ .id = 5, .customer_id = 1, .product_id = 1, .amount = 1 },
	Order{ .id = 6, .customer_id = 2, .product_id = 0, .amount = 9 },
	Order{ .id = 7, .customer_id = 1, .product_id = 3, .amount = 8 },
	Order{ .id = 8, .customer_id = 4, .product_id = 5, .amount = 2 },
	Order{ .id = 9, .customer_id = 1, .product_id = 4, .amount = 3 },
	Order{ .id = 10, .customer_id = 3, .product_id = 6, .amount = 2 },
	Order{ .id = 11, .customer_id = 5, .product_id = 1, .amount = 4 },
	Order{ .id = 12, .customer_id = 7, .product_id = 0, .amount = 5 },
	Order{ .id = 13, .customer_id = 4, .product_id = 5, .amount = 7 },
	Order{ .id = 14, .customer_id = 6, .product_id = 0, .amount = 9 }
};

static std::vector<ProductXOrder> expected_joined = {
	ProductXOrder(test_products[0], test_orders[6]),
	ProductXOrder(test_products[0], test_orders[12]),
	ProductXOrder(test_products[0], test_orders[14]),
	ProductXOrder(test_products[1], test_orders[5]),
	ProductXOrder(test_products[1], test_orders[11]),
	ProductXOrder(test_products[3], test_orders[7]),
	ProductXOrder(test_products[4], test_orders[1]),
	ProductXOrder(test_products[4], test_orders[2]),
	ProductXOrder(test_products[4], test_orders[3]),
	ProductXOrder(test_products[4], test_orders[9]),
	ProductXOrder(test_products[5], test_orders[0]),
	ProductXOrder(test_products[5], test_orders[8]),
	ProductXOrder(test_products[5], test_orders[13]),
	ProductXOrder(test_products[6], test_orders[10]),
	ProductXOrder(test_products[8], test_orders[4])
};

static inline dbpp::OnDiskTable<Product>
create_on_disk_products_table()
{
	auto products = dbpp::OnDiskTable<Product>::open(PRODUCTS_PATH);
	products.clear();

	for (const Product &product : test_products)
	{
		products.insert(product);
	}

	return products;
}

static inline dbpp::OnDiskTable<Order>
create_on_disk_orders_table()
{

	auto orders = dbpp::OnDiskTable<Order>::open(ORDERS_PATH);
	orders.clear();

	for (const Order &order : test_orders)
	{
		orders.insert(order);
	}

	return orders;
}

static inline void
bnl_join_disk_disk_into_disk()
{
	auto products = create_on_disk_products_table();
	auto orders = create_on_disk_orders_table();
	auto joined_table = dbpp::bnl_join_into_disk<ProductXOrder>(
		products, orders);
	auto expected_joined_table = dbpp::InMemoryTable(expected_joined);

	ASSERT_EQ(joined_table, expected_joined_table);
}

static inline void
bnl_join_mem_mem_into_disk()
{
	auto products = dbpp::InMemoryTable(test_products);
	auto orders = dbpp::InMemoryTable(test_orders);
	auto joined_table = dbpp::bnl_join_into_disk<ProductXOrder>(
		products, orders);
	auto expected_joined_table = dbpp::InMemoryTable(expected_joined);

	ASSERT_EQ(joined_table, expected_joined_table);
}

static inline void
bnl_join_disk_mem_into_disk()
{
	auto products = create_on_disk_products_table();
	auto orders = dbpp::InMemoryTable(test_orders);
	auto joined_table = dbpp::bnl_join_into_disk<ProductXOrder>(
		products, orders);
	auto expected_joined_table = dbpp::InMemoryTable(expected_joined);

	ASSERT_EQ(joined_table, expected_joined_table);
}

static inline void
bnl_join_mem_disk_into_disk()
{
	auto products = create_on_disk_products_table();
	auto orders = dbpp::InMemoryTable(test_orders);
	auto joined_table = dbpp::bnl_join_into_disk<ProductXOrder>(
		products, orders);
	auto expected_joined_table = dbpp::InMemoryTable(expected_joined);

	ASSERT_EQ(joined_table, expected_joined_table);
}

static inline void
bnl_join_disk_disk_into_mem()
{
	auto products = create_on_disk_products_table();
	auto orders = create_on_disk_orders_table();
	auto joined_table = dbpp::bnl_join_into_memory<ProductXOrder>(
		products, orders);
	auto expected_joined_table = dbpp::InMemoryTable(expected_joined);

	ASSERT_EQ(joined_table, expected_joined_table);
}

static inline void
bnl_join_mem_mem_into_mem()
{
	auto products = dbpp::InMemoryTable(test_products);
	auto orders = dbpp::InMemoryTable(test_orders);
	auto joined_table = dbpp::bnl_join_into_memory<ProductXOrder>(
		products, orders);
	auto expected_joined_table = dbpp::InMemoryTable(expected_joined);

	ASSERT_EQ(joined_table, expected_joined_table);
}

static inline void
bnl_join_disk_mem_into_mem()
{
	auto products = create_on_disk_products_table();
	auto orders = dbpp::InMemoryTable(test_orders);
	auto joined_table = dbpp::bnl_join_into_memory<ProductXOrder>(
		products, orders);
	auto expected_joined_table = dbpp::InMemoryTable(expected_joined);

	ASSERT_EQ(joined_table, expected_joined_table);
}

static inline void
bnl_join_mem_disk_into_mem()
{
	auto products = create_on_disk_products_table();
	auto orders = dbpp::InMemoryTable(test_orders);
	auto joined_table = dbpp::bnl_join_into_memory<ProductXOrder>(
		products, orders);
	auto expected_joined_table = dbpp::InMemoryTable(expected_joined);

	ASSERT_EQ(joined_table, expected_joined_table);
}

static inline void
equals_disk_disk()
{
	auto a = create_on_disk_products_table();
	auto b = create_on_disk_products_table();

	ASSERT_EQ(a, b);
}

static inline void
equals_mem_mem()
{
	auto a = dbpp::InMemoryTable(test_products);
	auto b = dbpp::InMemoryTable(test_products);

	ASSERT_EQ(a, b);
}

static inline void
equals_mem_disk()
{
	auto a = dbpp::InMemoryTable(test_products);
	auto b = create_on_disk_products_table();

	ASSERT_EQ(a, b);
}

static inline void
equals_disk_mem()
{
	auto a = create_on_disk_products_table();
	auto b = dbpp::InMemoryTable(test_products);

	ASSERT_EQ(a, b);
}

TEST_SEQUENCE(TableAlgorithms_tests, {
	bnl_join_disk_disk_into_disk,
	bnl_join_mem_mem_into_disk,
	bnl_join_disk_mem_into_disk,
	bnl_join_mem_disk_into_disk,
	bnl_join_disk_disk_into_mem,
	bnl_join_mem_mem_into_mem,
	bnl_join_disk_mem_into_mem,
	bnl_join_mem_disk_into_mem,
	equals_disk_disk,
	equals_mem_mem,
	equals_disk_mem,
	equals_mem_disk
});