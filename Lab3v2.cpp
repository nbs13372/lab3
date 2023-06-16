#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <unordered_set>


/*!
	\brief Класс, задающий книгу в библиотеке
*/
class Book
{
public:

	std::string authorName; ///< Имя автора
	std::string bookName; ///< Название книги
	int year; ///< Год публикации
	int pages; ///< Число страниц


	/*!
		Создает объект Book
		\param[in] authorName Имя автора
		\param[in] bookName Название книги
		\param[in] year Год публикации
		\param[in] pages Число страниц
		\return Возвращает объект Book
	*/
	Book(std::string authorName = "", std::string bookName = "", int year = 1970, int pages = 300)
	{
		this->authorName = authorName;
		this->bookName = bookName;
		this->year = year;
		this->pages = pages;
	}

	/*!
		Оператор сравнения равно
		\param[in] right правое сравниваемое значение
		\return Возвращает true, если левый объект равен, false - иначе
	*/
	bool operator==(const Book& right) const
	{
		return this->authorName == right.authorName;
	}
};


/*!
	\brief Класс, задающий первый хэш
*/
class hash1
{
public:

	/*!
		Оператор взятия хэша от объекта
		\param[in] book ссылка на объект, от которого берется хэш
		\return Возвращает хэш объекта
	*/
	size_t operator()(const Book& book) const
	{
		size_t result = 0;
		int shifts[] = { 3, 7, 5, 11 };
		unsigned int* hashes = new unsigned int[book.authorName.size() / 4 + 1] {0};

		for (int i = 0; i < book.authorName.size(); i++)
		{
			hashes[i / 4] ^= book.authorName[i] << ((i & 7) * 8);
		}

		for (int i = 0; i < book.authorName.size() / 4 + 1; i += 2)
		{
			result ^= hashes[i] << shifts[i & 3];
		}

		for (int i = 1; i < book.authorName.size() / 4 + 1; i += 2)
		{
			result ^= hashes[i] >> shifts[i & 3];
		}

		return result;
	}
};

/*!
	\brief Класс, задающий первый хэш
*/
class hash2
{
public:
	/*!
		Оператор взятия хэша от объекта
		\param[in] book ссылка на объект, от которого берется хэш
		\return Возвращает хэш объекта
	*/
	size_t operator()(const Book& book) const
	{
		size_t result = 0;
		unsigned int b = 378551;
		unsigned int a = 63689;


		for (int i = 0; i < book.authorName.size(); i++)
		{
			result = result * 63689 + book.authorName[i];
			a *= b;
		}

		return result;
	}
};

/*!
		Основной метод программы
		\return Код завершения программы
*/
int main()
{
	int sizes[9] = { 100, 1000, 5000, 10000, 20000, 40000, 60000, 80000, 100000 };

	for (int size : sizes)
	{
		std::ifstream in;
		in.open("data_" + std::to_string(size) + ".txt");

		std::unordered_set<Book, hash1> data1;
		std::unordered_set<Book, hash2> data2;

		std::vector<unsigned int> hashes1;
		std::vector<unsigned int> hashes2;
		hash1 hasher1;
		hash2 hasher2;

		Book* data = new Book[size];

		for (int i = 0; i < size; i++)
		{
			std::string authorName, bookName;
			int year, pages;
			in >> authorName >> bookName >> year >> pages;

			data[i] = Book(authorName, bookName, year, pages);

			data1.insert(data[i]);
			data2.insert(data[i]);

			unsigned int hash1 = hasher1(data[i]);
			unsigned int hash2 = hasher2(data[i]);

			bool isHash1 = false;
			bool isHash2 = false;

			for (int j = 0; j < hashes1.size(); j++)
				if (hashes1[j] == hash1)
				{
					isHash1 = true;
					break;
				}

			for (int j = 0; j < hashes2.size(); j++)
				if (hashes2[j] == hash2)
				{
					isHash2 = true;
					break;
				}

			if (!isHash1)
				hashes1.push_back(hash1);

			if (!isHash2)
				hashes2.push_back(hash2);
		}

		in.close();

		int search_index = (int)(rand() / RAND_MAX * size);

		auto begin = std::chrono::high_resolution_clock::now();

		Book finded_1 = *data1.find(data[search_index]);

		auto end = std::chrono::high_resolution_clock::now();

		std::cout << "First hash searching " << size << " elements " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() << " ns" << std::endl;

		begin = std::chrono::high_resolution_clock::now();

		Book finded_2 = *data2.find(data[search_index]);

		end = std::chrono::high_resolution_clock::now();

		std::cout << "Second hash searching " << size << " elements " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count() << " ns" << std::endl;

		std::cout << "Collisions with first hash " << size - hashes1.size() << " on " << size << " elements" << std::endl;
		std::cout << "Collisions with second hash " << size - hashes2.size() << " on " << size << " elements" << std::endl;

		delete[] data;
	}
}


//Создание файлов с данными
/*std::string genString()
{
	int i = std::rand() % 50 + 10;

	std::string result = "";

	for (int k = 0; k < i; k++)
	{
		char l = std::rand() % 26 + 0x41;
		result.append(std::string(1, l));
	}

	return result;
}

std::string creatOne()
{
	return genString() + " " + genString() + " " + std::to_string(std::rand() % 50 + 1970) + " " + std::to_string(std::rand() % 999 + 1) + "\n";
}*/
