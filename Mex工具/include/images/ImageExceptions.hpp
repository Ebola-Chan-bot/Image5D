/**
* @file        ImageExceptions.hpp
*             
* Purpose      Exception definitions for Image class
*             
* Authors      Dinesh Iyer 
*             
* Copyright    2019-2021 MathWorks, Inc.
*
*/

#ifndef imagesdatatypesimage_exceptions_header
#define imagesdatatypesimage_exceptions_header

#include <stdexcept>
#include <string>

#include "datatypes_util.hpp"

namespace images
{
    namespace datatypes
    {
        namespace image
        {
            // ------------------------------------------------
            // Base-class for exceptions
            // ------------------------------------------------
            class IMAGESDATATYPESIMAGE_API Exception : public std::runtime_error
            {
                public:
                    /**
                     * Construct an exception
                     *
                     * @param - exceptionName - Name of the exception.
                     *          Typically a word with no white-space characters that
                     *          typically matches the class name
                     * @param - reason - String describing the reason for the exception
                     */
                    Exception( const std::string& exceptionName, const std::string& reason );
 
                    virtual ~Exception() throw();
 
                    /**
                     * Returns the name of the exception
                     *
                     * @param - none
                     * @return - std::string
                     */
                    std::string name() const;
 
                    /**
                     * Returns the reason for the exception
                     *
                     * @param - none
                     * @return - std::string
                     */
                    std::string reason() const;
 
                private:
                    std::string fExceptionName;
            };

            // ------------------------------------------------
            // Exception classes
            // ------------------------------------------------
            /**
              * Number of channels in the image data does not match
              * the expected value for a specified Color Format
              */
            class IMAGESDATATYPESIMAGE_API NumChannelsMismatchException : public Exception
            {
                public:
                    NumChannelsMismatchException();
                    virtual ~NumChannelsMismatchException() throw();
            };
 
            /**
              * Invalid enumeration value specified for the color format
              */
            class IMAGESDATATYPESIMAGE_API ColorFormatInvalidException : public Exception
            {
                public:
                    ColorFormatInvalidException();
                    virtual ~ColorFormatInvalidException() throw();
            };
 
            /**
              * Invalid enumeration value specified for the pixel layout
              */
            class IMAGESDATATYPESIMAGE_API LayoutInvalidException : public Exception
            {
                public:
                    LayoutInvalidException();
                    virtual ~LayoutInvalidException() throw();
            };
 
            /**
              * Invalid enumeration value specified for source management
              * scheme
              */
            class IMAGESDATATYPESIMAGE_API SourceMgmtInvalidException : public Exception
            {
                public:
                    SourceMgmtInvalidException();
                    virtual ~SourceMgmtInvalidException() throw();
            };
            
            /**
              * Invalid enumeration value specified for reqlinquishing underlying data
              */
            class IMAGESDATATYPESIMAGE_API ReqlinquishInvalidException : public Exception
            {
                public:
                    ReqlinquishInvalidException();
                    virtual ~ReqlinquishInvalidException() throw();
            };
 
            /**
              * Error condition not covered by the exception classes above
              */
            class IMAGESDATATYPESIMAGE_API UnexpectedException : public Exception
            {
                public:
                    UnexpectedException(const std::string& reason);
                    virtual ~UnexpectedException() throw();
            };       
        }
    }
}

#endif  // imagesdatatypesimage_exceptions_header
