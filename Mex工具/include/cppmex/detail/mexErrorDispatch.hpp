#ifndef __MEX_API_ERROR_DISPATCH_HPP__
#define __MEX_API_ERROR_DISPATCH_HPP__

#include "mexExceptionType.hpp"


namespace matlab {
    namespace engine {
        namespace detail {

            inline matlab::engine::MATLABExecutionException createMATLABExecutionException(const matlab::data::StructArray& mException);

            inline std::vector<matlab::engine::MATLABExecutionException> createCause(const matlab::data::CellArray& cause) {
                size_t nCauses = cause.getNumberOfElements();
                std::vector<matlab::engine::MATLABExecutionException> causes(nCauses);
                for (size_t i = 0; i < nCauses; i++) {
                    matlab::data::Array exRef = cause[i];
                    matlab::data::StructArray ex(exRef);
                    causes[i] = createMATLABExecutionException(ex);
                }
                return causes;
            }

            inline std::vector<matlab::engine::StackFrame> createStackTrace(const matlab::data::StructArray& stack) {
                size_t nFrames = stack.getNumberOfElements();
                std::vector<matlab::engine::StackFrame> stackFrames(nFrames);

                for (size_t i = 0; i < nFrames; i++) {
                    matlab::data::Array fileRef = stack[i]["File"];
                    matlab::data::CharArray fileStr(fileRef);

                    matlab::data::Array nameRef = stack[i]["Name"];
                    matlab::data::CharArray nameStr(nameRef);

                    matlab::data::Array lineRef = stack[i]["Line"];
                    double line = lineRef[0];
                    stackFrames[i] = matlab::engine::StackFrame(fileStr.toUTF16(), nameStr.toUTF16(), uint32_t(line));
                }

                return stackFrames;
            }

            inline matlab::engine::MATLABExecutionException createMATLABExecutionException(const matlab::data::StructArray& mException) {
                matlab::data::Array idRef = mException[0][std::string("identifier")];
                matlab::data::CharArray id(idRef);
                matlab::data::Array messageRef = mException[0][std::string("message")];
                matlab::data::CharArray message(messageRef);

                matlab::data::Array stackRef = mException[0][std::string("stack")];
                matlab::data::StructArray stack(stackRef);

                matlab::data::Array causeRef = mException[0][std::string("cause")];
                matlab::data::CellArray cause(causeRef);

                std::vector<matlab::engine::MATLABExecutionException> meCause = createCause(cause);
                std::vector<matlab::engine::StackFrame> meStack = createStackTrace(stack);

                return matlab::engine::MATLABExecutionException(id.toAscii(), message.toUTF16(), meStack, meCause);
            }

            inline matlab::engine::MATLABSyntaxException createMATLABSyntaxException(const matlab::data::StructArray& mException) {
                matlab::data::Array idRef = mException[0][std::string("identifier")];
                matlab::data::CharArray id(idRef);
                matlab::data::Array messageRef = mException[0][std::string("message")];
                matlab::data::CharArray message(messageRef);

                return matlab::engine::MATLABSyntaxException(id.toAscii(), message.toUTF16());
            }

            inline matlab::engine::MATLABException createMATLABException(const matlab::data::StructArray& mException) {
                matlab::data::Array idRef = mException[0][std::string("identifier")];
                matlab::data::CharArray id(idRef);
                matlab::data::Array messageRef = mException[0][std::string("message")];
                matlab::data::CharArray message(messageRef);

                return matlab::engine::MATLABException(id.toAscii(), message.toUTF16());
            }
        }
    }
}


void throwIfError(int errID, void* mexcept);

#endif