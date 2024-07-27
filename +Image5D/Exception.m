classdef Exception<MATLAB.Lang.IEnumerableException
	%适用于Image5D包的异常枚举，指示文件格式无法识别或输入参数有误等异常情况
	enumeration
		Method_deprecated
		Unexpected_file_extension
		Unexpected_subsref_type
		DEFAULT_PixelType_cannot_convert_to_MATLAB_datatypes
		Wrong_number_of_parameters
	end
end