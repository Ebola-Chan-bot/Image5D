#include "CppUnitTest.h"
#include <IOmeTiff读写器.h>
#include <Image5D资源.h>
#include<random>
#include<哈希计算器.h>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Image5D;
namespace Image5D测试
{
	TEST_CLASS(Image5D测试)
	{
	public:

		TEST_METHOD(Tiff文件写出测试)
		{
			//先生成图像数据
			constexpr uint16_t 宽度 = 512;
			constexpr uint16_t 高度 = 512;
			constexpr uint16_t 层数 = 100;
			constexpr uint32_t 像素数 = 宽度 * 高度 * 层数;
			uint16_t* 像素数组 = new uint16_t[像素数];
			//填充随机像素值
			std::generate_n(像素数组, 像素数, []()
				{
					static	std::random_device rd;   // non-deterministic generator
			static std::mt19937 gen(rd());  // to seed mersenne twister.
			static std::uniform_int_distribution<> dist(0, 65535); // distribute results between 1 and 6 inclusive.
			return dist(gen);
				});
			const 颜色 通道颜色[] = { 常用颜色::绿色 }; //-1表示白色
			IOmeTiff读写器* const 读写器 = IOmeTiff读写器::覆盖创建(L"D:\\随机TIFF.tif", 数据类型转像素类型<std::remove_pointer_t<decltype(像素数组)>>, 宽度, 高度, 1, 层数, 1, 通道颜色, 维度顺序::XYCZT);
			读写器->写出像素(像素数组); //像素数组应该是uint16_t[]类型，而不是uint16_t[][][]类型。如果你的原始数据是后者，应该将其展开为一维数组，按照维度从低到高XYZ的顺序。
			delete 读写器;
		}
		//首次加载该模块必须先执行哈希初始化
		Image5D测试() { 哈希初始化(); }
	};
}
