function Output = ExceptionCheck(OutputException)
if isstruct(OutputException)
	ExceptionType=Image5D.Image5DException(OutputException.Type);
	if ExceptionType~=Image5D.Image5DException.Successful_operation
		if OutputException.Code
			ExceptionType.Throw(sprintf("%s, internal: 0x%x",ExceptionType,OutputException.Code));
		else
			ExceptionType.Throw;
		end
	end
else
	Output=OutputException;
end