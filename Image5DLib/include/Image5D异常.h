#pragma once
#include <stdint.h>
#include <pugixml.hpp>
#include<Windows.h>
namespace Image5D
{
	enum class Exception
	{
		Operation_success,
		File_opening_failed,
		Memory_mapping_failed,
		File_does_not_contain_blocks,
		REF_block_is_incomplete,
		The_XML_block_is_incomplete,
		UID_block_is_incomplete,
		The_pixel_block_is_incomplete,
		Header_file_is_incomplete,
		The_image_attributes_are_incomplete,
		Image_attribute_parsing_failed,
		Image_acquisition_undefined,
		Imaging_parameters_are_not_defined,
		Z_layer_size_is_not_defined,
		Layer_Z_name_undefined,
		The_channel_is_not_defined,
		Channels_enable_undefined,
		Channel_ID_not_defined,
		The_channel_order_is_undefined,
		Channel_device_name_not_defined,
		Scan_type_not_defined,
		Scan_Settings_not_defined,
		Scan_width_not_defined,
		Scan_height_is_not_defined,
		Series_interval_undefined,
		Cant_find_the_frame_header,
		Cant_find_the_image_header,
		Cant_find_the_lookup_table,
		Lookup_table_parsing_failed,
		The_red_component_is_undefined,
		The_green_component_is_undefined,
		The_blue_component_is_undefined,
		Empty_pixel_blocks,
		Method_unimplemented,
		File_header_is_incomplete,
		Unsupported_version_numbers,
		Head_IFD_incomplete,
		The_image_description_is_incomplete,
		Image_description_parsing_failed,
		Missing_OME_node,
		Missing_UUID_attribute,
		Missing_Image_node,
		Missing_the_Name_attribute,
		Without_Pixels_nodes,
		Missing_SizeX_attribute,
		Missing_SizeY_attribute,
		Missing_SizeC_attribute,
		Missing_SizeZ_attribute,
		Missing_SizeT_attribute,
		The_DimensionOrder_attribute_is_missing,
		The_dimensional_order_is_invalid,
		Missing_the_Type_attribute,
		The_color_number_does_not_match_SizeC,
		The_Color_attribute_is_missing,
		The_pixel_type_is_invalid,
		Specified_dimension_out_of_bounds,
		SizeX_is_0,
		SizeY_is_0,
		SizeC_is_0,
		SizeZ_is_0,
		SizeT_is_0,
		The_construction_parameter_is_invalid,
		The_color_pointer_is_NULL,
		The_image_description_is_NULL,
		Invalid_parameter_modification,
		Pixel_offset_out_of_file_range,
		Missing_required_labels,
		SizeCZT_does_not_match_SizeI,
		BitsPerSample_does_not_match_PixelType,
		The_image_property_is_not_defined,
		Image_phase_undefined,
		Phase_channel_not_defined,
		Image_configuration_undefined,
		The_z_axis_is_undefined,
		Scan_parameters_are_not_defined,
		Image_size_undefined,
		Scan_speed_is_not_defined,
		Velocity_information_is_undefined,
		Lookup_table_undefined,
		Unable_to_create_index_file,
		Unable_to_open_index_file,
		Index_file_corrupted,
		The_image_file_is_incomplete,
		Failed_to_set_the_file_pointer,
		The_device_name_is_too_long,
		Readonly_open_can_not_write,
		Readonly_open_cannot_be_modified,
		The_index_contains_no_blocks,
		The_channel_length_is_undefined,
		Memory_copy_failed,
		The_opacity_is_undefined,
		Failed_to_create_the_index_file,
		Continuous_Oir_mapping_fails,
		Tiff_file_creation_failed,
		Tiff_file_mapping_failed,
		The_specified_file_cannot_be_found,
		The_specified_path_cannot_be_found,
		File_access_denied,
		The_file_is_occupied_by_another_process,
		The_disk_is_full,
		The_file_size_is_0,
		Unsupported_byte_order,
		The_number_of_entered_parameters_is_incorrect,
		Output_pointer_memory_overflow,
		Input_pointer_access_out_of_bounds,
		Input_array_elements_too_few,
		File_creation_failed,
		Object_destruction_failed,
		Debug_breakpoints,
		Index_block_offset_write_failure,
		Memory_access_conflict,
		Cant_find_the_XML_block,
		Oir_file_header_is_incomplete,
		Oir_base_block_index_error,
		The_Oir_metadata_block_is_incomplete,
		Oir_image_attribute_out_of_bounds,
		LutUid_length_out_of_bounds,
		LutUid_out_of_bounds,
		LutXml_length_out_of_bounds,
		LutXml_out_of_bounds,
		LutXml_parsing_failed,
		Unexpected_label_value_type,
		Z_layer_step_size_is_not_defined,
		The_Z_level_starting_point_is_not_defined,
		Z_brightness_is_missing_relative_coordinates,
		Z_PMT_lacks_amplification_voltage,
		Z_Laser_setup_is_incomplete,
		The_actual_shooting_depth_is_outside_the_BrightZ_range,
		The_Z_PMT_Settings_are_incomplete,
		Missing_laser_parameters,
		PMT_voltage_not_defined,
		PMT_setup_is_incomplete,
		Invalid_pointer,
		The_library_version_is_lower_than_the_index_file_version,
		Detector_ID_not_defined,
		Z_driver_type_is_too_long,
		PMT_lacks_detectorId,
		Unknown_exception,
		Index_loading_failed,
		Entered_an_empty_array,
		Parsing_the_index_file_failed,
		CreationDateTime_not_defined,
	};
	struct Win32异常
	{
		Exception Image5D异常;
		DWORD Win32错误码;
	};
}