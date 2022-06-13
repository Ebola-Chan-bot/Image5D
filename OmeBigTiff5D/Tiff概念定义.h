#pragma once
#include "Tiff版本.h"
template <版本 V>
struct Word结构 {};
template<>
struct Word结构<小> { using 类型 = UINT16; };
template<>
struct Word结构<大> { using 类型 = UINT64; };
template <版本 V>
using Word = Word结构<V>::类型;
template <版本 V>
struct DWord结构 {};
template<>
struct DWord结构<小> { using 类型 = UINT32; };
template<>
struct DWord结构<大> { using 类型 = UINT64; };
template <版本 V>
using DWord = DWord结构<V>::类型;
template <版本 V, typename 对象类型>
struct 文件偏移
{
	using 偏移类型 = DWord<V>;
	偏移类型 偏移;
	对象类型* operator()(const void* 基地址)const { return (对象类型*)((char*)基地址 + 偏移); }
	文件偏移& operator+=(偏移类型 增量)
	{
		偏移 += 增量;
		return *this;
	}
	偏移类型 operator+(偏移类型 增量)const { return 偏移 + 增量; }
	bool operator<(偏移类型 偏移) { return 文件偏移::偏移 < 偏移; }
};
enum SampleFormat :UINT16
{
	SAMPLEFORMAT_UINT = 1,
	SAMPLEFORMAT_INT = 2,
	SAMPLEFORMAT_IEEEFP = 3,
	SAMPLEFORMAT_VOID = 4,
	SAMPLEFORMAT_COMPLEXINT = 5,
	SAMPLEFORMAT_COMPLEXIEEEFP = 6,
};
constexpr SampleFormat 像素类型采样格式[] = { SAMPLEFORMAT_UINT,SAMPLEFORMAT_UINT,SAMPLEFORMAT_UINT,SAMPLEFORMAT_INT,SAMPLEFORMAT_INT,SAMPLEFORMAT_INT,SAMPLEFORMAT_IEEEFP,SAMPLEFORMAT_IEEEFP };
enum class TagID :UINT16
{
	ImageWidth = 256,
	ImageLength = 257,
	BitsPerSample = 258,
	Compression = 259,
	PhotometricInterpretation = 262,
	ImageDescription = 270,
	StripOffsets = 273,
	RowsPerStrip = 278,
	StripByteCounts = 279,
	XResolution = 282,
	YResolution = 283,
	ResolutionUnit = 296,
	SampleFormat = 339
};
enum class TagType :UINT16
{
	BYTE = 1,
	ASCII = 2,
	SHORT = 3,
	LONG = 4,
	RATIONAL = 5,
	SBYTE = 6,
	UNDEFINED = 7,
	SSHORT = 8,
	SLONG = 9,
	SRATIONAL = 10,
	FLOAT = 11,
	DOUBLE = 12,
	LONG8 = 16,
	SLONG8 = 17,
	IFD8 = 18
};
enum class Compression :UINT16
{
	NoCompression = 1,
	PackBits = 32773
};
enum class PhotometricInterpretation :UINT16
{
	WhiteIsZero = 0,
	BlackIsZero = 1,
};
enum class ResolutionUnit :UINT16
{
	NoUnit = 1,
	Inch = 2,
	Centimeter = 3,
};
#pragma pack(push,2)
struct Rational
{
	UINT32 Numerator;
	UINT32 Denominator;
};
struct SRational
{
	INT32 Numerator;
	INT32 Denominator;
};
template <版本 V>
struct Tag
{
	template <typename T>
	using 文件偏移 = 文件偏移<V, T>;
	TagID Identifier;
	TagType DataType;
	DWord<V> NoValues = 1;
	union
	{
		UINT8 BYTE值;
		char ASCII值;
		UINT16 SHORT值;
		UINT32 LONG值;
		Rational RATIONAL值;
		INT8 SBYTE值;
		DWord<V> UNDEFINED值;
		INT16 SSHORT值;
		INT32 SLONG值;
		SRational SRATIONAL值;
		float FLOAT值;
		double DOUBLE值;
		UINT64 LONG8值;
		INT64 SLONG8值;
		UINT64 IFD8值;
		SampleFormat SampleFormat枚举;
		Compression Compression枚举;
		PhotometricInterpretation PhotometricInterpretation枚举;
		ResolutionUnit ResolutionUnit枚举;
		文件偏移<UINT8> BYTE偏移;
		文件偏移<char> ASCII偏移;
		文件偏移<UINT16> SHORT偏移;
		文件偏移<UINT32> LONG偏移;
		文件偏移<Rational> RATIONAL偏移;
		文件偏移<INT8> SBYTE偏移;
		文件偏移<DWord<V>> UNDEFINED偏移;
		文件偏移<INT16> SSHORT偏移;
		文件偏移<INT32> SLONG偏移;
		文件偏移<SRational> SRATIONAL偏移;
		文件偏移<float> FLOAT偏移;
		文件偏移<double> DOUBLE偏移;
		文件偏移<UINT64> LONG8偏移;
		文件偏移<INT64> SLONG8偏移;
		文件偏移<UINT64> IFD8偏移;
	};
};
template <版本 V>
struct IFD;
template <版本 V>
using IFD偏移 = 文件偏移<V, IFD<V>>;
template <版本 V>
struct IFD
{
	Word<V> 标签数;
	using Tag = Tag<V>;
	Tag* begin()const { return (Tag*)(this + 1); }
	Tag* end()const { return begin() + 标签数; }
	IFD偏移<V> 下个IFD()const { return *(IFD偏移<V>*)end(); }
};
#pragma pack(pop)