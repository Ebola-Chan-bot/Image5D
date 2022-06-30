function Output = ExceptionCheck(OutputException)
if isstruct(OutputException)
	ExceptionType=Image5D.Image5DException(OutputException.Type);
	if ExceptionType~=Image5D.Image5DException.Successful_operation
		if OutputException.Code
			error("Image5D:Image5DException","%s, internal: 0x%x",ExceptionType,OutputException.Code);
		else
			error("Image5D:Image5DException",string(ExceptionType));
		end
	end
else
	Output=OutputException;
end