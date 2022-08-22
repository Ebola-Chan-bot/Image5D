classdef XmlErrorCode<uint8
	enumeration
		status_ok(0)				% No error

		status_file_not_found(1)		% File was not found during load_file()
		status_io_error(2)			% Error reading from file/stream
		status_out_of_memory(3)		% Could not allocate memory
		status_internal_error(4)		% Internal error occurred

		status_unrecognized_tag(5)	% Parser could not determine tag type

		status_bad_pi(6)				% Parsing error occurred while parsing document declaration/processing instruction
		status_bad_comment(7)			% Parsing error occurred while parsing comment
		status_bad_cdata(8)			% Parsing error occurred while parsing CDATA section
		status_bad_doctype(9)			% Parsing error occurred while parsing document type declaration
		status_bad_pcdata(10)			% Parsing error occurred while parsing PCDATA section
		status_bad_start_element(11)	% Parsing error occurred while parsing start element tag
		status_bad_attribute(12)		% Parsing error occurred while parsing element attribute
		status_bad_end_element(13)		% Parsing error occurred while parsing end element tag
		status_end_element_mismatch(14)% There was a mismatch of start-end tags (closing tag had incorrect name() some tag was not closed or there was an excessive closing tag)

		status_append_invalid_root(15)	% Unable to append nodes since root type is not node_element or node_document (exclusive to xml_node::append_buffer)

		status_no_document_element(16)	% Parsing resulted in a document without element nodes
	end
end