#include <vector>
#include <unordered_map>
#include "boost\thread.hpp"


template<typename Key, typename Value>
class Dictionary
{
private:
	std::unordered_map<Key, Value> values;
public:
	void Add(Key key, Value val)
	{
		if (!contains(key))
			values[key] = val;
	}
	bool Contains(Key key)
	{
		auto it = values.find(key);
		return it != values.end();
	}

	Value& operator[](Key key)
	{
		if (Contains(key))
			return values[key];
		else
			throw std::out_of_range("This key does not exist in the dictionary");
	}

	void Remove(Key key)
	{
		values.erase(key);
	}

	void Clear()
	{
		values.clear();
	}

};

template<typename T>
class ConcurrentVector
{
private:
	std::vector<T> values;
	boost::mutex valMutex;
public:
	void Add(T& val)
	{
		boost::lock_guard<boost::mutex> lock(valMutex);
		values.push_back(val);

	}

	void Remove(T& val)
	{
		boost::lock_guard<boost::mutex> lock(valMutex);
		for (auto it = values.begin(); it != values.end(); it++)
		if (*it == val)
		{
			values.erase(it);
			break;
		}
	}

	template<class Function>
	T& GetItem(Function predicate)
	{
		boost::lock_guard<boost::mutex> lock(valMutex);
		for (auto& v : values)
		if (predicate(v))
			return v;
		return T{};
	}

	template<class Function>
	void ForEach(Function f)
	{
		boost::lock_guard<boost::mutex> lock(valMutex);
		for (auto& v : values)
			f(v);
	}
};
