classdef SafePointer
	%此对象代表一个安全的C++指针，其Capacity属性可用于防止读写越界
	properties(SetAccess=immutable)
		%内存起始位置
		Pointer uint64
		%内存块容量。超出此容量的读写是非法的。
		Capacity uint64
	end
	methods
		function obj = SafePointer(Pointer,Capacity)
			%构造一个具有指定位置和容量的C++内存指针
			%# 语法
			% ```
			% obj=Image5D.SafePointer(Pointer,Capacity);
			% ```
			%# 输入参数
			% Pointer(1,1)uint64，内存指针位置
			% Capacity(1,1)uint64，内存块容量
			obj.Pointer=Pointer;
			obj.Capacity=Capacity;
		end
	end
end