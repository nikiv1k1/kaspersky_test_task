#include "stdafx.h"
#include "CppUnitTest.h"
#include "Storage.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{	
	DWORD WINAPI WriterProc(LPVOID lpParam)
	{
		Storage *stor_for_threads = static_cast<Storage*>(lpParam);
		Assert::IsNotNull(stor_for_threads);

		for (int i = 0; i < 2; ++i)
		{
			StorageData data("AMD", "A4.1", 76);
			std::string id("11122");
			stor_for_threads->Insert(id, data);

			Sleep(300);
			stor_for_threads->Erase(id);
		}

		return ERROR_SUCCESS;
	}

	DWORD WINAPI SearchProc(LPVOID lpParam)
	{
		Storage *stor_for_threads = static_cast<Storage*>(lpParam);
		Assert::IsNotNull(stor_for_threads);

		for (int i = 0; i < 5; ++i)
		{
			auto result = stor_for_threads->FindValuesByManufacturer("AMD");
			Assert::IsTrue(result.size() > 1);
			Sleep(100);
		}
		return ERROR_SUCCESS;
	}

	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(BasicTestMethod)
		{
			Storage storage1;

			StorageData data1 ("Intel", "Corei7", 100);
			StorageData data2 ("AMD", "A4", 50);
			StorageData data3 ("AMD", "A6", 75);

			std::string id1("123");
			std::string id2("456");
			std::string id3("567");

			Assert::IsTrue(storage1.Insert(id1, data1));
			Assert::IsTrue(storage1.Insert(id2, data2));
			Assert::IsFalse(storage1.Insert(id2, data2));
			Assert::IsTrue(storage1.Insert(id3, data3));

			std::string bad_id;

			Assert::IsTrue(storage1.FindValueByID(bad_id).IsEmpty());
			StorageData found_val = storage1.FindValueByID(id1);

			Assert::IsTrue(found_val.GetManufacturer() == data1.GetManufacturer());
			Assert::IsTrue(found_val.GetProductName() == data1.GetProductName());
			Assert::IsTrue(found_val.GetPrice() == data1.GetPrice());

			std::vector<std::pair<std::string, StorageData>> found_manuf = storage1.FindValuesByManufacturer(data2.GetManufacturer());
			Assert::IsTrue(found_manuf.size() == 2);
			Assert::IsTrue(found_manuf[0].first == id2);
			Assert::IsTrue(found_manuf[0].second.GetManufacturer() == data2.GetManufacturer());
			Assert::IsTrue(found_manuf[0].second.GetProductName() == data2.GetProductName());
			Assert::IsTrue(found_manuf[0].second.GetPrice() == data2.GetPrice());

			Assert::IsFalse(storage1.Erase(bad_id));
			Assert::IsTrue(storage1.Erase(id1));
		}

		TEST_METHOD(ThreadsTest)
		{
			//Initialize storage
			StorageData data1("Intel", "Corei7", 100);
			StorageData data2("AMD", "A4", 50);
			StorageData data3("AMD", "A6", 75);

			std::string id1("123");
			std::string id2("456");
			std::string id3("567");
			Storage stor_for_threads;

			stor_for_threads.Insert(id1, data1);
			stor_for_threads.Insert(id2, data2);
			stor_for_threads.Insert(id3, data3);

			std::vector<HANDLE> writers(3);
			std::vector<HANDLE> readers(5);
			//Create readers/writers
			for (auto iter = readers.begin(); iter != readers.end(); ++iter)
			{
				*iter = CreateThread(nullptr, 0L, SearchProc, &stor_for_threads, 0, nullptr);
				Assert::IsNotNull(*iter);
			}
			for (auto iter = writers.begin(); iter != writers.end(); ++iter)
			{
				*iter = CreateThread(nullptr, 0L, WriterProc, &stor_for_threads, 0, nullptr);
				Assert::IsNotNull(*iter);
			}

			//Wait for threads to finish
			readers.insert(readers.end(), writers.begin(), writers.end());
			DWORD status = WaitForMultipleObjects(readers.size(), readers.data(), TRUE, 20000);
			Assert::AreEqual(status, WAIT_OBJECT_0);
		}
	};
}