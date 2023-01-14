#pragma once
#include <stdint.h>
#include <memory>
#include <pugixml.hpp>
#include<Windows.h>
namespace Image5D
{
	enum 异常枚举
	{
		操作成功 = 0,
		文件打开失败,
		内存映射失败,
		文件不包含块,
		REF块不完整,
		XML块不完整,
		UID块不完整,
		像素块不完整,
		头文件不完整,
		图像属性不完整,
		图像属性解析失败,
		图像采集未定义,
		成像参数未定义,
		Z层尺寸未定义,
		Z层名称未定义,
		通道未定义,
		通道enable未定义,
		通道id未定义,
		通道order未定义,
		通道设备名未定义,
		扫描类型未定义,
		扫描设置未定义,
		扫描宽度未定义,
		扫描高度未定义,
		系列间隔未定义,
		找不到帧标头,
		找不到图像标头,
		找不到查找表,
		查找表解析失败,
		红色分量未定义,
		绿色分量未定义,
		蓝色分量未定义,
		空的像素块,
		未实现的方法,
		文件头不完整,
		不支持的版本号,
		头IFD不完整,
		图像描述不完整,
		图像描述解析失败,
		缺少OME节点,
		缺少UUID属性,
		缺少Image节点,
		缺少Name属性,
		缺少Pixels节点,
		缺少SizeX属性,
		缺少SizeY属性,
		缺少SizeC属性,
		缺少SizeZ属性,
		缺少SizeT属性,
		缺少DimensionOrder属性,
		维度顺序无效,
		缺少Type属性,
		颜色数与SizeC不匹配,
		缺少Color属性,
		像素类型无效,
		指定维度越界,
		SizeX为0,
		SizeY为0,
		SizeC为0,
		SizeZ为0,
		SizeT为0,
		构造参数无效,
		颜色指针为NULL,
		图像描述为NULL,
		修改参数无效,
		像素偏移超出文件范围,
		缺少必需标签,
		SizeCZT与SizeI不匹配,
		BitsPerSample与PixelType不匹配,
		图像属性未定义,
		图像相位未定义,
		相位通道未定义,
		图像配置未定义,
		Z层轴未定义,
		扫描参数未定义,
		图像尺寸未定义,
		扫描速度未定义,
		速度信息未定义,
		查找表未定义,
		无法创建索引文件,
		无法打开索引文件,
		索引文件损坏,
		图像文件不完整,
		文件指针设置失败,
		设备名称太长,
		只读打开不可写出,
		只读打开不可修改,
		索引中不包含块,
		通道长度未定义,
		内存拷贝失败,
		不透明度未定义,
		索引文件创建失败,
		Oir连续映射失败,
		Tiff文件创建失败,
		Tiff文件映射失败,
		找不到指定的文件,
		找不到指定的路径,
		文件拒绝访问,
		文件被另一个进程占用,
		磁盘已满,
		文件大小为0,
		不支持的字节顺序,
		输入参数个数错误,
		输出指针内存溢出,
		输入指针访问越界,
		输入数组元素太少,
		文件创建失败,
		销毁对象失败,
		调试断点,
		索引块偏移写出失败,
		内存访问冲突,
		找不到XML块,
		Oir文件头不完整,
		Oir基块索引不完整,
	};
	enum 内部异常类型
	{
		无 = 0,
		Image5D,
		Win32异常,
		XML异常
	};
	struct Image5D异常
	{
		异常枚举 异常类型;
		内部异常类型 内部异常;
		union
		{
			DWORD Win32错误代码;
			pugi::xml_parse_status XML错误代码;
			uint32_t 错误代码;
			const Image5D异常* 内部Image5D异常;
		};
		constexpr Image5D异常(异常枚举 异常类型) : 异常类型(异常类型), 内部异常(无), 错误代码(0) {}
		constexpr Image5D异常(异常枚举 异常类型, DWORD 错误代码) : 异常类型(异常类型), 内部异常(Win32异常), Win32错误代码(错误代码) {}
		constexpr Image5D异常(异常枚举 异常类型, pugi::xml_parse_status 错误代码) : 异常类型(异常类型), 内部异常(XML异常), XML错误代码(错误代码) {}
		constexpr Image5D异常(异常枚举 异常类型, const Image5D异常* 内部异常) : 异常类型(异常类型), 内部异常(Image5D), 内部Image5D异常(内部异常) {}
		operator bool()const { return 异常类型; }
	};
	constexpr Image5D异常 成功异常(操作成功);
	constexpr Image5D异常 越界异常(指定维度越界);
	constexpr Image5D异常 内存异常(内存拷贝失败);
	constexpr Image5D异常 参数异常(输入参数个数错误);
	constexpr Image5D异常 内存溢出(输出指针内存溢出);
}