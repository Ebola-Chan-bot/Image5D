#pragma once
//此代码暂未使用
#include<type_traits>
#include<stdlib.h>
#include<string.h>
namespace 动态聚合
{
	enum class 异常
	{
		长数组不能赋值给短数组,
		动态数组的长度不能修改,
	};
	struct 动态类型
	{
		virtual size_t 字节数()const = 0;
	};
	//此类不允许构造，请从派生类或基础字节数据强制转换得到指针
	template<typename 元素类型>
		requires std::is_trivial_v<元素类型>
	struct 动态数组视图:public 动态类型
	{
		size_t 长度()const
		{
			return 长度;
		}
		//如果要修改长度，请确保支持此视图的派生类分配了足够的内存空间
		virtual void 长度(size_t 新长度)
		{
			_长度 = 新长度;
		}
		size_t 字节数()const override
		{
			return sizeof(动态数组视图) + _长度 * sizeof(元素类型);
		}
		动态数组视图() = delete;
		动态数组视图(const 动态数组视图&) = delete;
		//如果从较长的数组赋值给较短的数组，请确保此视图的派生类分配了足够的内存空间，否则将导致未定义行为
		动态数组视图& operator=(const 动态数组视图&原数组)
		{
			memmove(this, &原数组, 原数组.字节数());
			return *this;
		}
		//可以从指针创建引用以直接使用[]运算符
		元素类型& operator[](size_t 索引)
		{
			return ((元素类型*)(this + 1))[索引];
		}
		//可以从指针创建引用以直接使用[]运算符
		const 元素类型& operator[](size_t 索引)const
		{
			return ((元素类型*)(this + 1))[索引];
		}
		元素类型* begin()
		{
			return (元素类型*)(this + 1);
		}
		元素类型* end()
		{
			return begin() + _长度;
		}
		const 元素类型* cbegin()const
		{
			return begin();
		}
		const 元素类型* cend()const
		{
			return cend();
		}
	protected:
		virtual ~动态数组视图() {}
		size_t _长度;
	};
	//不允许构造对象，使用静态方法创建对象并获取指针
	template<typename 元素类型>
	struct 动态数组:public 动态数组视图<元素类型>
	{
		//使用delete删除指针
		static 动态数组* 创建(size_t 长度)
		{
			动态数组* 新数组 = (动态数组*)malloc(sizeof(动态数组) + sizeof(元素类型) * 长度);
			新数组->_长度 = 长度;
		}
		static void operator delete(void* 指针)
		{
			free(指针);
		}
		//不能从较长的数组赋值给较短的数组
		动态数组& operator=(const 动态数组& 原数组)
		{
			if (原数组._长度 > _长度)
				throw 异常::长数组不能赋值给短数组;
			memcpy(this, &原数组, 原数组.字节数());
		}
		//不能从较长的数组赋值给较短的数组
		动态数组& operator=(const 动态数组视图& 原数组)
		{
			if (原数组._长度 > _长度)
				throw 异常::长数组不能赋值给短数组;
			memmove(this, &原数组, 原数组.字节数());
		}
		void 长度(size_t 新长度)override
		{
			throw 异常::动态数组的长度不能修改;
		}
	};
	template<typename T>
	constexpr size_t 字节数(const T&)
	{
		return sizeof(T);
	}
	template<typename T>
		requires requires(const T& 对象) { 对象.字节数(); }
	size_t 字节数(const T& 对象)
	{
		return 对象.字节数();
	}
	template<typename...字段类型>
	struct 动态结构
	{
		static constexpr size_t 字段数 = sizeof...(字段类型);
		static 动态结构* 创建(const 字段类型&...字段值)
		{
			动态结构* 新对象 = (动态结构*)malloc(字节数(字段值) + ...);

		}
	};
	size_t B;
	void FF()
	{
		size_t A;
		auto C = 动态数组<int>::创建(10);
		const size_t AA[] = { 字节数(A),字节数(B),字节数(*C) };
	}
}