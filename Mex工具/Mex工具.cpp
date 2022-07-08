#include "pch.h"
#include "Mex工具.h"
ArrayFactory 数组工厂;
template<>
String 万能转码<String>(Array& 输入)
{
	switch (输入.getType())
	{
	case ArrayType::CHAR:
		return CharArray(std::move(输入)).toUTF16();
	case ArrayType::MATLAB_STRING:
		return (StringArray(std::move(输入))[0]);
	default:
		throw InvalidArrayTypeException("输入不是字符串");
	}
}
template String 万能转码<String>(Array& 输入);
template<>
CharArray 万能转码<CharArray>(Array& 输入)
{
	switch (输入.getType())
	{
	case ArrayType::CHAR:
		return std::move(输入);
	case ArrayType::MATLAB_STRING:
		return 数组工厂.createCharArray(String(StringArray(std::move(输入))[0]));
	default:
		throw InvalidArrayTypeException("输入不是字符串");
	}
}
template CharArray 万能转码<CharArray>(Array& 输入);
void 异常输出补全(ArgumentList& outputs)
{
	std::vector<Array>::iterator 输出头 = outputs.begin();
	const std::vector<Array>::iterator 输出尾 = outputs.end();
	while (++输出头 < 输出尾)
		*输出头 = 数组工厂.createEmptyArray();
}
template<>
std::string 万能转码<std::string>(Array& 输入)
{
	const String 字符串 = 万能转码<String>(输入);
	std::string 返回;
	返回.resize_and_overwrite(字符串.size() * 3, [&字符串](char* 指针, size_t 尺寸)
		{
			return WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)字符串.c_str(), -1, 指针, 尺寸, nullptr, nullptr);
		});
	return 返回;
}
template std::string 万能转码<std::string>(Array& 输入);
template<>
String 万能转码<String>(const char* 输入)
{
	String 返回;
	const size_t 长度 = strlen(输入)+1;
	返回.resize_and_overwrite(长度, [输入](char16_t* 指针, size_t 尺寸)
		{
#ifdef _DEBUG
			尺寸 = MultiByteToWideChar(CP_UTF8, 0, 输入, -1, (wchar_t*)指针, 尺寸);
			return 尺寸;
#else
			return MultiByteToWideChar(CP_UTF8, 0, 输入, -1, (wchar_t*)指针, 尺寸);
#endif
		});
	return 返回;
}
template String 万能转码<String>(const char*);
//定义移动
void mexFunctionAdapter(int nlhs_,
	int nlhs,
	int nrhs,
	void* vrhs[],
	void* mFun,
	void (*callbackOutput)(int, void**),
	void (*callbackErrHandler)(const char*, const char*)) {

	matlab::mex::Function* mexFunction = reinterpret_cast<matlab::mex::Function*>(mFun);

	std::vector<matlab::data::Array> edi_prhs;
	edi_prhs.reserve(nrhs);
	implToArray(nrhs, vrhs, edi_prhs);

	std::vector<matlab::data::Array> edi_plhs(nlhs);
	matlab::mex::ArgumentList outputs(edi_plhs.begin(), edi_plhs.end(), nlhs_);
	matlab::mex::ArgumentList inputs(edi_prhs.begin(), edi_prhs.end(), nrhs);

	try {
		(*mexFunction)(outputs, inputs);
	}
	catch (...) {
		mexHandleException(callbackErrHandler);
		return;
	}

	arrayToImplOutput(nlhs, edi_plhs, callbackOutput);
}
void mexFunction() {}
void mexDestroyMexFunction(void* mexFunc,
	void (*callbackErrHandler)(const char*, const char*)) {
	matlab::mex::Function* mexFunction = reinterpret_cast<matlab::mex::Function*>(mexFunc);
	try {
		mexDestructorUtil(mexFunction);
	}
	catch (...) {
		mexHandleException(callbackErrHandler);
		return;
	}
}
void* mexCreateMexFunction(void (*callbackErrHandler)(const char*, const char*)) {
	try {
		matlab::mex::Function* mexFunc = mexCreatorUtil<MexFunction>();
		return mexFunc;
	}
	catch (...) {
		mexHandleException(callbackErrHandler);
		return nullptr;
	}
}
void mexHandleException(void (*callbackErrHandler)(const char*, const char*)) {
	try {
		throw;
	}
	catch (const matlab::engine::MATLABException& ex) {
		callbackErrHandler(ex.what(), ex.getMessageID().c_str());
	}
	catch (const matlab::engine::Exception& ex) {
		callbackErrHandler(ex.what(), "");
	}
	catch (const matlab::Exception& ex) {
		callbackErrHandler(ex.what(), "");
	}
	catch (const std::exception& ex) {
		callbackErrHandler(ex.what(), "");
	}
	catch (...) {
		callbackErrHandler("Unknown exception thrown.", "");
	}
}
void mexDestructorUtil(matlab::mex::Function* t) {
	delete t;
}
void implToArray(int na, void* va[], std::vector<matlab::data::Array>& pa) {
	assert(na == static_cast<int>(pa.capacity()));

	for (int i = 0; i < na; i++) {
		matlab::data::impl::ArrayImpl* impl = reinterpret_cast<matlab::data::impl::ArrayImpl*>(va[i]);
		pa.push_back(matlab::data::detail::Access::createObj<matlab::data::Array>(impl));
	}
}
void implDeleter(matlab::data::impl::ArrayImpl** impl) {
	if (impl != nullptr)
		free(impl);
}
void arrayToImplOutput(int nlhs, std::vector<matlab::data::Array>& edi_plhs, void (*callbackOutput)(int, void**)) {
	assert(nlhs == static_cast<int>(edi_plhs.size()));
	std::unique_ptr<matlab::data::impl::ArrayImpl*, void(*)(matlab::data::impl::ArrayImpl**)> vlhsPtr(new matlab::data::impl::ArrayImpl * [nlhs], [](matlab::data::impl::ArrayImpl** ptr) {
		delete[] ptr;
		});
	void** vlhs = (void**)vlhsPtr.get();
	arrayToImpl(nlhs, vlhs, edi_plhs);
	callbackOutput(nlhs, vlhs);
}
void arrayToImpl(int na, void* va[], const std::vector<matlab::data::Array>& pa) {
	for (int i = 0; i < na; i++) {
		va[i] = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(pa[i]);
	}
}
matlab::mex::Function::Function() {
	functionImpl = mexGetFunctionImpl();
}
void matlab::mex::Function::mexLock() {
	int errID = 0;
	cppmex_mexLock_with_error_check(functionImpl, &errID);
	throwIfError(errID, nullptr);
}
void matlab::mex::Function::mexUnlock() {
	int errID = 0;
	cppmex_mexUnlock_with_error_check(functionImpl, &errID);
	throwIfError(errID, nullptr);
}
void matlab::engine::MATLABEngine::setVariable(const std::u16string& varName,
	const matlab::data::Array& var,
	matlab::engine::WorkspaceType workspaceType) {
	const char* workspace = (workspaceType == matlab::engine::WorkspaceType::BASE) ? "base" : "global";
	void* mexcept = nullptr;

	void* varImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(var);

	int errID = 0;
	mexApiSetVariable(pImpl, varImpl, workspace, varName.c_str(), &errID, &mexcept);

	throwIfError(errID, mexcept);
}
void matlab::engine::MATLABEngine::setVariable(const std::string& varName,
	const matlab::data::Array& var,
	matlab::engine::WorkspaceType workspaceType) {
	setVariable(std::u16string(varName.begin(), varName.end()), var, workspaceType);
}
void matlab::engine::MATLABEngine::setProperty(matlab::data::Array& object,
	size_t index,
	const std::u16string& propertyName,
	const matlab::data::Array& value) {
	void* mexcept = nullptr;
	void* objImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(object);
	void* propertyImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(value);

	int errID = 0;
	void* impl = mexApiSetProperty(pImpl, objImpl, index, propertyName.c_str(), propertyImpl, &errID, &mexcept);

	throwIfError(errID, mexcept);

	object = matlab::data::detail::Access::createObj<matlab::data::Array>(reinterpret_cast<matlab::data::impl::ArrayImpl*>(impl));
}

void matlab::engine::MATLABEngine::setProperty(matlab::data::Array& object,
	size_t index,
	const std::string& propertyName,
	const matlab::data::Array& value) {
	setProperty(object, index, std::u16string(propertyName.begin(), propertyName.end()), value);
}

void matlab::engine::MATLABEngine::setProperty(matlab::data::Array& object,
	const std::u16string& propertyName,
	const matlab::data::Array& value) {
	setProperty(object, 0, propertyName, value);
}

void matlab::engine::MATLABEngine::setProperty(matlab::data::Array& object,
	const std::string& propertyName,
	const matlab::data::Array& value) {
	setProperty(object, 0, std::u16string(propertyName.begin(), propertyName.end()), value);
}
void matlab::engine::MATLABEngine::eval(const std::u16string& str,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	matlab::data::ArrayFactory factory;
	auto mStr = factory.createScalar(str);

	feval(matlab::engine::convertUTF8StringToUTF16String("eval"), 0, std::vector<matlab::data::Array>({ mStr }), output, error);
}
matlab::mex::Function::~Function() NOEXCEPT_FALSE {
	mexDestroyFunctionImpl(functionImpl);
}
std::shared_ptr<matlab::engine::MATLABEngine> matlab::mex::Function::getEngine() {
	void* engine = cppmex_getEngine(functionImpl);
	return std::make_shared<matlab::engine::MATLABEngine>(engine);
}
void matlab::mex::Function::operator()(matlab::mex::ArgumentList& outs, matlab::mex::ArgumentList& ins) {}




std::u16string matlab::mex::Function::getFunctionName() const {
	const char16_t* fn = cppmex_getFunctionName(functionImpl);

	if (!fn) {
		std::string outOfMemoryError = "Not enough memory available to support the request.";
		throw matlab::engine::Exception(outOfMemoryError);
	}

	char16_t* fName = const_cast<char16_t*>(fn);
	std::u16string fNameStr = std::u16string(fName);
	mexReleaseMemory(nullptr, fName);
	return fNameStr;
}

/*** matlab::engine::MATLABEngine ***/
std::vector<matlab::data::Array> matlab::engine::MATLABEngine::feval(const std::u16string& function,
	const int nlhs,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	int nrhs = static_cast<int>(args.size());

	matlab::data::impl::ArrayImpl** plhs_arr_impl = (nlhs == 0) ? nullptr : (matlab::data::impl::ArrayImpl**)malloc(sizeof(matlab::data::impl::ArrayImpl*) * nlhs);
	std::unique_ptr<matlab::data::impl::ArrayImpl*, void(*)(matlab::data::impl::ArrayImpl**)> plhs_impl_guard(plhs_arr_impl, &implDeleter);

	matlab::data::impl::ArrayImpl** args_arr_impl = (nrhs == 0) ? nullptr : (matlab::data::impl::ArrayImpl**)malloc(sizeof(matlab::data::impl::ArrayImpl*) * nrhs);
	std::unique_ptr<matlab::data::impl::ArrayImpl*, void(*)(matlab::data::impl::ArrayImpl**)> args_impl_guard(args_arr_impl, &implDeleter);

	void** plhs_impl = reinterpret_cast<void**>(plhs_arr_impl);
	void** args_impl = reinterpret_cast<void**>(args_arr_impl);

	arrayToImpl(nrhs, args_impl, args);

	void* mexcept = nullptr;

	std::string function_utf8 = matlab::engine::convertUTF16StringToUTF8String(function);

	void* output_ = output ? new std::shared_ptr<StreamBuffer>(std::move(output)) : nullptr;
	void* error_ = error ? new std::shared_ptr<StreamBuffer>(std::move(error)) : nullptr;

	int errID = 0;
	mexApiFeval(pImpl, nlhs, plhs_impl, nrhs, args_impl, function_utf8.c_str(), &errID, &mexcept, output_, error_, &writeToStreamBuffer, &deleteStreamBufferImpl);

	throwIfError(errID, mexcept);

	std::vector<matlab::data::Array> plhs;
	plhs.reserve(nlhs);
	implToArray(nlhs, plhs_impl, plhs);

	return plhs;
}

std::vector<matlab::data::Array> matlab::engine::MATLABEngine::feval(const std::string& function,
	const int nlhs,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return feval(std::u16string(function.begin(), function.end()), nlhs, args, output, error);
}

matlab::data::Array matlab::engine::MATLABEngine::feval(const std::u16string& function,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {

	std::vector<matlab::data::Array> out_vec = feval(function, 1, args, output, error);
	return out_vec.at(0);
}

matlab::data::Array matlab::engine::MATLABEngine::feval(const std::string& function,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return feval(std::u16string(function.begin(), function.end()), args, output, error);
}

matlab::data::Array matlab::engine::MATLABEngine::feval(const std::u16string& function,
	const matlab::data::Array& arg,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return feval(function, std::vector<matlab::data::Array>({ arg }), output, error);
}

matlab::data::Array matlab::engine::MATLABEngine::feval(const std::string& function,
	const matlab::data::Array& arg,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return feval(std::u16string(function.begin(), function.end()), std::vector<matlab::data::Array>({ arg }), output, error);
}

matlab::data::Array matlab::engine::MATLABEngine::getVariable(const std::string& varName,
	matlab::engine::WorkspaceType workspaceType) {
	return getVariable(std::u16string(varName.begin(), varName.end()), workspaceType);
}





matlab::data::Array matlab::engine::MATLABEngine::getProperty(const matlab::data::Array& object,
	size_t index,
	const std::u16string& propertyName) {
	void* mexcept = nullptr;
	void* objImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(object);

	int errID = 0;
	void* impl = mexApiGetProperty(pImpl, objImpl, index, propertyName.c_str(), &errID, &mexcept);

	throwIfError(errID, mexcept);

	return matlab::data::detail::Access::createObj<matlab::data::Array>(reinterpret_cast<matlab::data::impl::ArrayImpl*>(impl));
}

matlab::data::Array matlab::engine::MATLABEngine::getProperty(const matlab::data::Array& object,
	size_t index,
	const std::string& propertyName) {
	return getProperty(object, index, std::u16string(propertyName.begin(), propertyName.end()));
}

matlab::data::Array matlab::engine::MATLABEngine::getProperty(const matlab::data::Array& object,
	const std::u16string& propertyName) {
	return getProperty(object, 0, propertyName);
}

matlab::data::Array matlab::engine::MATLABEngine::getProperty(const matlab::data::Array& object,
	const std::string& propertyName) {
	return getProperty(object, 0, std::u16string(propertyName.begin(), propertyName.end()));
}
matlab::engine::FutureResult<void> matlab::engine::MATLABEngine::evalAsync(const std::u16string& str,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	std::promise<void>* p = new std::promise<void>();
	std::future<void> f = p->get_future();
	void* output_ = output ? new std::shared_ptr<StreamBuffer>(std::move(output)) : nullptr;
	void* error_ = error ? new std::shared_ptr<StreamBuffer>(std::move(error)) : nullptr;
	uintptr_t handle = mexApiEvalAsync(pImpl, str.c_str(), &set_eval_promise_data, &set_eval_promise_exception, p, output_, error_, &writeToStreamBuffer, &deleteStreamBufferImpl);
	return matlab::engine::FutureResult<void>(std::move(f), std::make_shared<matlab::engine::TaskReference>(handle, mexApiCancelFevalWithCompletion));
}

matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::getVariableAsync(const std::u16string& varName,
	WorkspaceType workspaceType) {
	std::promise<matlab::data::Array>* p = new std::promise<matlab::data::Array>();
	std::future<matlab::data::Array> f = p->get_future();
	const char* workspace = (workspaceType == matlab::engine::WorkspaceType::BASE) ? "base" : "global";
	uintptr_t handle = mexApiGetVariableAsync(pImpl, workspace, varName.c_str(), &set_feval_promise_data, &set_feval_promise_exception, p);
	return matlab::engine::FutureResult<matlab::data::Array>(std::move(f), std::make_shared<matlab::engine::TaskReference>(handle, mexApiCancelFevalWithCompletion));
}

matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::getVariableAsync(const std::string& varName,
	WorkspaceType workspaceType) {
	return getVariableAsync(std::u16string(varName.begin(), varName.end()), workspaceType);
}

matlab::engine::FutureResult<void> matlab::engine::MATLABEngine::setVariableAsync(const std::u16string& varName,
	const matlab::data::Array& var,
	WorkspaceType workspaceType) {
	std::promise<void>* p = new std::promise<void>();
	std::future<void> f = p->get_future();
	const char* workspace = (workspaceType == matlab::engine::WorkspaceType::BASE) ? "base" : "global";
	matlab::data::impl::ArrayImpl* varImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(var);
	uintptr_t handle = mexApiSetVariableAsync(pImpl, varImpl, workspace, varName.c_str(), &set_feval_promise_data, &set_feval_promise_exception, p);
	return matlab::engine::FutureResult<void>(std::move(f), std::make_shared<matlab::engine::TaskReference>(handle, mexApiCancelFevalWithCompletion));
}

matlab::engine::FutureResult<void> matlab::engine::MATLABEngine::setVariableAsync(const std::string& varName,
	const matlab::data::Array& var,
	WorkspaceType workspaceType) {
	return setVariableAsync(std::u16string(varName.begin(), varName.end()), var, workspaceType);
}

matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::getPropertyAsync(const matlab::data::Array& object,
	size_t index,
	const std::u16string& propertyName) {
	std::promise<matlab::data::Array>* p = new std::promise<matlab::data::Array>();
	std::future<matlab::data::Array> f = p->get_future();
	matlab::data::impl::ArrayImpl* objectImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(object);
	uintptr_t handle = mexApiGetPropertyAsync(pImpl, objectImpl, index, propertyName.c_str(), &set_feval_promise_data, &set_feval_promise_exception, p);
	return matlab::engine::FutureResult<matlab::data::Array>(std::move(f), std::make_shared<matlab::engine::TaskReference>(handle, mexApiCancelFevalWithCompletion));
}

matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::getPropertyAsync(const matlab::data::Array& object,
	size_t index,
	const std::string& propertyName) {
	return getPropertyAsync(object, index, std::u16string(propertyName.begin(), propertyName.end()));
}

matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::getPropertyAsync(const matlab::data::Array& object,
	const std::u16string& propertyName) {
	return getPropertyAsync(object, 0, propertyName);
}

matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::getPropertyAsync(const matlab::data::Array& object,
	const std::string& propertyName) {
	return getPropertyAsync(object, std::u16string(propertyName.begin(), propertyName.end()));
}

matlab::engine::FutureResult<void> matlab::engine::MATLABEngine::setPropertyAsync(matlab::data::Array& object,
	size_t index,
	const std::u16string& propertyName,
	const matlab::data::Array& value) {
	std::promise<matlab::data::Array>* p = new std::promise<matlab::data::Array>();
	std::future<matlab::data::Array> f = p->get_future();
	matlab::data::impl::ArrayImpl* objectImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(object);
	matlab::data::impl::ArrayImpl* propImpl = matlab::data::detail::Access::getImpl<matlab::data::impl::ArrayImpl>(value);
	uintptr_t handle = mexApiSetPropertyAsync(pImpl, objectImpl, index, propertyName.c_str(), propImpl, &set_feval_promise_data, &set_feval_promise_exception, p);
	auto copyableF = std::make_shared<std::future<matlab::data::Array>>(std::move(f));
	auto convertToResultType = [&object, copyableF]() {
		matlab::data::Array vec = copyableF->get();
		object = std::move(vec);
	};
	std::future<void> future = std::async(std::launch::deferred, std::move(convertToResultType));
	return FutureResult<void>(std::move(future), std::make_shared<matlab::engine::TaskReference>(handle, mexApiCancelFevalWithCompletion));
}

matlab::engine::FutureResult<void> matlab::engine::MATLABEngine::setPropertyAsync(matlab::data::Array& object,
	size_t index,
	const std::string& propertyName,
	const matlab::data::Array& value) {
	return setPropertyAsync(object, index, std::u16string(propertyName.begin(), propertyName.end()), value);
}

matlab::engine::FutureResult<void> matlab::engine::MATLABEngine::setPropertyAsync(matlab::data::Array& object,
	const std::u16string& propertyName,
	const matlab::data::Array& value) {
	return setPropertyAsync(object, 0, propertyName, value);
}

matlab::engine::FutureResult<void> matlab::engine::MATLABEngine::setPropertyAsync(matlab::data::Array& object,
	const std::string& propertyName,
	const matlab::data::Array& value) {
	return setPropertyAsync(object, std::u16string(propertyName.begin(), propertyName.end()), value);
}
matlab::engine::FutureResult<std::vector<matlab::data::Array>> matlab::engine::MATLABEngine::fevalAsync(const std::u16string& function,
	const int nlhs,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {

	int nrhs = static_cast<int>(args.size());

	matlab::data::impl::ArrayImpl** args_arr_impl = (nrhs == 0) ? nullptr : (matlab::data::impl::ArrayImpl**)malloc(sizeof(matlab::data::impl::ArrayImpl*) * nrhs);
	std::unique_ptr<matlab::data::impl::ArrayImpl*, void(*)(matlab::data::impl::ArrayImpl**)> args_impl_guard(args_arr_impl, &implDeleter);

	void** args_impl = reinterpret_cast<void**>(args_arr_impl);

	arrayToImpl(nrhs, args_impl, args);

	std::promise<std::vector<matlab::data::Array> >* p = new std::promise<std::vector<matlab::data::Array> >();
	std::future<std::vector<matlab::data::Array> > f = p->get_future();

	std::string function_utf8 = matlab::engine::convertUTF16StringToUTF8String(function);

	void* output_ = output ? new std::shared_ptr<StreamBuffer>(std::move(output)) : nullptr;
	void* error_ = error ? new std::shared_ptr<StreamBuffer>(std::move(error)) : nullptr;

	uintptr_t handle = mexApiFevalAsync(pImpl, nlhs, nrhs, args_impl, function_utf8.c_str(), false, &set_feval_promise_data, &set_feval_promise_exception, p, output_, error_, &writeToStreamBuffer, &deleteStreamBufferImpl);

	return matlab::engine::FutureResult<std::vector<matlab::data::Array>>(std::move(f), std::make_shared<matlab::engine::TaskReference>(handle, mexApiCancelFevalWithCompletion));
}


matlab::engine::FutureResult<std::vector<matlab::data::Array>> matlab::engine::MATLABEngine::fevalAsync(const std::string& function,
	const int nlhs,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return fevalAsync(std::u16string(function.begin(), function.end()), nlhs, args, output, error);
}


matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::fevalAsync(const std::u16string& function,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	int nrhs = static_cast<int>(args.size());

	matlab::data::impl::ArrayImpl** args_arr_impl = (nrhs == 0) ? nullptr : (matlab::data::impl::ArrayImpl**)malloc(sizeof(matlab::data::impl::ArrayImpl*) * nrhs);
	std::unique_ptr<matlab::data::impl::ArrayImpl*, void(*)(matlab::data::impl::ArrayImpl**)> args_impl_guard(args_arr_impl, &implDeleter);

	void** args_impl = reinterpret_cast<void**>(args_arr_impl);

	arrayToImpl(nrhs, args_impl, args);

	std::promise<matlab::data::Array>* p = new std::promise<matlab::data::Array>();
	std::future<matlab::data::Array> f = p->get_future();

	std::string function_utf8 = matlab::engine::convertUTF16StringToUTF8String(function);

	void* output_ = output ? new std::shared_ptr<StreamBuffer>(std::move(output)) : nullptr;
	void* error_ = error ? new std::shared_ptr<StreamBuffer>(std::move(error)) : nullptr;

	uintptr_t handle = mexApiFevalAsync(pImpl, 1, nrhs, args_impl, function_utf8.c_str(), true, &set_feval_promise_data, &set_feval_promise_exception, p, output_, error_, &writeToStreamBuffer, &deleteStreamBufferImpl);

	return matlab::engine::FutureResult<matlab::data::Array>(std::move(f), std::make_shared<matlab::engine::TaskReference>(handle, mexApiCancelFevalWithCompletion));
}


matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::fevalAsync(const std::string& function,
	const std::vector<matlab::data::Array>& args,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return fevalAsync(std::u16string(function.begin(), function.end()), args, output, error);
}


matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::fevalAsync(const std::u16string& function,
	const matlab::data::Array& arg,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return fevalAsync(function, std::vector<matlab::data::Array>({ arg }), output, error);
}


matlab::engine::FutureResult<matlab::data::Array> matlab::engine::MATLABEngine::fevalAsync(const std::string& function,
	const matlab::data::Array& arg,
	const std::shared_ptr<matlab::engine::StreamBuffer>& output,
	const std::shared_ptr<matlab::engine::StreamBuffer>& error) {
	return fevalAsync(std::u16string(function.begin(), function.end()), arg, output, error);
}
matlab::data::Array getArray(void* v) {
	matlab::data::impl::ArrayImpl* impl = reinterpret_cast<matlab::data::impl::ArrayImpl*>(v);
	if (impl == nullptr)
		return matlab::data::Array();
	return matlab::data::detail::Access::createObj<matlab::data::Array>(impl);
}

matlab::data::Array matlab::engine::MATLABEngine::getVariable(const std::u16string& varName,
	matlab::engine::WorkspaceType workspaceType) {
	const char* workspace = (workspaceType == matlab::engine::WorkspaceType::BASE) ? "base" : "global";
	void* mexcept = nullptr;

	int errID = 0;
	void* res = mexApiGetVariable(pImpl, workspace, varName.c_str(), &errID, &mexcept);

	throwIfError(errID, mexcept);

	matlab::data::Array ret = getArray(res);
	return ret;
}
void throwIfError(int errID, void* mexcept) {
	matlab::mex::detail::ErrorType errorID(static_cast<matlab::mex::detail::ErrorType>(errID));
	switch (errorID) {
	case matlab::mex::detail::ErrorType::NoException:
		break;
	case matlab::mex::detail::ErrorType::RuntimeError: {
		matlab::data::impl::ArrayImpl* impl = reinterpret_cast<matlab::data::impl::ArrayImpl*>(mexcept);
		matlab::data::Array exArr(matlab::data::detail::Access::createObj<matlab::data::Array>(impl));
		matlab::data::StructArray sArr(exArr);
		matlab::data::TypedArray<int> errStatus = sArr[0][std::string("status")];
		int rErrID_ = errStatus[0];
		matlab::mex::detail::ErrorType rErrorID(static_cast<matlab::mex::detail::ErrorType>(rErrID_));
		switch (rErrorID) {
		case matlab::mex::detail::ErrorType::SyntaxError: {
			matlab::engine::MATLABSyntaxException exp = matlab::engine::detail::createMATLABSyntaxException(sArr);
			throw exp;
		}
		case matlab::mex::detail::ErrorType::ExecutionError: {
			matlab::engine::MATLABExecutionException exp = matlab::engine::detail::createMATLABExecutionException(sArr);
			throw exp;
		}
		case matlab::mex::detail::ErrorType::EngineError: {
			matlab::engine::MATLABException exp = matlab::engine::detail::createMATLABException(sArr);
			throw exp;
		}
		case matlab::mex::detail::ErrorType::InterruptedError: {
			std::string msg = "MATLAB command was interrupted.";
			throw matlab::engine::InterruptedException("MATLAB:mex:CppMexException", matlab::engine::convertUTF8StringToUTF16String(msg));
		}
		default:
			throw matlab::engine::MATLABException("MATLAB:mex:CppMexException", matlab::engine::convertUTF8StringToUTF16String("Runtime Error"));
		}
	}
	case matlab::mex::detail::ErrorType::ThreadError: {
		const std::string msg = "Synchronous version of MATLAB Engine functions must be called from the thread that called the MEX function.";
		throw matlab::engine::MATLABException("MATLAB:mex:CppMexThreadMismatch", matlab::engine::convertUTF8StringToUTF16String(msg));
	}
	case matlab::mex::detail::ErrorType::OutOfMemory: {
		std::string outOfMemoryError = "Not enough memory available to support the request.";
		throw matlab::OutOfMemoryException(outOfMemoryError);
	}
	case matlab::mex::detail::ErrorType::CancelError: {
		std::string msg = "MATLAB command was cancelled.";
		throw matlab::engine::CancelledException("MATLAB:mex:CppMexException", matlab::engine::convertUTF8StringToUTF16String(msg));
	}
	case matlab::mex::detail::ErrorType::SystemError: {
		std::string msg = "Unexpected exception caught in feval.";
		throw matlab::engine::MATLABException("MATLAB:mex:CppMexException", matlab::engine::convertUTF8StringToUTF16String(msg));
	}
	default:
		throw matlab::engine::MATLABException("MATLAB:mex:CppMexException", matlab::engine::convertUTF8StringToUTF16String("Error"));
	}
}