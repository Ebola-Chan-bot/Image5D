快速随机读写5D图像格式，目前支持读入 Olympus OIR 和读写 OME-TIFF，比市面上流行的竞品BioFormats要快得多。主要原因在于本库使用了更底层的C++指针操作，以及Windows内存映射文件机制，而BioFormats则使用Java，无法利用 Win32 API 优化。

5D格式泛指逻辑上具有 XYCZT 5 个维度的图像文件格式。在逻辑上，其内部图像文件是一个5D数组。但实现上，文件内部可能缺乏良好的索引，实际上不能根据文件头随机读写，仍然需要先遍历整个文件建立索引。有的格式甚至还会将大文件拆分成多个小文件，进一步增加了索引逻辑的复杂性。

本库经过精心设计，将复杂的底层逻辑隐藏，而将整个图像文件抽象成一个5D数组，允许用户随意切片，高速读写。核心是C++代码，提供MATLAB包装接口。后续根据需要可能添加其它语言接口。

依赖：[pugixml](https://pugixml.org/)，AVX2以上指令集，Windows系统（推荐Win11，低版本未测试）。本库涉及中文	C++函数名，代码文件采用UTF-8编码，建议将计算机语言编码也设为UTF-8（中国大陆电脑通常默认GB2312，这主要是兼容性包袱造成微软不敢轻易改动，但不建议在新的项目中再使用这种过时的编码了）

# 目录
- [部署指南](#部署指南)
- [Olympus OIR](#olympus-oir)
- [OME-TIFF](#ome-tiff)
	- [Tiff与BigTiff](#tiff与bigtiff)
	- [OME-TIFF](#ome-tiff-1)
	- [OB5-TIFF](#ob5-tiff)
	- [MATLAB接口类](#matlab接口类)
	- [辅助数据类型](#辅助数据类型)
	- [已知问题](#已知问题)
# 部署指南
本库的核心逻辑代码以静态库项目形式存在，可以方便地包装成面向各种语言的接口模块。存储库中还提供了面向MATLAB和C++的包装项目示例。
## 存储库结构
根目录是一个 Visual Studio 解决方案和一个MATLAB工程公用目录，用一套Git系统管理。

- +Image5D，MATLAB工具箱包目录，包
- doc，MATLAB工具箱快速入门文档
- Image5DLib，包含Image5D核心逻辑的静态库项目
- Image5DMex，包装Image5DLib，面向MATLAB，可编译为MEX文件函数的项目
- Image5D测试，包装Image5DLib，提供面向C++的示例项目和代码
- resources，MATLAB工程定义文件和MATLAB接口签名
- Image5D.prj，MATLAB工程主文件
- Image5D.sln，Visual Studio 解决方案主文件
- Image5D工具箱.prj，MATLAB工具箱打包工程
## 面向MATLAB的快速部署
最简单的方法就是直接安装编译好的工具箱。可以在[MATLAB附加功能管理器](https://ww2.mathworks.cn/matlabcentral/fileexchange/114435-image5d-oir-tiff)中搜索。此工具箱已经十分完善地包装好了各种接口。
## 手动包装静态库
无论面向MATLAB还是C++，你都需要一个C++项目来包装静态库。你需要对你的项目进行如下基本配置：
- 将`Image5DLib\include`中的头文件加入外部包含目录。这些头文件定义了使用静态库所必需的接口。
- 链接`Image5DLib.lib`文件；此外，还要额外再链接Windows提供的`Rpcrt4.lib`和`Bcrypt.lib`
- 安装`pugixml`依赖库，建议通过NuGet自动配置
- 使用C++20或更新的语言标准
- 静态库本身不能包含必要的资源，你需要手动将`Image5DLib\include`中的`TiffData模板.xml` `XML模板.xml`和`资源.rc`添加到你的包装项目中。如果你的包装项目中还有其它资源，不要创建新的rc文件，直接添加到已有rc文件中。
- 静态库不能自动初始化，使用任何其它API之前，你需要手动初始化。在包装项目的初始化入口处，唯一一次执行`Image5D::哈希初始化();`（包含`哈希计算器.h`）。对于DLL或MEX文件函数来说，这行代码通常应该放在`DllMain`函数的`DLL_PROCESS_ATTACH`分支；对于EXE来说，通常放在`main`函数中。对于单元测试项目，则可以放在测试类的构造函数中。
- 如果你需要读入OIR，`#include<Oir读入器.h>`；如果需要读写TIFF，`#include<IOmeTiff读写器.h>`。
- 注意，本库不是无异常的设计，可能会抛出`Image5D异常`。如果你的包装项目要求无异常，你将必须要处理所有的异常。
# Olympus OIR
这是Olympus显微镜默认的输出格式。文件虽然有内置索引，但分散在多文件序列中，且包含非像素信息，首次启动需要建立像素索引，稍慢，大约数秒。那之后会在同目录下建立索引文件，下次启动速度会加快。

本格式为Olympus私有格式，未公开详细文件规范，因此仅支持读入，不支持写出。

C++核心：Oir读入器

MATLAB接口类：Image5D.OirReader，可在MATLAB中用`doc Image5D.OirReader`命令查看详细文档，以下简要列出接口：
```MATLAB
classdef OirReader<handle
	%Olympus OIR 文件快速5D读入器，使用连续内存映射文件实现高速读入
	properties(Dependent)
		%图像宽度
		SizeX
		%图像高度
		SizeY
		%颜色通道数
		SizeC
		%Z层数
		SizeZ
		%采样时点数
		SizeT
		%每个采样时点的间隔毫秒数
		SeriesInterval
	end
	methods(Static)
		function ConcatenateByRename(HeaderPaths)
			%通过修改文件名的方法串联多个OIR文件序列
		end
		function SplitByRename(HeaderPath,SplitTable)
			%如果使用ConcatenateByRename发生了错误的合并并且已经无法撤销，可以尝试用此方法重新拆分开来
		end
	end
	methods
		function obj=OirReader(HeaderPath)
			%构造OirReader对象
		end
		function [Devices,Colors]=DeviceColors(obj)
			%取各通道的采样设备和颜色信息
		end
		function Pixels=ReadPixels(obj,TStart,TSize,ZStart,ZSize,CStart,CSize)
			%读入像素块值
		end
		function ReadToPointer(obj,Pointer,TStart,TSize,ZStart,ZSize,CStart,CSize)
			%读入像素块值到给定指针
		end
	end
end
```
# OME-TIFF
本库可以高速读写OME-TIFF文件。OME-TIFF 并不是一套单一的文件格式规范，实际上随着时间发展出了很多变体。
## TIFF与BigTiff
TIFF（Tag Image File Format，标签图像文件格式）是一种广泛使用的多图像存档文件格式，可以存储多张2D图像，每个像素也可以是1、2或4字节（称位深度或SizeP），它们线性排列在被称为IFD的维度（称I维度）上，每个IFD类似于视频的一“帧”，除了像素数值以外还包含高度（Y维度）、宽度（X维度）等元数据信息，每一项元数据被称为一个标签。TIFF规范规定了每个IFD都必须包含的，所谓必需标签，以及一些可选包含的标签。

经典的TIFF格式存在无法处理>4㎇文件的问题。BigTiff是TIFF格式的变体，解决了4㎇限制的问题。[TIFF和BigTiff格式规范](https://www.awaresystems.be/imaging/tiff.html)
## OME-TIFF
但是，很多时候，高、宽、IFD，这3个维度并不足以满足我们的需求，于是出现了OME（Open Microscopy Environment，开放显微镜环境）-TIFF扩展格式，将I维度扩展为C, Z, T三个维度：
- C，Channel，颜色通道。这里指的不是常见的RGB颜色通道，而是拍摄设备所支持的同时工作的记录设备。这些设备往往可以记录不同波长的光信号，包括但不限于RGB，甚至还包括非光学信号。
- Z，深度层级。许多拍摄设备可以对拍摄对象进行多层深度扫描拍摄，形成立体图像，因此需要增加一个Z维度。
- T，Time，时间轴。

这样，OME-TIFF实际上是具有XYCZT这5个维度的图像文件。为了与TIFF格式兼容，它利用了TIFF格式里可选的ImageDescription标签。OME-TIFF规定，在完全满足TIFF规范的基础上，首个IFD的ImageDescription标签变为必需标签。这个标签在TIFF规范中可以是任意的字符串，OME则规范了这个字符串必须为特定的OME-XML格式元数据，用于记录XYCZT各维度尺寸信息，各通道颜色（ChannelColor），各维度在存储空间中的线性排列顺序（DimensionOrder，谁为高维谁为低维），单个像素值的数据类型（PixelType），以及拍摄设备相关的元数据信息。[OME规范](https://docs.openmicroscopy.org/ome-model/6.2.2/ome-tiff/specification.html)

OME规范与TIFF/BigTiff的规范是正交的，即它既可以基于TIFF，也可以基于BigTiff。因此，传统的TIFF或BigTiff解析代码可以同样正确解读OME-TIFF，只是不能利用OME规范的额外功能而已。在传统代码眼中，OME-TIFF仍然只有XYI三个维度，其中I维度是由CZT三个维度展开而成。将I维度折叠成CZT三个维度所需的全部信息记录在ImageDescription中，折叠工作由支持OME规范的代码虚拟地完成，而不对I维度本身作任何改动。

但是，OME规范要求将5个维度的尺寸全部记录在首IFD标签中，这隐含地要求了所有IFD图像帧必须具有相同的高度、宽度和位深度；而一般的TIFF规范中无此要求，各个IFD可以独立设置各自的高度宽度。
## OB5-TIFF
上述TIFF格式及其各类变体，仍存在一个设计上的根本缺陷，即不支持随机访问。

文件中的字节是在地址空间中线性排列的，无论文件具有何种结构，这种结构都必须序列化为一串字节。TIFF规范在文件头中定义了首个IFD的字节偏移，即从文件起始开始向后多少个字节到达首个IFD；而IFD中的StripOffsets标签则记录了该IFD所对应的实际的像素数据的字节偏移；IFD结构的最后还记录了下一个IFD的字节偏移。也就是说，TIFF文件中的IFD维度并不是连续线性分布的，而是呈单向链表结构，只能从首IFD开始逐一寻找下一个IFD，直到最后一个IFD会指示它的下一个IFD的字节偏移为0，即本IFD是最后一个IFD。
链表结构的优势在于可以高效地插入和删除，但随机访问性能很差。虽然读取程序可以建立缓存来存储每个IFD实际像素数据的指针，但仍不能免于遍历目标IFD之前的所有IFD。
OB5是作者在OME-BigTiff基础上进一步严格格式规范：
- 紧接BigTiff文件头之后，字节偏移16的位置写入一个固定字节79，标志该文件遵守本规范
- IFD的标签可以是单个值，也可以指向文件的某个位置，表示该位置存储着该标签的值。因此首IFD的ImageDescription标签，在本规范中被要求指向所有IFD之前，紧接文件头之后。ImageDescription块与首IFD之间可以有任意大空隙，为后续扩展预留空间。
- 每个IFD都有且仅有13个标签。这些标签按照固定顺序排列：ImageDescription, StripOffsets, ImageWidth, ImageLength, BitsPerSample, RowsPerStrip, StripByteCounts, SampleFormat, Compression, PhotometricInterpretation, XResolution, YResolution, ResolutionUnit。因为每个标签恰好占用20字节且紧密线性排列，所以每个IFD占用的字节也是固定的。
- 所有IFD必须按照顺序紧密线性排列，因此可以根据首IFD偏移直接算出任何一个IFD的偏移，无需遍历。而ImageDescription中记录了CZT尺寸信息，因此可用SizeC×SizeZ×SizeT=SizeI直接算出IFD总数，因而算出所有IFD占用的字节总数。
- 所有IFD对应的实际像素数据块，也必须按照维度顺序紧密线性排列。因此根据首IFD的StripOffsets，以及在ImageDescription中读到的SizeX×SizeY×SizeP算出每个IFD对应像素数据占用的字节数，进一步算出任意一个IFD的指针，同样无需遍历。此外，内存连续的像素数据块可以看作一个5D数组，进行高效的索引和向量化、并行拷贝操作。
- 像素块在IFD块之后，两者之间可以有任意大空隙，便于IFD扩展预留空间。像素块到文件尾之间也可适当留空隙。

本格式的优势是具有高效的随机访问性能，无需遍历即可计算出所需数据的正确位置，一步直达。缺点是需要在写入数据之前预分配足够的文件空间，如果写入过程中发现空间不足，需要将后续数据成员全部向后移动，并更改所有的文件指针，性能开销较大。但这仅限于在写入前无从得知数据量的情况，如果能在写入前得知数据大小并预分配充分大的存储空间，本格式的写入性能仍高于传统TIFF。
## MATLAB接口类
看详细文档，以下简要列出接口：
```MATLAB
classdef OmeTiffRWer<handle
	%OME TIFF 快速5D读写器，使用内存映射文件和强制像素块连续实现高速读写
	properties(Dependent)
		%像素类型。如果文件符合OME标准，从 OME XML 中读入此属性；否则从首IFD中取得。假定所有IFD的像素类型与首IFD相同。
		PixelType Image5D.PixelType
		%维度顺序。仅 OME TIFF 支持此属性。
		DimensionOrder Image5D.DimensionOrder
		%像素字节数，根据PixelType计算得到。不能直接修改此属性，请修改PixelType。
		SizeP
		%图像宽度。如果文件符合OME标准，从 OME XML 中读入此属性；否则从首IFD中取得。假定所有IFD的图像宽度与首IFD相同。
		SizeX uint16
		%图像高度。如果文件符合OME标准，从 OME XML 中读入此属性；否则从首IFD中取得。假定所有IFD的图像高度与首IFD相同。
		SizeY uint16
		%IFD总数。如果文件符合OME标准，此值必等于SizeC*SizeZ*SizeT。不能直接修改此属性，请修改SizeC、SizeZ或SizeT。
		SizeI
		%颜色通道数。仅 OME TIFF 支持此属性。
		SizeC uint8
		%Z层数。仅 OME TIFF 支持此属性。
		SizeZ uint8
		%采样时点数。仅 OME TIFF 支持此属性。
		SizeT uint16
		%各通道颜色，每个通道对应一个Image5D.ChannelColor对象。仅 OME TIFF 支持此属性。
		ChannelColors Image5D.ChannelColor
		%文件名。仅 OME TIFF 支持此属性。此值从 OME XML 中读取，不一定与真实的文件名相同。
		FileName string
		%图像描述。只包含首IFD的图像描述。如果文件符合OME标准，此图像描述即为 OME XML 元数据。修改此属性将可能导致其它基本属性也被修改。
		ImageDescription string
	end
	methods(Static)
		function obj=OpenRead(FilePath)
			%只读打开现存TIFF文件
		end
		function obj=OpenRW(FilePath)
			%读写打开现存 OB5 TIFF 文件
		end
		function obj=Create(FilePath,PixelTypeOrImageDescription,SizeX,SizeY,ChannelColors,SizeZ,SizeT,DimensionOrder)
			%覆盖创建 OB5 TIFF 文件
		end
		function [Image,DimensionOrder,ChannelColors]=QuickRead(TiffPath)
			%快速读入Tiff图像
		end
		function QuickWrite(TiffPath,Image,DimensionOrder,ChannelColors)
			%快速写出Tiff图像
		end
	end
	methods
		function ModifyParameters(obj,PixelType,SizeX,SizeY,SizeC,SizeZ,SizeT,ChannelColors,DimensionOrder,FileName)
			%一次修改多个参数，只进行一次文件重构，因此比依次修改多个属性更快。
		end
		function Pixels=ReadPixels(obj,TStart,TSize,varargin)
			%读入指定范围内的像素值。仅 OME TIFF 支持此方法。
		end
		function Pixels=ReadPixelsI(obj,IStart,ISize)
			%读入一定IFD范围内的像素值
		end
		function WritePixels(obj,Pixels,TStart,TSize,varargin)
			%写出指定范围内的像素值
		end
		function WritePixelsI(obj,Pixels,IStart,ISize)
			%写出一定IFD范围内的像素值
		end
		function Pointer=PixelPointer(obj,T,Z,C)
			%返回一个指向像素值的指针。仅 OB5 TIFF 支持此方法。
		end
		function Pointer=PixelPointerI(obj,I)
			%返回一个指向像素值的指针。仅 OB5 TIFF 支持此方法。
		end
		function ReadToPointer(obj,Pointer,TStart,TSize,varargin)
			%读入像素块值到给定指针
		end
		function ReadToPointerI(obj,Pointer,IStart,ISize)
			%读入指定IFD范围内的像素块值到给定指针
		end
		function WriteFromPointer(obj,Pointer,TStart,TSize,varargin)
			%从给定指针写出到指定像素范围
		end
		function WriteFromPointerI(obj,Pointer,IStart,ISize)
			%从给定指针写出到指定IFD范围
		end
	end
end
```
## 辅助数据类型
以下类型定义用于主类的参数或输出结果等
```MATLAB
classdef ChannelColor<uint32
	%通道颜色，是将一个uint32从低到高拆分成ABGR四个uint8颜色通道的共用体。
	properties(Dependent)
		%透明通道
		A
		%蓝色通道
		B
		%绿色通道
		G
		%红色通道
		R
	end
	methods(Static)
		function obj = New(A,B,G,R)
			%创建新的ChannelColor对象。可以输入数组以一次性构造多个ChannelColor对象
		end
		function obj=FromOirColors(OirColors)
			%将OIR颜色通道矩阵转换为ChannelColor对象
		end
	end
end
classdef DimensionOrder<uint8
	%OME TIFF 规范定义了6种维度顺序，再加上一种DEFAULT表示缺省值
	enumeration
		XYCZT(0)
		XYCTZ(1)
		XYZCT(2)
		XYZTC(3)
		XYTCZ(4)
		XYTZC(5)
		%此项仅在OmeTiffRWer.ModifyParameters方法中使用，表示不修改维度顺序。
		DEFAULT(6)
	end
	methods
		function SP=SizePermute(obj)
			%取得该维度顺序的重排序数
		end
	end
end
classdef PixelType<uint8
	%OME TIFF 规范定义的8种像素类型，加上DEFAULT表示缺省值
	enumeration
		UINT8(0)
		UINT16(1)
		UINT32(2)
		INT8(3)
		INT16(4)
		INT32(5)
		FLOAT(6)
		DOUBLE(7)
		%此项仅在OmeTiffRWer.ModifyParameters方法中使用，表示不修改像素类型。
		DEFAULT(8)
	end
	methods
		function MT=MatlabType(obj)
			%将像素类型转换为MATLAB数据类型
		end
	end
end
```
## 已知问题
本库并不支持标准OME-TIFF规范的全部特性，包括但不限于：
- 仅支持Windows标准Little-endian字节序，不支持ImageJ输出的Big-endian字节序
- 不支持对非OB5-TIFF格式的写出，只能读入
- 不支持分多条带的TIFF像素块
- 不支持分多文件的OME-TIFF图像