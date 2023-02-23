#pragma once
#include<stdint.h>
#include<type_traits>
namespace Image5D
{
#pragma pack(push,2)
	enum 字节序_e :uint16_t
	{
		从低到高 = 0x4949,
		从高到低 = 0x4D4D
	};
	template<字节序_e O>
	class 字节序_s
	{
		template<typename T>
		requires std::is_pod_v<T>
		static constexpr T R_v(T 输入)
		{
			T 输出 = 输入;
			if (O == 从高到低)
			{
				输出 = 0;
				T 掩码 = 0xff;
				int8_t 移位 = (sizeof(T) - 1) * 8;
				for (uint8_t a = 0; a < sizeof(T); ++a)
				{
					if (移位 > 0)
						输出 |= (输入 & 掩码) << 移位;
					else
						输出 |= (输入 & 掩码) >> -移位;
					掩码 <<= 8;
					移位 -= 16;
				}
			}
			return 输出;
		}
	public:
		//所有要写入文件的POD类型用此类包装，以支持不同字节序
		template<typename T>
		requires std::is_pod_v<T>
		class R_s
		{
			T 底层;
		public:
			constexpr R_s(T 表层) :底层(R_v(表层)) {}
			constexpr operator T()const noexcept { return R_v(底层); }
			R_s& operator+=(T 增量)noexcept
			{
				底层 = R_v(R_v(底层) + 增量);
				return *this;
			}
			R_s() {}
		};
		enum Tiff版本_e :uint16_t
		{
			基本,
			小 = R_v<uint16_t>(42),
			大 = R_v<uint16_t>(43),
		};
		template <Tiff版本_e V>
		struct 字
		{
			using Word = uint64_t;
			using DWord = uint64_t;
		};
		template<>
		struct 字<小>
		{
			using Word = uint16_t;
			using DWord = uint32_t;
		};
		enum SampleFormat :uint16_t
		{
			SAMPLEFORMAT_UINT = R_v<uint16_t>(1),
			SAMPLEFORMAT_INT = R_v<uint16_t>(2),
			SAMPLEFORMAT_IEEEFP = R_v<uint16_t>(3),
			SAMPLEFORMAT_VOID = R_v<uint16_t>(4),
			SAMPLEFORMAT_COMPLEXINT = R_v<uint16_t>(5),
			SAMPLEFORMAT_COMPLEXIEEEFP = R_v<uint16_t>(6),
		};
		static constexpr SampleFormat 像素类型采样格式[] = { SAMPLEFORMAT_UINT,SAMPLEFORMAT_UINT,SAMPLEFORMAT_UINT,SAMPLEFORMAT_INT,SAMPLEFORMAT_INT,SAMPLEFORMAT_INT,SAMPLEFORMAT_IEEEFP,SAMPLEFORMAT_IEEEFP };
		enum class TagID :uint16_t
		{
			ImageWidth = R_v<uint16_t>(256),
			ImageLength = R_v<uint16_t>(257),
			BitsPerSample = R_v<uint16_t>(258),
			Compression = R_v<uint16_t>(259),
			PhotometricInterpretation = R_v<uint16_t>(262),
			ImageDescription = R_v<uint16_t>(270),
			StripOffsets = R_v<uint16_t>(273),
			RowsPerStrip = R_v<uint16_t>(278),
			StripByteCounts = R_v<uint16_t>(279),
			XResolution = R_v<uint16_t>(282),
			YResolution = R_v<uint16_t>(283),
			ResolutionUnit = R_v<uint16_t>(296),
			SampleFormat = R_v<uint16_t>(339)
		};
		enum class TagType :uint16_t
		{
			BYTE = R_v<uint16_t>(1),
			ASCII = R_v<uint16_t>(2),
			SHORT = R_v<uint16_t>(3),
			LONG = R_v<uint16_t>(4),
			RATIONAL = R_v<uint16_t>(5),
			SBYTE = R_v<uint16_t>(6),
			UNDEFINED = R_v<uint16_t>(7),
			SSHORT = R_v<uint16_t>(8),
			SLONG = R_v<uint16_t>(9),
			SRATIONAL = R_v<uint16_t>(10),
			FLOAT = R_v<uint16_t>(11),
			DOUBLE = R_v<uint16_t>(12),
			LONG8 = R_v<uint16_t>(16),
			SLONG8 = R_v<uint16_t>(17),
			IFD8 = R_v<uint16_t>(18),
		};
		enum class Compression :uint16_t
		{
			NoCompression = R_v<uint16_t>(1),
			PackBits = R_v<uint16_t>(32773),
		};
		enum class PhotometricInterpretation :uint16_t
		{
			WhiteIsZero = R_v<uint16_t>(0),
			BlackIsZero = R_v<uint16_t>(1),
		};
		enum class ResolutionUnit :uint16_t
		{
			NoUnit = R_v<uint16_t>(1),
			Inch = R_v<uint16_t>(2),
			Centimeter = R_v<uint16_t>(3),
		};
		struct Rational
		{
			R_s<uint32_t>Numerator;
			R_s<uint32_t>Denominator;
		};
		struct SRational
		{
			R_s<int32_t>Numerator;
			R_s<int32_t>Denominator;
		};
		template<Tiff版本_e V>
		struct Tiff版本_s
		{
			using DWord = 字<V>::DWord;
			template<typename 对象类型>
			class 文件偏移
			{
				R_s<DWord>偏移;
			public:
				对象类型* operator()(const void* 基地址)const noexcept { return (对象类型*)((char*)基地址 + 偏移); }
				文件偏移() {}
				文件偏移& operator+=(DWord 增量)noexcept
				{
					偏移 += 增量;
					return *this;
				}
				operator DWord()const noexcept { return 偏移; }
				constexpr 文件偏移(DWord 偏移) :偏移(偏移) {}
			};
			struct Tag
			{
				template <typename T>
				using 文件偏移 = 文件偏移<T>;
				TagID Identifier;
				TagType DataType;
				R_s<DWord>NoValues;
				union
				{
					R_s<uint8_t>BYTE值;
					R_s<char>ASCII值;
					R_s<uint16_t>SHORT值;
					R_s<uint32_t>LONG值;
					R_s<int8_t>SBYTE值;
					R_s<DWord>UNDEFINED值;
					R_s<int16_t>SSHORT值;
					R_s<int32_t>SLONG值;
					R_s<float>FLOAT值;
					R_s<double>DOUBLE值;
					R_s<uint64_t>LONG8值;
					R_s<int64_t>SLONG8值;
					R_s<uint64_t>IFD8值;
					Rational RATIONAL值;
					SRational SRATIONAL值;
					SampleFormat SampleFormat枚举;
					Compression Compression枚举;
					PhotometricInterpretation PhotometricInterpretation枚举;
					ResolutionUnit ResolutionUnit枚举;
					文件偏移<R_s<uint8_t>> BYTE偏移;
					文件偏移<R_s<char>> ASCII偏移;
					文件偏移<R_s<uint16_t>> SHORT偏移;
					文件偏移<R_s<uint32_t>> LONG偏移;
					文件偏移<Rational> RATIONAL偏移;
					文件偏移<R_s<int8_t>> SBYTE偏移;
					文件偏移<R_s<DWord>> UNDEFINED偏移;
					文件偏移<R_s<int16_t>> SSHORT偏移;
					文件偏移<R_s<int32_t>> SLONG偏移;
					文件偏移<SRational> SRATIONAL偏移;
					文件偏移<R_s<float>> FLOAT偏移;
					文件偏移<R_s<double>> DOUBLE偏移;
					文件偏移<R_s<uint64_t>> LONG8偏移;
					文件偏移<R_s<int64_t>> SLONG8偏移;
					文件偏移<R_s<uint64_t>> IFD8偏移;
				};
			};
			struct IFD;
			using IFD偏移 = 文件偏移<IFD>;
			struct IFD
			{
				R_s<typename 字<V>::Word>标签数;
				Tag* begin()const noexcept { return (Tag*)(this + 1); }
				Tag* end()const noexcept{ return begin() + 标签数; }
				IFD偏移 下个IFD()const noexcept { return *(IFD偏移*)end(); }
				constexpr IFD(R_s<typename 字<V>::Word> 标签数) :标签数(标签数) {}
			};
			class IFD迭代器
			{
				IFD* 指针;
				const void* 基地址;
			public:
				IFD& operator*()const { return *指针; }
				IFD迭代器& operator++()noexcept
				{
					指针 = 指针->下个IFD()(基地址);
					return *this;
				}
				IFD迭代器 operator++(int)noexcept
				{
					IFD迭代器 返回值 = *this;
					指针 = 指针->下个IFD()(基地址);
					return 返回值;
				}
				IFD* operator->()const noexcept { return 指针; }
				bool operator>(void* 比较指针) noexcept { return 指针 > 比较指针; }
				IFD迭代器(IFD* 指针, const void* 基地址) :指针(指针), 基地址(基地址) {}
				operator bool()noexcept { return 指针 > 基地址; }
				bool operator<(void* 比较指针)noexcept { return 指针 < 比较指针; }
				IFD* operator+(int 偏移)noexcept { return 指针 + 偏移; }
			};
		};
		template <Tiff版本_e V>
		struct Tiff文件头
		{
			字节序_e BO = O;
			Tiff版本_e 版本号;
			constexpr Tiff文件头(Tiff版本_e 版本号) :版本号(版本号) {}
		};
		template<>
		struct Tiff文件头<小> :public Tiff文件头<基本>
		{
			Tiff版本_s<小>::IFD偏移 首IFD偏移;
			Tiff版本_s<小>::IFD迭代器 begin()const noexcept { return Tiff版本_s<小>::IFD迭代器(首IFD偏移(this), this); }
			Tiff文件头() :Tiff文件头<基本>(小) {}
		};
		template<>
		struct Tiff文件头<大> :public Tiff文件头<基本>
		{
			R_s<uint16_t>偏移尺寸;
			R_s<uint16_t>留空;
			Tiff版本_s<大>::IFD偏移 首IFD偏移;
			Tiff版本_s<大>::IFD迭代器 begin()const noexcept { return Tiff版本_s<大>::IFD迭代器(首IFD偏移(this), this); }
			Tiff文件头() :Tiff文件头<基本>(大) {}
		};
	};
#pragma pack(pop)
}