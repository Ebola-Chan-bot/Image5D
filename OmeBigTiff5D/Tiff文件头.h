#pragma once
#include "Tiff概念定义.h"
template <版本 V>
class IFD迭代器
{
	using IFD = IFD<V>;
	using 迭代器 = IFD迭代器<V>;
	IFD* 指针;
	const void* 基地址;
public:
	IFD& operator*()const { return *指针; }
	迭代器& operator++()
	{
		指针 = 指针->下个IFD()(基地址);
		return *this;
	}
	迭代器 operator++(int)
	{
		迭代器 返回值 = *this;
		指针 = 指针->下个IFD()(基地址);
		return 返回值;
	}
	IFD* operator->()const { return 指针; }
	bool operator>(void* 比较指针) { return 指针 > 比较指针; }
	IFD迭代器(IFD* 指针, void* 基地址) :指针(指针), 基地址(基地址) {}
	operator bool() { return 指针 > 基地址; }
	bool operator<(void* 比较指针) { return 指针 < 比较指针; }
	IFD* operator+(int 偏移) { return 指针 + 偏移; }
};
enum 字节顺序 :UINT16
{
	从低到高 = 0x4949,
	从高到低 = 0x4D4D
};
template <版本 V>
struct Tiff文件头
{
	字节顺序 BO = 从低到高;
	版本 版本号;
};
template<>
struct Tiff文件头<小> :public Tiff文件头<基本>
{
	IFD偏移<小> 首IFD偏移;
	IFD迭代器<小> begin(void* 基地址)const { return IFD迭代器<小>(首IFD偏移(基地址), 基地址); }
};
template<>
struct Tiff文件头<大> :public Tiff文件头<基本>
{
	UINT16 偏移尺寸 = 8;
	UINT16 留空 = 0;
	IFD偏移<大> 首IFD偏移;
	IFD迭代器<大> begin(void* 基地址)const { return IFD迭代器<大>(首IFD偏移(基地址), 基地址); }
};