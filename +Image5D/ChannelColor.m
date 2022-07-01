classdef ChannelColor<uint32
	%通道颜色，是将一个uint32从低到高拆分成ABGR四个uint8颜色通道的共用体。
	%本类不公开构造方法，需要使用静态方法New创建新对象。
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
	methods(Access=private)
		function obj=ChannelColor(Value)
			obj@uint32(Value);
		end
	end
	methods(Static)
		function obj = New(A,B,G,R)
			%创建新的ChannelColor对象。可以输入数组以一次性构造多个ChannelColor对象
			%# 语法
			% ```
			% import Image5D.ChannelColor
			%
			% obj=ChannelColor.New(A,B,G,R);
			% %使用不透明度和蓝绿红分量创建新ChannelColor。
			%
			% obj=ChannelColor.New(Value);
			% %从32位ABGR整数颜色值取得ChannelColor
			%
			% obj=ChannelColor.New(Matrix);
			% %从uint8或浮点ABGR颜色矩阵创建新ChannelColor
			% ```
			%# 示例
			% ```
			% Image5D.ChannelColor.New([1 1 1;1 0 0;0 1 0;0 0 1])
			% Image5D.ChannelColor.New([1 1 1],[1 0 0],[0 1 0],[0 0 1])
			% %上述两行代码完全等效，都生成3个颜色，依次为完全不透明的纯色蓝、绿、红
			% ```
			%# 输入参数
			% A，不透明度。如果为uint8类型，范围0~255；如果为single或double类型，范围0~1。可以用数组指定多个颜色的不透明度。
			% B，蓝色分量，数据类型和尺寸必须与A一致。
			% G，绿色分量，数据类型和尺寸必须与A一致。
			% R，红色分量，数据类型和尺寸必须与A一致。
			% Value，从int32或uint32形式的ABGR颜色共用体取得ChannelColor对象。可以用数组指定多个颜色。
			% Matrix，ABGR颜色矩阵，第1维依次排列ABGR通道，第2维是多个颜色。如果为uint8类型，范围0~255；如果为single或double类型，范围0~1
			%# 返回值
			% obj，新的ChannelColor对象。如果输入参数指定了多个颜色，obj总是列向量，无论输入数组尺寸为何。
			if nargin>=4
				A=[shiftdim(A,-1);shiftdim(B,-1);shiftdim(G,-1);shiftdim(R,-1)];
			end
			if isfloat(A)
				A=uint8(A*255);
			end
			obj=Image5D.ChannelColor(typecast(A(:),"uint32"));
		end
	end
	methods
		function Value=get.A(obj)
			Value=uint8(bitand(bitshift(uint32(obj),-0),0x000000ff));
		end
		function Value=get.B(obj)
			Value=uint8(bitand(bitshift(uint32(obj),-8),0x000000ff));
		end
		function Value=get.G(obj)
			Value=uint8(bitand(bitshift(uint32(obj),-16),0x000000ff));
		end
		function Value=get.R(obj)
			Value=uint8(bitand(bitshift(uint32(obj),-24),0x000000ff));
		end
		function obj=set.A(obj,Value)
			if isfloat(Value)
				Value=uint8(Value*255);
			end
			obj=Image5D.ChannelColor(bitand(uint32(obj),0xffffff00)+bitshift(uint32(Value),0));
		end
		function obj=set.B(obj,Value)
			if isfloat(Value)
				Value=uint8(Value*255);
			end
			obj=Image5D.ChannelColor(bitand(uint32(obj),0xffff00ff)+bitshift(uint32(Value),8));
		end
		function obj=set.G(obj,Value)
			if isfloat(Value)
				Value=uint8(Value*255);
			end
			obj=Image5D.ChannelColor(bitand(uint32(obj),0xff00ffff)+bitshift(uint32(Value),16));
		end
		function obj=set.R(obj,Value)
			if isfloat(Value)
				Value=uint8(Value*255);
			end
			obj=Image5D.ChannelColor(bitand(uint32(obj),0x00ffffff)+bitshift(uint32(Value),24));
		end
		function C=horzcat(obj,varargin)
			varargin=cellfun(@uint32,varargin,UniformOutput=false);
			C=Image5D.ChannelColor(horzcat(uint32(obj),varargin{:}));
		end
		function C=vertcat(obj,varargin)
			varargin=cellfun(@uint32,varargin,UniformOutput=false);
			C=Image5D.ChannelColor(vertcat(uint32(obj),varargin{:}));
		end
		function C=cat(obj,dim,varargin)
			varargin=cellfun(@uint32,varargin,UniformOutput=false);
			C=Image5D.ChannelColor(cat(dim,uint32(obj),varargin{:}));
		end
	end
end