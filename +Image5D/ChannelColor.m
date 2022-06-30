classdef ChannelColor<uint32
	properties(Dependent)
		A
		B
		G
		R
	end
	methods(Access=private)
		function obj=ChannelColor(Value)
			obj@uint32(Value);
		end
	end
	methods(Static)
		function obj = New(A,B,G,R)
			switch class(A)
				case "uint8"
					obj=Image5D.ChannelColor(typecast([A,B,G,R],"uint32"));
				case "int32"
					obj=Image5D.ChannelColor(typecast(A,"uint32"));
				case "uint32"
					obj=Image5D.ChannelColor(A);
				case {"single","double"}
					obj=Image5D.ChannelColor(typecast(uint8([A,B,G,R]*255),"uint32"));
			end
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
	end
end