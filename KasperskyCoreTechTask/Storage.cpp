#include "stdafx.h"
#include <tuple>

#include "Storage.h"

//�������: O(log(N)), ������: O(N)
bool Storage::Insert(const std::string & id, const StorageData & data)
{
	std::unique_lock<std::shared_mutex> write_lock(storage_mutex_);
	auto result = local_id_storage_.insert(std::pair<std::string, StorageData>(id, data));
	if (std::get<1>(result))
	{
		local_manufac_storage_.insert(ManufID(data.GetManufacturer(), id));
	}

	return std::get<1>(result);
}

//�������: O(1), ������: O(N)
StorageData Storage::FindValueByID(const std::string & id) const
{
	std::shared_lock<std::shared_mutex> read_lock(storage_mutex_);
	auto iter = local_id_storage_.find(id);
	if (iter != local_id_storage_.end())
	{
		return iter->second;
	}

	return StorageData();
}

/*�������: O(logN), ������: O(N)*/
std::vector<std::pair<std::string, StorageData>> Storage::FindValuesByManufacturer(const std::string & manufacturer) const
{
	std::shared_lock<std::shared_mutex> read_lock(storage_mutex_);

	std::vector<std::pair<std::string, StorageData>> result;
	auto lower_iter = local_manufac_storage_.lower_bound(ManufID(manufacturer, ""));	/*���������� ������������� ��������� ���������� ��������, 
																						��������� lower_bound ����� �� ������� set::end() */
	for (;lower_iter != local_manufac_storage_.end() && lower_iter->manufacturer == manufacturer; ++lower_iter)
	{
		auto id_iter = local_id_storage_.find(lower_iter->id);
		if (id_iter == local_id_storage_.end())	//������, ���������������� ���������������
			return std::vector<std::pair<std::string, StorageData>>();
		result.push_back(*id_iter);
	}

	return result;
}

/*�������: O(log(N)) ������: O(N)*/
bool Storage::Erase(const std::string & id)
{
	std::unique_lock<std::shared_mutex> write_lock(storage_mutex_);
	auto iter = local_id_storage_.find(id);
	if (iter == local_id_storage_.end())
		return false;

	local_manufac_storage_.erase(ManufID(iter->second.GetManufacturer(), id));
	local_id_storage_.erase(iter);
	return true;
}
