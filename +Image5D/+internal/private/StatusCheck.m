function StatusCheck(Status)
import Image5D.InnerExceptionType
if Status.ExceptionType
	Exception=Image5D.Image5DException(Status.ExceptionType);
	if Status.InnerException
		switch InnerExceptionType(Status.InnerException)
			case InnerExceptionType.Win32Exception
				InnerException=MATLAB.WindowsErrorCode(Status.ErrorCode);
			case InnerExceptionType.XmlException
				InnerException=Image5D.XmlErrorCode(Status.ErrorCode);
		end
		Exception.Throw(string(InnerException),Detail=InnerException);
	else
		Exception.Throw;
	end
end
end