#pragma once
#include <string>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include <set>

/*
уникальный артикул Ц строка
производитель товара Ц строка
наименование товара Ц строка
цена Ц целое число
*/

class StorageData
{
public:
	StorageData() : empty_(true) {};
	StorageData(const std::string &manufacturer, const std::string &product_name, uint16_t price)
		: empty_(false),
		manufacturer_(manufacturer),
		product_name_(product_name),
		price_(price)
		{};
	std::string GetManufacturer() const { return manufacturer_; };
	std::string GetProductName() const { return product_name_; };
	uint16_t GetPrice() const { return price_; };
	bool IsEmpty() const { return empty_; };

private:
	bool empty_;
	std::string manufacturer_;
	std::string product_name_;
	uint16_t price_;
};

class Storage
{
	//внутренн€€ структура дл€ св€зывани€ производител€ и id товара
	struct ManufID
	{
		ManufID(const std::string &manufacturer_, const std::string &id_)
			: manufacturer(manufacturer_),
			id(id_)
		{};

		std::string manufacturer;
		std::string id;
	};

	struct lex_compare
	{
		bool operator() (const ManufID &lhs, const ManufID &rhs) const
		{
			if (lhs.manufacturer < rhs.manufacturer)
				return true;
			else if (lhs.manufacturer == rhs.manufacturer)
				return lhs.id < rhs.id;

			return false;
		}
	};

public:
	bool Insert(const std::string &id, const StorageData &data);
	StorageData FindValueByID(const std::string &id) const;
	std::vector<std::pair<std::string, StorageData>> FindValuesByManufacturer(const std::string &manufacturer) const;
	bool Erase(const std::string &id);
private:
	std::unordered_map<std::string, StorageData> local_id_storage_;
	std::set<ManufID, lex_compare> local_manufac_storage_;
	mutable std::shared_mutex storage_mutex_;
};