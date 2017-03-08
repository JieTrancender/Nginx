#pragma once

#include "ngx_wrapper.hpp"
#include "ngx_exception.hpp"

#include <ngx_core.h>
#include <ngx_palloc.h>

//#include <winsock.h>
typedef unsigned char   u_char;

class NgxPool final : public NgxWrapper<ngx_pool_t>
{
public:
	typedef NgxWrapper<ngx_pool_t> super_type;
	typedef NgxPool this_type;
public:
	NgxPool(ngx_pool_t* p) :super_type(p)
	{}

	~NgxPool() = default;

	//ngx_pool_t�ٺݶ�Nginx�����ݽṹ���Գ�Ա����pool����ʽ���֣���������һ��ģ�幹�캯���Զ�ʹ�������Ա
	//ĩ�����T������ngx_http_request_t��ngx_conf_t�Ⱥ���pool��Ա�Ľṹ
	template <typename T>
	NgxPool(T* x) :NgxPool(x->pool)
	{}
public:
	//ngx_pool_t��ص��ڴ�������ͷź���
	//void* ngx_palloc(ngx_pool_t *pool, size_t size);
	//void* ngx_pnalloc(ngx_pool_t *pool, size_t size);
	//void* ngx_pcalloc(ngx_poll_t *poll, size_t size);
	//ngx_int_t ngx_pfree(ngx_pool_t *pool, void *p);

	//�����ڴ����뺯���Ĳ��
	//ngx_pallocʹ�����ڴ���룬�����ܻ����������ڴ��˷�
	//ngx_pnallocû��ʹ���ڴ����
	//ngx_pcalloc�ڲ�������ngx_palloc�����Ұ��ڴ������
	//��������������Ӧ��ʹ��ngx_pcalloc�����Ӱ�ȫ
	template <typename T, bool no_exception = false>
	T* palloc() const
	{
		auto p = ngx_pcalloc(get(), sizeof(T));

		if (!p)
		{
			//�Ƿ������׳��쳣
			if (no_exception)
			{
				return nullptr;
			}

			NgxException::raise();
		}

	    //������ǿ��ȫ��
		assert(p);
		return new (p) T();
	}

    //�׳��쳣�汾
	template <typename T>
	T* alloc() const
	{
		return palloc<T, false>();
	}

	//���׳��쳣�汾
	template <typename T>
	T* alloc_noexcept() const
	{
		return palloc<T, true>();
	}

	//����ָ���ֽ������ڴ�
	template <typename T>
	T* nalloc(std::size_t n) const
	{
		auto p = ngx_pnalloc(get(), n);

		//����ָ��
		NgxException::require(p);

		return reinterpret_cast<T*>(p);
	}

	//������������
	template <typename T, typename ... Args>
	T* construct(const Args& ... args) const
	{
		auto p = ngx_pcalloc(get(), sizeof(T));

		NgxException::require(p);

		//ת����������
		return new (p) T(args ...);
	}

	//��ngx_pool_cleanup_add()�ķ�װ
	//Nginx���ͷ���Դ�Ļ�������
	//1. ��ȡһ��ngx_pool_cleanup_t����
	//2. ��������handler��data
	//3. ������ṩdata����(nullptr)����ô�û�����Ҫ�ֹ�����
	template <typename F, typename T>
	ngx_pool_cleanup_t* cleanup(F func, T* data, std::size_t size = 0) const
	{
		auto p = ngx_poll_cleanup_add(get(), size);

		NgxException::require(p);

		//����������
		p->handler = func;

		if (data)
		{
			p->data = data;
		}

		//����������Ϣ����
		return p;
	}

	//��������Լ������õ���������Դ���ͱ������������������䵽Nginx�����������
	template <typename T>
	static void destory(void *p)
	{
		//ת��void*���ٵ�����������
		(reinterpret_cast<T*>(p))->~T();
	}
	
	//����cleanup
	//����ָ��ָ���ͷ��ڴ�ʱ�����õĺ���Ϊ��Ӧ�������������
	template <typename T>
	void cleanup(T* data) const
	{
		cleanup(&this_type::destory<T>, data);
	}

	//�ַ�������
	ngx_str_t dup(ngx_str_t& str) const
	{
		ngx_str_t  temp;

		temp.len = str.len;
		temp.data = ngx_pstrdup(get(), &str);

		NgxException::require(temp.data);
		return temp;
	}

	//������ͨ�ַ���
	ngx_str_t dup(boost::string_ref str) const
	{
		ngx_str_t temp{ str.size(), (u_char*)str.data() };
		return dup(temp);
	}
};
