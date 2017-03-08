#pragma once

#include <string>
#include <exception>

#include <boost/exception/all.hpp>
#include <boost/utility/string_ref.hpp>

#include <ngx_config.h>
#include <ngx_conf_file.h>

class NgxException final : public virtual std::exception, public virtual boost::exception
{
public:
	//Boost的轻量级字符串，没有拷贝代价
	typedef boost::string_ref string_ref_type;
private:
	ngx_int_t m_code = NGX_ERROR;
	std::string m_msg;
public:
	NgxException(ngx_int_t x, string_ref_type msg) :m_code(x), m_msg(msg)
	{}

	//委托构造函数
	NgxException(ngx_int_t x = NGX_ERROR):NgxException(x, "")
	{}

	NgxException(string_ref_type msg):NgxException(NGX_ERROR, msg)
	{}

	virtual ~NgxException() noexcept
	{}
public:
	ngx_int_t code() const
	{
		return m_code;
	}

	virtual const char* what() const noexcept override
	{
		return m_msg.c_str();
	}
public:
	//抛出异常函数
	static void raise(ngx_int_t rc = NGX_ERROR, string_ref_type msg = "")
	{
		throw NgxException(rc, msg);
	}

	//判断条件是否满足
	static void require(bool cond, ngx_int_t e = NGX_ERROR)
	{
		if (!cond)
		{
			raise(e);
		}
	}

	//检查错误码
	static void require(ngx_int_t rc, ngx_int_t x = NGX_OK)
	{
		require(rc == x, rc);
	}

	//检查空指针
	template <typename T>
	static void require(T* p, ngx_int_t e = NGX_ERROR)
	{
		require(p != nullptr, e);
	}

	static void fail(bool cond, ngx_int_t e = NGX_ERROR)
	{
		if (cond)
		{
			raise(e);
		}
	}
};