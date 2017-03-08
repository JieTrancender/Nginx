#pragma once

#include "nginx.hpp"
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

	//重载merge，实现ngx_str_t的条件赋值操作
	static void merge(ngx_str_t& c, const ngx_str_t& p, const ngx_str_t& d)
	{
		if (!c.data)
		{
			c = p.data ? p : d;
		}
	}

	//可变参数置为未初始化状态
	template <typename T, typename ... Args>
	static void unset(T& v, Args& ... args)
	{
		v = NgxUnsetValue::get();
		unset(args...);
	}

	//递归终结函数
	static void unset()
	{}
};
