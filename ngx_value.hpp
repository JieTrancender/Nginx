#pragma once

#include "ngx_unset_value.hpp"

class NgxValue final
{
public:
	NgxValue() = default;
	~NgxValue() = default;
public:
	template <typename T>
	static bool invalid(const T& v)
	{
		return v == static_cast<T>(NgxUnsetValue::get());
	}

	template <typename T, typename U>
	static void init(T& x, const U& v)
	{
		if (invalid(x))
		{
			x = v;
		}
	}

	template <typename T, typename U, typename V>
	static void merge(T& c, const U& p, const V& d)
	{
		if (invalid(c))
		{
			c = invalid(p) ? d : p;
		}
	}

	//�ɱ������Ϊδ��ʼ��״̬
	template <typename T, typename ... Args>
	static void unset(T& v, Args& ... args)
	{
		v = NgxUnsetValue::get();
		unset(args...);
	}

	//�ݹ��սắ��
	static void unset()
	{}
};
