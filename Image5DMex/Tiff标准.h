#pragma once
#include<stdint.h>
enum Tiff版本 :uint16_t
{
	基本,
	小 = 42,
	大 = 43,
};
template <Tiff版本 V>
struct Word结构 {};
template<>
struct Word结构<小> { using 类型 = uint16_t; };
template<>
struct Word结构<大> { using 类型 = uint64_t; };
template <Tiff版本 V>
using Word = Word结构<V>::类型;
template <Tiff版本 V>
struct DWord结构 {};
template<>
struct DWord结构<小> { using 类型 = uint32_t; };
template<>
struct DWord结构<大> { using 类型 = uint64_t; };
template <Tiff版本 V>
using DWord = DWord结构<V>::类型;
template< Tiff版本 V, typename 对象类型>
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
enum SampleFormat :uint16_t
{
	SAMPLEFORMAT_UINT = 1,
	SAMPLEFORMAT_INT = 2,
	SAMPLEFORMAT_IEEEFP = 3,
	SAMPLEFORMAT_VOID = 4,
	SAMPLEFORMAT_COMPLEXINT = 5,
	SAMPLEFORMAT_COMPLEXIEEEFP = 6,
};
constexpr SampleFormat 像素类型采样格式[] = { SAMPLEFORMAT_UINT,SAMPLEFORMAT_UINT,SAMPLEFORMAT_UINT,SAMPLEFORMAT_INT,SAMPLEFORMAT_INT,SAMPLEFORMAT_INT,SAMPLEFORMAT_IEEEFP,SAMPLEFORMAT_IEEEFP };
enum class TagID :uint16_t
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
enum class TagType :uint16_t
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
enum class Compression :uint16_t
{
	NoCompression = 1,
	PackBits = 32773
};
enum class PhotometricInterpretation :uint16_t
{
	WhiteIsZero = 0,
	BlackIsZero = 1,
};
enum class ResolutionUnit :uint16_t
{
	NoUnit = 1,
	Inch = 2,
	Centimeter = 3,
};
#pragma pack(push,2)
struct Rational
{
	uint32_t Numerator;
	uint32_t Denominator;
};
struct SRational
{
	int32_t Numerator;
	int32_t Denominator;
};
template <Tiff版本 V>
struct Tag
{
	template <typename T>
	using 文件偏移 = 文件偏移<V, T>;
	TagID Identifier;
	TagType DataType;
	DWord<V> NoValues = 1;
	union
	{
		uint8_t BYTE值;
		char ASCII值;
		uint16_t SHORT值;
		uint32_t LONG值;
		Rational RATIONAL值;
		int8_t SBYTE值;
		DWord<V> UNDEFINED值;
		int16_t SSHORT值;
		int32_t SLONG值;
		SRational SRATIONAL值;
		float FLOAT值;
		double DOUBLE值;
		uint64_t LONG8值;
		int64_t SLONG8值;
		uint64_t IFD8值;
		SampleFormat SampleFormat枚举;
		Compression Compression枚举;
		PhotometricInterpretation PhotometricInterpretation枚举;
		ResolutionUnit ResolutionUnit枚举;
		文件偏移<uint8_t> BYTE偏移;
		文件偏移<char> ASCII偏移;
		文件偏移<uint16_t> SHORT偏移;
		文件偏移<uint32_t> LONG偏移;
		文件偏移<Rational> RATIONAL偏移;
		文件偏移<int8_t> SBYTE偏移;
		文件偏移<DWord<V>> UNDEFINED偏移;
		文件偏移<int16_t> SSHORT偏移;
		文件偏移<int32_t> SLONG偏移;
		文件偏移<SRational> SRATIONAL偏移;
		文件偏移<float> FLOAT偏移;
		文件偏移<double> DOUBLE偏移;
		文件偏移<uint64_t> LONG8偏移;
		文件偏移<int64_t> SLONG8偏移;
		文件偏移<uint64_t> IFD8偏移;
	};
};
template <Tiff版本 V>
struct IFD;
template <Tiff版本 V>
using IFD偏移 = 文件偏移<V, IFD<V>>;
template <Tiff版本 V>
struct IFD
{
	Word<V> 标签数;
	using Tag = Tag<V>;
	Tag* begin()const { return (Tag*)(this + 1); }
	Tag* end()const { return begin() + 标签数; }
	IFD偏移<V> 下个IFD()const { return *(IFD偏移<V>*)end(); }
};
#pragma pack(pop)