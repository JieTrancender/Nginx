#pragma once

class NgxUnsetValue
{
public:
	template <typename T>
	operator T() const
	{
		return static_cast<T>(-1);
	}

	template <typename T>
	operator T*() const
	{
		return reinterpret_cast<T*>(-1);
	}

	//������
	static const NgxUnsetValue& get()
	{
		static NgxUnsetValue const v = {};
		return v;
	}
};
