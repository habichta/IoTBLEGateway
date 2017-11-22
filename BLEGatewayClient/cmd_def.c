



#include <stdio.h>

#include "cmd_def.h"


void (*bglib_output)(uint8 len1,uint8* data1,uint16 len2,uint8* data2)=0;    
static const struct ble_msg  apis[]={
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_system,ble_cmd_system_reset_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_system,ble_cmd_system_hello_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_system,ble_cmd_system_address_get_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_system,ble_cmd_system_reg_write_id}, 0x24,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_system,ble_cmd_system_reg_read_id}, 0x4,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_system,ble_cmd_system_get_counters_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_system,ble_cmd_system_get_connections_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x5,ble_cls_system,ble_cmd_system_read_memory_id}, 0x26,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_system,ble_cmd_system_get_info_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_system,ble_cmd_system_endpoint_tx_id}, 0x82,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x7,ble_cls_system,ble_cmd_system_whitelist_append_id}, 0x2a,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x7,ble_cls_system,ble_cmd_system_whitelist_remove_id}, 0x2a,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_system,ble_cmd_system_whitelist_clear_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_system,ble_cmd_system_endpoint_rx_id}, 0x22,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_system,ble_cmd_system_endpoint_set_watermarks_id}, 0x222,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_system,ble_cmd_system_aes_setkey_id}, 0x8,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_system,ble_cmd_system_aes_encrypt_id}, 0x8,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_system,ble_cmd_system_aes_decrypt_id}, 0x8,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_flash,ble_cmd_flash_ps_defrag_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_flash,ble_cmd_flash_ps_dump_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_flash,ble_cmd_flash_ps_erase_all_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_flash,ble_cmd_flash_ps_save_id}, 0x84,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_flash,ble_cmd_flash_ps_load_id}, 0x4,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_flash,ble_cmd_flash_ps_erase_id}, 0x4,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_flash,ble_cmd_flash_erase_page_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x5,ble_cls_flash,ble_cmd_flash_write_data_id}, 0x86,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x5,ble_cls_flash,ble_cmd_flash_read_data_id}, 0x26,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x4,ble_cls_attributes,ble_cmd_attributes_write_id}, 0x824,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x4,ble_cls_attributes,ble_cmd_attributes_read_id}, 0x44,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_attributes,ble_cmd_attributes_read_type_id}, 0x4,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_attributes,ble_cmd_attributes_user_read_response_id}, 0x822,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_attributes,ble_cmd_attributes_user_write_response_id}, 0x22,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x4,ble_cls_attributes,ble_cmd_attributes_send_id}, 0x842,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_connection,ble_cmd_connection_disconnect_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_connection,ble_cmd_connection_get_rssi_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x9,ble_cls_connection,ble_cmd_connection_update_id}, 0x44442,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_connection,ble_cmd_connection_version_update_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_connection,ble_cmd_connection_channel_map_get_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_connection,ble_cmd_connection_channel_map_set_id}, 0x82,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_connection,ble_cmd_connection_features_get_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_connection,ble_cmd_connection_get_status_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_connection,ble_cmd_connection_raw_tx_id}, 0x82,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x8,ble_cls_attclient,ble_cmd_attclient_find_by_type_value_id}, 0x84442,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x6,ble_cls_attclient,ble_cmd_attclient_read_by_group_type_id}, 0x8442,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x6,ble_cls_attclient,ble_cmd_attclient_read_by_type_id}, 0x8442,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x5,ble_cls_attclient,ble_cmd_attclient_find_information_id}, 0x442,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_attclient,ble_cmd_attclient_read_by_handle_id}, 0x42,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x4,ble_cls_attclient,ble_cmd_attclient_attribute_write_id}, 0x842,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x4,ble_cls_attclient,ble_cmd_attclient_write_command_id}, 0x842,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_attclient,ble_cmd_attclient_indicate_confirm_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_attclient,ble_cmd_attclient_read_long_id}, 0x42,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x6,ble_cls_attclient,ble_cmd_attclient_prepare_write_id}, 0x8442,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_attclient,ble_cmd_attclient_execute_write_id}, 0x22,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_attclient,ble_cmd_attclient_read_multiple_id}, 0x82,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_sm,ble_cmd_sm_encrypt_start_id}, 0x22,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_sm,ble_cmd_sm_set_bondable_mode_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_sm,ble_cmd_sm_delete_bonding_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_sm,ble_cmd_sm_set_parameters_id}, 0x222,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x5,ble_cls_sm,ble_cmd_sm_passkey_entry_id}, 0x62,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_sm,ble_cmd_sm_get_bonds_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_sm,ble_cmd_sm_set_oob_data_id}, 0x8,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_sm,ble_cmd_sm_whitelist_bonds_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_gap,ble_cmd_gap_set_privacy_flags_id}, 0x22,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_gap,ble_cmd_gap_set_mode_id}, 0x22,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_gap,ble_cmd_gap_discover_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0xf,ble_cls_gap,ble_cmd_gap_connect_direct_id}, 0x44442a,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_gap,ble_cmd_gap_end_procedure_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x8,ble_cls_gap,ble_cmd_gap_connect_selective_id}, 0x4444,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_gap,ble_cmd_gap_set_filtering_id}, 0x222,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x5,ble_cls_gap,ble_cmd_gap_set_scan_parameters_id}, 0x244,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x5,ble_cls_gap,ble_cmd_gap_set_adv_parameters_id}, 0x244,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_gap,ble_cmd_gap_set_adv_data_id}, 0x82,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x7,ble_cls_gap,ble_cmd_gap_set_directed_connectable_mode_id}, 0x2a,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_hardware,ble_cmd_hardware_io_port_config_irq_id}, 0x222,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x6,ble_cls_hardware,ble_cmd_hardware_set_soft_timer_id}, 0x226,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_hardware,ble_cmd_hardware_adc_read_id}, 0x222,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_io_port_config_direction_id}, 0x22,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_io_port_config_function_id}, 0x22,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_hardware,ble_cmd_hardware_io_port_config_pull_id}, 0x222,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_hardware,ble_cmd_hardware_io_port_write_id}, 0x222,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_io_port_read_id}, 0x22,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x6,ble_cls_hardware,ble_cmd_hardware_spi_config_id}, 0x222222,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_spi_transfer_id}, 0x82,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_hardware,ble_cmd_hardware_i2c_read_id}, 0x222,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_hardware,ble_cmd_hardware_i2c_write_id}, 0x822,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_hardware,ble_cmd_hardware_set_txpower_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x5,ble_cls_hardware,ble_cmd_hardware_timer_comparator_id}, 0x4222,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_io_port_irq_enable_id}, 0x22,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_io_port_irq_direction_id}, 0x22,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_hardware,ble_cmd_hardware_analog_comparator_enable_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_hardware,ble_cmd_hardware_analog_comparator_read_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_hardware,ble_cmd_hardware_analog_comparator_config_irq_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_hardware,ble_cmd_hardware_set_rxgain_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_hardware,ble_cmd_hardware_usb_enable_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x3,ble_cls_test,ble_cmd_test_phy_tx_id}, 0x222,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_test,ble_cmd_test_phy_rx_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_test,ble_cmd_test_phy_end_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_test,ble_cmd_test_phy_reset_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_test,ble_cmd_test_get_channel_map_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_test,ble_cmd_test_debug_id}, 0x8,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_test,ble_cmd_test_channel_mode_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_dfu,ble_cmd_dfu_reset_id}, 0x2,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x4,ble_cls_dfu,ble_cmd_dfu_flash_set_address_id}, 0x6,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x1,ble_cls_dfu,ble_cmd_dfu_flash_upload_id}, 0x8,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_cmd|0x0,0x0,ble_cls_dfu,ble_cmd_dfu_flash_upload_finish_id}, 0x0,(ble_cmd_handler)ble_default},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_system,ble_cmd_system_reset_id}, 0x0,	(ble_cmd_handler)ble_rsp_system_reset},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_system,ble_cmd_system_hello_id}, 0x0,	(ble_cmd_handler)ble_rsp_system_hello},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x6,ble_cls_system,ble_cmd_system_address_get_id}, 0xa,	(ble_cmd_handler)ble_rsp_system_address_get},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_system,ble_cmd_system_reg_write_id}, 0x4,	(ble_cmd_handler)ble_rsp_system_reg_write},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_system,ble_cmd_system_reg_read_id}, 0x24,	(ble_cmd_handler)ble_rsp_system_reg_read},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x5,ble_cls_system,ble_cmd_system_get_counters_id}, 0x22222,	(ble_cmd_handler)ble_rsp_system_get_counters},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x1,ble_cls_system,ble_cmd_system_get_connections_id}, 0x2,	(ble_cmd_handler)ble_rsp_system_get_connections},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x5,ble_cls_system,ble_cmd_system_read_memory_id}, 0x86,	(ble_cmd_handler)ble_rsp_system_read_memory},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0xc,ble_cls_system,ble_cmd_system_get_info_id}, 0x2244444,	(ble_cmd_handler)ble_rsp_system_get_info},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_system,ble_cmd_system_endpoint_tx_id}, 0x4,	(ble_cmd_handler)ble_rsp_system_endpoint_tx},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_system,ble_cmd_system_whitelist_append_id}, 0x4,	(ble_cmd_handler)ble_rsp_system_whitelist_append},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_system,ble_cmd_system_whitelist_remove_id}, 0x4,	(ble_cmd_handler)ble_rsp_system_whitelist_remove},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_system,ble_cmd_system_whitelist_clear_id}, 0x0,	(ble_cmd_handler)ble_rsp_system_whitelist_clear},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_system,ble_cmd_system_endpoint_rx_id}, 0x84,	(ble_cmd_handler)ble_rsp_system_endpoint_rx},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_system,ble_cmd_system_endpoint_set_watermarks_id}, 0x4,	(ble_cmd_handler)ble_rsp_system_endpoint_set_watermarks},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_system,ble_cmd_system_aes_setkey_id}, 0x0,	(ble_cmd_handler)ble_rsp_system_aes_setkey},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x1,ble_cls_system,ble_cmd_system_aes_encrypt_id}, 0x8,	(ble_cmd_handler)ble_rsp_system_aes_encrypt},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x1,ble_cls_system,ble_cmd_system_aes_decrypt_id}, 0x8,	(ble_cmd_handler)ble_rsp_system_aes_decrypt},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_flash,ble_cmd_flash_ps_defrag_id}, 0x0,	(ble_cmd_handler)ble_rsp_flash_ps_defrag},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_flash,ble_cmd_flash_ps_dump_id}, 0x0,	(ble_cmd_handler)ble_rsp_flash_ps_dump},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_flash,ble_cmd_flash_ps_erase_all_id}, 0x0,	(ble_cmd_handler)ble_rsp_flash_ps_erase_all},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_flash,ble_cmd_flash_ps_save_id}, 0x4,	(ble_cmd_handler)ble_rsp_flash_ps_save},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_flash,ble_cmd_flash_ps_load_id}, 0x84,	(ble_cmd_handler)ble_rsp_flash_ps_load},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_flash,ble_cmd_flash_ps_erase_id}, 0x0,	(ble_cmd_handler)ble_rsp_flash_ps_erase},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_flash,ble_cmd_flash_erase_page_id}, 0x4,	(ble_cmd_handler)ble_rsp_flash_erase_page},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_flash,ble_cmd_flash_write_data_id}, 0x4,	(ble_cmd_handler)ble_rsp_flash_write_data},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x1,ble_cls_flash,ble_cmd_flash_read_data_id}, 0x8,	(ble_cmd_handler)ble_rsp_flash_read_data},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_attributes,ble_cmd_attributes_write_id}, 0x4,	(ble_cmd_handler)ble_rsp_attributes_write},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x7,ble_cls_attributes,ble_cmd_attributes_read_id}, 0x8444,	(ble_cmd_handler)ble_rsp_attributes_read},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x5,ble_cls_attributes,ble_cmd_attributes_read_type_id}, 0x844,	(ble_cmd_handler)ble_rsp_attributes_read_type},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_attributes,ble_cmd_attributes_user_read_response_id}, 0x0,	(ble_cmd_handler)ble_rsp_attributes_user_read_response},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_attributes,ble_cmd_attributes_user_write_response_id}, 0x0,	(ble_cmd_handler)ble_rsp_attributes_user_write_response},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_attributes,ble_cmd_attributes_send_id}, 0x4,	(ble_cmd_handler)ble_rsp_attributes_send},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_connection,ble_cmd_connection_disconnect_id}, 0x42,	(ble_cmd_handler)ble_rsp_connection_disconnect},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_connection,ble_cmd_connection_get_rssi_id}, 0x32,	(ble_cmd_handler)ble_rsp_connection_get_rssi},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_connection,ble_cmd_connection_update_id}, 0x42,	(ble_cmd_handler)ble_rsp_connection_update},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_connection,ble_cmd_connection_version_update_id}, 0x42,	(ble_cmd_handler)ble_rsp_connection_version_update},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_connection,ble_cmd_connection_channel_map_get_id}, 0x82,	(ble_cmd_handler)ble_rsp_connection_channel_map_get},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_connection,ble_cmd_connection_channel_map_set_id}, 0x42,	(ble_cmd_handler)ble_rsp_connection_channel_map_set},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_connection,ble_cmd_connection_features_get_id}, 0x42,	(ble_cmd_handler)ble_rsp_connection_features_get},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x1,ble_cls_connection,ble_cmd_connection_get_status_id}, 0x2,	(ble_cmd_handler)ble_rsp_connection_get_status},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x1,ble_cls_connection,ble_cmd_connection_raw_tx_id}, 0x2,	(ble_cmd_handler)ble_rsp_connection_raw_tx},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_attclient,ble_cmd_attclient_find_by_type_value_id}, 0x42,	(ble_cmd_handler)ble_rsp_attclient_find_by_type_value},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_attclient,ble_cmd_attclient_read_by_group_type_id}, 0x42,	(ble_cmd_handler)ble_rsp_attclient_read_by_group_type},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_attclient,ble_cmd_attclient_read_by_type_id}, 0x42,	(ble_cmd_handler)ble_rsp_attclient_read_by_type},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_attclient,ble_cmd_attclient_find_information_id}, 0x42,	(ble_cmd_handler)ble_rsp_attclient_find_information},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_attclient,ble_cmd_attclient_read_by_handle_id}, 0x42,	(ble_cmd_handler)ble_rsp_attclient_read_by_handle},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_attclient,ble_cmd_attclient_attribute_write_id}, 0x42,	(ble_cmd_handler)ble_rsp_attclient_attribute_write},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_attclient,ble_cmd_attclient_write_command_id}, 0x42,	(ble_cmd_handler)ble_rsp_attclient_write_command},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_attclient,ble_cmd_attclient_indicate_confirm_id}, 0x4,	(ble_cmd_handler)ble_rsp_attclient_indicate_confirm},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_attclient,ble_cmd_attclient_read_long_id}, 0x42,	(ble_cmd_handler)ble_rsp_attclient_read_long},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_attclient,ble_cmd_attclient_prepare_write_id}, 0x42,	(ble_cmd_handler)ble_rsp_attclient_prepare_write},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_attclient,ble_cmd_attclient_execute_write_id}, 0x42,	(ble_cmd_handler)ble_rsp_attclient_execute_write},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_attclient,ble_cmd_attclient_read_multiple_id}, 0x42,	(ble_cmd_handler)ble_rsp_attclient_read_multiple},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_sm,ble_cmd_sm_encrypt_start_id}, 0x42,	(ble_cmd_handler)ble_rsp_sm_encrypt_start},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_sm,ble_cmd_sm_set_bondable_mode_id}, 0x0,	(ble_cmd_handler)ble_rsp_sm_set_bondable_mode},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_sm,ble_cmd_sm_delete_bonding_id}, 0x4,	(ble_cmd_handler)ble_rsp_sm_delete_bonding},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_sm,ble_cmd_sm_set_parameters_id}, 0x0,	(ble_cmd_handler)ble_rsp_sm_set_parameters},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_sm,ble_cmd_sm_passkey_entry_id}, 0x4,	(ble_cmd_handler)ble_rsp_sm_passkey_entry},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x1,ble_cls_sm,ble_cmd_sm_get_bonds_id}, 0x2,	(ble_cmd_handler)ble_rsp_sm_get_bonds},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_sm,ble_cmd_sm_set_oob_data_id}, 0x0,	(ble_cmd_handler)ble_rsp_sm_set_oob_data},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_sm,ble_cmd_sm_whitelist_bonds_id}, 0x24,	(ble_cmd_handler)ble_rsp_sm_whitelist_bonds},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_gap,ble_cmd_gap_set_privacy_flags_id}, 0x0,	(ble_cmd_handler)ble_rsp_gap_set_privacy_flags},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_gap,ble_cmd_gap_set_mode_id}, 0x4,	(ble_cmd_handler)ble_rsp_gap_set_mode},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_gap,ble_cmd_gap_discover_id}, 0x4,	(ble_cmd_handler)ble_rsp_gap_discover},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_gap,ble_cmd_gap_connect_direct_id}, 0x24,	(ble_cmd_handler)ble_rsp_gap_connect_direct},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_gap,ble_cmd_gap_end_procedure_id}, 0x4,	(ble_cmd_handler)ble_rsp_gap_end_procedure},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_gap,ble_cmd_gap_connect_selective_id}, 0x24,	(ble_cmd_handler)ble_rsp_gap_connect_selective},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_gap,ble_cmd_gap_set_filtering_id}, 0x4,	(ble_cmd_handler)ble_rsp_gap_set_filtering},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_gap,ble_cmd_gap_set_scan_parameters_id}, 0x4,	(ble_cmd_handler)ble_rsp_gap_set_scan_parameters},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_gap,ble_cmd_gap_set_adv_parameters_id}, 0x4,	(ble_cmd_handler)ble_rsp_gap_set_adv_parameters},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_gap,ble_cmd_gap_set_adv_data_id}, 0x4,	(ble_cmd_handler)ble_rsp_gap_set_adv_data},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_gap,ble_cmd_gap_set_directed_connectable_mode_id}, 0x4,	(ble_cmd_handler)ble_rsp_gap_set_directed_connectable_mode},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_io_port_config_irq_id}, 0x4,	(ble_cmd_handler)ble_rsp_hardware_io_port_config_irq},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_set_soft_timer_id}, 0x4,	(ble_cmd_handler)ble_rsp_hardware_set_soft_timer},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_adc_read_id}, 0x4,	(ble_cmd_handler)ble_rsp_hardware_adc_read},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_io_port_config_direction_id}, 0x4,	(ble_cmd_handler)ble_rsp_hardware_io_port_config_direction},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_io_port_config_function_id}, 0x4,	(ble_cmd_handler)ble_rsp_hardware_io_port_config_function},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_io_port_config_pull_id}, 0x4,	(ble_cmd_handler)ble_rsp_hardware_io_port_config_pull},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_io_port_write_id}, 0x4,	(ble_cmd_handler)ble_rsp_hardware_io_port_write},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x4,ble_cls_hardware,ble_cmd_hardware_io_port_read_id}, 0x224,	(ble_cmd_handler)ble_rsp_hardware_io_port_read},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_spi_config_id}, 0x4,	(ble_cmd_handler)ble_rsp_hardware_spi_config},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x4,ble_cls_hardware,ble_cmd_hardware_spi_transfer_id}, 0x824,	(ble_cmd_handler)ble_rsp_hardware_spi_transfer},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_hardware,ble_cmd_hardware_i2c_read_id}, 0x84,	(ble_cmd_handler)ble_rsp_hardware_i2c_read},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x1,ble_cls_hardware,ble_cmd_hardware_i2c_write_id}, 0x2,	(ble_cmd_handler)ble_rsp_hardware_i2c_write},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_hardware,ble_cmd_hardware_set_txpower_id}, 0x0,	(ble_cmd_handler)ble_rsp_hardware_set_txpower},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_timer_comparator_id}, 0x4,	(ble_cmd_handler)ble_rsp_hardware_timer_comparator},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_io_port_irq_enable_id}, 0x4,	(ble_cmd_handler)ble_rsp_hardware_io_port_irq_enable},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_io_port_irq_direction_id}, 0x4,	(ble_cmd_handler)ble_rsp_hardware_io_port_irq_direction},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_hardware,ble_cmd_hardware_analog_comparator_enable_id}, 0x0,	(ble_cmd_handler)ble_rsp_hardware_analog_comparator_enable},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x3,ble_cls_hardware,ble_cmd_hardware_analog_comparator_read_id}, 0x24,	(ble_cmd_handler)ble_rsp_hardware_analog_comparator_read},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_analog_comparator_config_irq_id}, 0x4,	(ble_cmd_handler)ble_rsp_hardware_analog_comparator_config_irq},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_hardware,ble_cmd_hardware_set_rxgain_id}, 0x0,	(ble_cmd_handler)ble_rsp_hardware_set_rxgain},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_hardware,ble_cmd_hardware_usb_enable_id}, 0x4,	(ble_cmd_handler)ble_rsp_hardware_usb_enable},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_test,ble_cmd_test_phy_tx_id}, 0x0,	(ble_cmd_handler)ble_rsp_test_phy_tx},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_test,ble_cmd_test_phy_rx_id}, 0x0,	(ble_cmd_handler)ble_rsp_test_phy_rx},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_test,ble_cmd_test_phy_end_id}, 0x4,	(ble_cmd_handler)ble_rsp_test_phy_end},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_test,ble_cmd_test_phy_reset_id}, 0x0,	(ble_cmd_handler)ble_rsp_test_phy_reset},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x1,ble_cls_test,ble_cmd_test_get_channel_map_id}, 0x8,	(ble_cmd_handler)ble_rsp_test_get_channel_map},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x1,ble_cls_test,ble_cmd_test_debug_id}, 0x8,	(ble_cmd_handler)ble_rsp_test_debug},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_test,ble_cmd_test_channel_mode_id}, 0x0,	(ble_cmd_handler)ble_rsp_test_channel_mode},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x0,ble_cls_dfu,ble_cmd_dfu_reset_id}, 0x0,	(ble_cmd_handler)ble_rsp_dfu_reset},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_dfu,ble_cmd_dfu_flash_set_address_id}, 0x4,	(ble_cmd_handler)ble_rsp_dfu_flash_set_address},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_dfu,ble_cmd_dfu_flash_upload_id}, 0x4,	(ble_cmd_handler)ble_rsp_dfu_flash_upload},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_rsp|0x0,0x2,ble_cls_dfu,ble_cmd_dfu_flash_upload_finish_id}, 0x4,	(ble_cmd_handler)ble_rsp_dfu_flash_upload_finish},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0xc,ble_cls_system,ble_evt_system_boot_id}, 0x2244444,	(ble_cmd_handler)ble_evt_system_boot},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x1,ble_cls_system,ble_evt_system_debug_id}, 0x8,	(ble_cmd_handler)ble_evt_system_debug},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x2,ble_cls_system,ble_evt_system_endpoint_watermark_rx_id}, 0x22,	(ble_cmd_handler)ble_evt_system_endpoint_watermark_rx},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x2,ble_cls_system,ble_evt_system_endpoint_watermark_tx_id}, 0x22,	(ble_cmd_handler)ble_evt_system_endpoint_watermark_tx},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x4,ble_cls_system,ble_evt_system_script_failure_id}, 0x44,	(ble_cmd_handler)ble_evt_system_script_failure},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x0,ble_cls_system,ble_evt_system_no_license_key_id}, 0x0,	(ble_cmd_handler)ble_evt_system_no_license_key},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x2,ble_cls_system,ble_evt_system_protocol_error_id}, 0x4,	(ble_cmd_handler)ble_evt_system_protocol_error},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x3,ble_cls_flash,ble_evt_flash_ps_key_id}, 0x84,	(ble_cmd_handler)ble_evt_flash_ps_key},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x7,ble_cls_attributes,ble_evt_attributes_value_id}, 0x84422,	(ble_cmd_handler)ble_evt_attributes_value},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x6,ble_cls_attributes,ble_evt_attributes_user_read_request_id}, 0x2442,	(ble_cmd_handler)ble_evt_attributes_user_read_request},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x3,ble_cls_attributes,ble_evt_attributes_status_id}, 0x24,	(ble_cmd_handler)ble_evt_attributes_status},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x10,ble_cls_connection,ble_evt_connection_status_id}, 0x24442a22,	(ble_cmd_handler)ble_evt_connection_status},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x6,ble_cls_connection,ble_evt_connection_version_ind_id}, 0x4422,	(ble_cmd_handler)ble_evt_connection_version_ind},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x2,ble_cls_connection,ble_evt_connection_feature_ind_id}, 0x82,	(ble_cmd_handler)ble_evt_connection_feature_ind},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x2,ble_cls_connection,ble_evt_connection_raw_rx_id}, 0x82,	(ble_cmd_handler)ble_evt_connection_raw_rx},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x3,ble_cls_connection,ble_evt_connection_disconnected_id}, 0x42,	(ble_cmd_handler)ble_evt_connection_disconnected},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x3,ble_cls_attclient,ble_evt_attclient_indicated_id}, 0x42,	(ble_cmd_handler)ble_evt_attclient_indicated},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x5,ble_cls_attclient,ble_evt_attclient_procedure_completed_id}, 0x442,	(ble_cmd_handler)ble_evt_attclient_procedure_completed},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x6,ble_cls_attclient,ble_evt_attclient_group_found_id}, 0x8442,	(ble_cmd_handler)ble_evt_attclient_group_found},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x7,ble_cls_attclient,ble_evt_attclient_attribute_found_id}, 0x82442,	(ble_cmd_handler)ble_evt_attclient_attribute_found},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x4,ble_cls_attclient,ble_evt_attclient_find_information_found_id}, 0x842,	(ble_cmd_handler)ble_evt_attclient_find_information_found},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x5,ble_cls_attclient,ble_evt_attclient_attribute_value_id}, 0x8242,	(ble_cmd_handler)ble_evt_attclient_attribute_value},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x2,ble_cls_attclient,ble_evt_attclient_read_multiple_response_id}, 0x82,	(ble_cmd_handler)ble_evt_attclient_read_multiple_response},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x3,ble_cls_sm,ble_evt_sm_smp_data_id}, 0x822,	(ble_cmd_handler)ble_evt_sm_smp_data},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x3,ble_cls_sm,ble_evt_sm_bonding_fail_id}, 0x42,	(ble_cmd_handler)ble_evt_sm_bonding_fail},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x5,ble_cls_sm,ble_evt_sm_passkey_display_id}, 0x62,	(ble_cmd_handler)ble_evt_sm_passkey_display},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x1,ble_cls_sm,ble_evt_sm_passkey_request_id}, 0x2,	(ble_cmd_handler)ble_evt_sm_passkey_request},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x4,ble_cls_sm,ble_evt_sm_bond_status_id}, 0x2222,	(ble_cmd_handler)ble_evt_sm_bond_status},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0xb,ble_cls_gap,ble_evt_gap_scan_response_id}, 0x822a23,	(ble_cmd_handler)ble_evt_gap_scan_response},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x2,ble_cls_gap,ble_evt_gap_mode_changed_id}, 0x22,	(ble_cmd_handler)ble_evt_gap_mode_changed},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x7,ble_cls_hardware,ble_evt_hardware_io_port_status_id}, 0x2226,	(ble_cmd_handler)ble_evt_hardware_io_port_status},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x1,ble_cls_hardware,ble_evt_hardware_soft_timer_id}, 0x2,	(ble_cmd_handler)ble_evt_hardware_soft_timer},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x3,ble_cls_hardware,ble_evt_hardware_adc_result_id}, 0x52,	(ble_cmd_handler)ble_evt_hardware_adc_result},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x5,ble_cls_hardware,ble_evt_hardware_analog_comparator_status_id}, 0x26,	(ble_cmd_handler)ble_evt_hardware_analog_comparator_status},
	{{(uint8)ble_dev_type_ble|(uint8)ble_msg_type_evt|0x0,0x4,ble_cls_dfu,ble_evt_dfu_boot_id}, 0x6,	(ble_cmd_handler)ble_evt_dfu_boot},
	{{0,0,0,0}, 0, 0}}; 
const struct ble_msg * ble_get_msg(uint8 idx)
{
    return &apis[idx];
}
const struct ble_msg * ble_find_msg_hdr(struct ble_header hdr)
{
    const struct ble_msg *msg=apis;
    
    while(msg->handler)
    {
        if(((msg->hdr.type_hilen&0xF8)==(hdr.type_hilen&0xF8))&&
            (msg->hdr.cls==hdr.cls)&&
            (msg->hdr.command==hdr.command))return msg;
        msg++;
    }
    return 0;
}

const struct ble_msg * ble_get_msg_hdr(struct ble_header hdr)
{
    if((hdr.type_hilen&0x80)==ble_msg_type_evt)
    {
        if(hdr.cls>=(sizeof(ble_class_evt_handlers)/sizeof(struct ble_class_handler_t)))
            return NULL;
        if(hdr.command>=ble_class_evt_handlers[hdr.cls].maxhandlers)
            return NULL;
        return ble_class_evt_handlers[hdr.cls].msgs[hdr.command];        
    }else
    if((hdr.type_hilen&0x80)==ble_msg_type_rsp)
    {
        if(hdr.cls>=(sizeof(ble_class_rsp_handlers)/sizeof(struct ble_class_handler_t)))
            return NULL;
        if(hdr.command>=ble_class_rsp_handlers[hdr.cls].maxhandlers)
            return NULL;
        return ble_class_rsp_handlers[hdr.cls].msgs[hdr.command];        
    }
    return ble_find_msg_hdr(hdr);
}
void ble_send_message(uint8 msgid,...)            
    {
        uint32 i;
        uint32 u32;
        uint16 u16;
        uint8  u8;
                    
        struct ble_cmd_packet packet;
        uint8 *b=(uint8 *)&packet.payload;
            
        uint8 *hw;
        uint8 *data_ptr=0;
        uint16 data_len=0;
        va_list va;
        va_start(va,msgid);
        
        i=apis[msgid].params;
        packet.header=apis[msgid].hdr;
        while(i)
        {
        
            switch(i&0xF)
            {
            
                case 7:/*int32*/
                case 6:/*uint32*/
                    u32=va_arg(va,uint32);
                    *b++=u32&0xff;u32>>=8;
                    *b++=u32&0xff;u32>>=8;
                    *b++=u32&0xff;u32>>=8;
                    *b++=u32&0xff;
                    break;
                case 5:/*int16*/
                case 4:/*uint16*/
                    u16=va_arg(va,unsigned);
                    *b++=u16&0xff;u16>>=8;
                    *b++=u16&0xff;
                    break;
                case 3:/*int8*/
                case 2:/*uint8*/
                    u8=va_arg(va,int);
                    *b++=u8&0xff;
                break;     
                
                case 9:/*string*/
                case 8:/*uint8 array*/
                    data_len=va_arg(va,int);
                    *b++=data_len;        
                    
                    u16=data_len+packet.header.lolen;
                    packet.header.lolen=u16&0xff;
                    packet.header.type_hilen|=u16>>8;
                     
                    data_ptr=va_arg(va,uint8*);
                break;
                case 10:/*hwaddr*/
                    hw=va_arg(va,uint8*);
                    
                    *b++=*hw++;
                    *b++=*hw++;
                    *b++=*hw++;
                    *b++=*hw++;
                    *b++=*hw++;
                    *b++=*hw++;
                    
                break;
                case 11:/*uint16 array*/
                    data_len=va_arg(va,int);
                    *b++=data_len&0xff;        
                    *b++=data_len>>8;        
                    
                    u16=data_len+packet.header.lolen;
                    packet.header.lolen=u16&0xff;
                    packet.header.type_hilen|=u16>>8;
                     
                    data_ptr=va_arg(va,uint8*);
                break;
            }
            i=i>>4;
        }
        va_end(va);


            if(bglib_output)bglib_output(sizeof(struct ble_header)+apis[msgid].hdr.lolen,(uint8*)&packet,data_len,(uint8*)data_ptr);
}

static const struct ble_msg* const ble_class_system_rsp_handlers[]=
{
	&apis[ble_rsp_system_reset_idx],
	&apis[ble_rsp_system_hello_idx],
	&apis[ble_rsp_system_address_get_idx],
	&apis[ble_rsp_system_reg_write_idx],
	&apis[ble_rsp_system_reg_read_idx],
	&apis[ble_rsp_system_get_counters_idx],
	&apis[ble_rsp_system_get_connections_idx],
	&apis[ble_rsp_system_read_memory_idx],
	&apis[ble_rsp_system_get_info_idx],
	&apis[ble_rsp_system_endpoint_tx_idx],
	&apis[ble_rsp_system_whitelist_append_idx],
	&apis[ble_rsp_system_whitelist_remove_idx],
	&apis[ble_rsp_system_whitelist_clear_idx],
	&apis[ble_rsp_system_endpoint_rx_idx],
	&apis[ble_rsp_system_endpoint_set_watermarks_idx],
	&apis[ble_rsp_system_aes_setkey_idx],
	&apis[ble_rsp_system_aes_encrypt_idx],
	&apis[ble_rsp_system_aes_decrypt_idx],
};
static const struct ble_msg* const ble_class_system_evt_handlers[]=
{
	&apis[ble_evt_system_boot_idx],
	&apis[ble_evt_system_debug_idx],
	&apis[ble_evt_system_endpoint_watermark_rx_idx],
	&apis[ble_evt_system_endpoint_watermark_tx_idx],
	&apis[ble_evt_system_script_failure_idx],
	&apis[ble_evt_system_no_license_key_idx],
	&apis[ble_evt_system_protocol_error_idx],
};
static const struct ble_msg* const ble_class_flash_rsp_handlers[]=
{
	&apis[ble_rsp_flash_ps_defrag_idx],
	&apis[ble_rsp_flash_ps_dump_idx],
	&apis[ble_rsp_flash_ps_erase_all_idx],
	&apis[ble_rsp_flash_ps_save_idx],
	&apis[ble_rsp_flash_ps_load_idx],
	&apis[ble_rsp_flash_ps_erase_idx],
	&apis[ble_rsp_flash_erase_page_idx],
	&apis[ble_rsp_flash_write_data_idx],
	&apis[ble_rsp_flash_read_data_idx],
};
static const struct ble_msg* const ble_class_flash_evt_handlers[]=
{
	&apis[ble_evt_flash_ps_key_idx],
};
static const struct ble_msg* const ble_class_attributes_rsp_handlers[]=
{
	&apis[ble_rsp_attributes_write_idx],
	&apis[ble_rsp_attributes_read_idx],
	&apis[ble_rsp_attributes_read_type_idx],
	&apis[ble_rsp_attributes_user_read_response_idx],
	&apis[ble_rsp_attributes_user_write_response_idx],
	&apis[ble_rsp_attributes_send_idx],
};
static const struct ble_msg* const ble_class_attributes_evt_handlers[]=
{
	&apis[ble_evt_attributes_value_idx],
	&apis[ble_evt_attributes_user_read_request_idx],
	&apis[ble_evt_attributes_status_idx],
};
static const struct ble_msg* const ble_class_connection_rsp_handlers[]=
{
	&apis[ble_rsp_connection_disconnect_idx],
	&apis[ble_rsp_connection_get_rssi_idx],
	&apis[ble_rsp_connection_update_idx],
	&apis[ble_rsp_connection_version_update_idx],
	&apis[ble_rsp_connection_channel_map_get_idx],
	&apis[ble_rsp_connection_channel_map_set_idx],
	&apis[ble_rsp_connection_features_get_idx],
	&apis[ble_rsp_connection_get_status_idx],
	&apis[ble_rsp_connection_raw_tx_idx],
};
static const struct ble_msg* const ble_class_connection_evt_handlers[]=
{
	&apis[ble_evt_connection_status_idx],
	&apis[ble_evt_connection_version_ind_idx],
	&apis[ble_evt_connection_feature_ind_idx],
	&apis[ble_evt_connection_raw_rx_idx],
	&apis[ble_evt_connection_disconnected_idx],
};
static const struct ble_msg* const ble_class_attclient_rsp_handlers[]=
{
	&apis[ble_rsp_attclient_find_by_type_value_idx],
	&apis[ble_rsp_attclient_read_by_group_type_idx],
	&apis[ble_rsp_attclient_read_by_type_idx],
	&apis[ble_rsp_attclient_find_information_idx],
	&apis[ble_rsp_attclient_read_by_handle_idx],
	&apis[ble_rsp_attclient_attribute_write_idx],
	&apis[ble_rsp_attclient_write_command_idx],
	&apis[ble_rsp_attclient_indicate_confirm_idx],
	&apis[ble_rsp_attclient_read_long_idx],
	&apis[ble_rsp_attclient_prepare_write_idx],
	&apis[ble_rsp_attclient_execute_write_idx],
	&apis[ble_rsp_attclient_read_multiple_idx],
};
static const struct ble_msg* const ble_class_attclient_evt_handlers[]=
{
	&apis[ble_evt_attclient_indicated_idx],
	&apis[ble_evt_attclient_procedure_completed_idx],
	&apis[ble_evt_attclient_group_found_idx],
	&apis[ble_evt_attclient_attribute_found_idx],
	&apis[ble_evt_attclient_find_information_found_idx],
	&apis[ble_evt_attclient_attribute_value_idx],
	&apis[ble_evt_attclient_read_multiple_response_idx],
};
static const struct ble_msg* const ble_class_sm_rsp_handlers[]=
{
	&apis[ble_rsp_sm_encrypt_start_idx],
	&apis[ble_rsp_sm_set_bondable_mode_idx],
	&apis[ble_rsp_sm_delete_bonding_idx],
	&apis[ble_rsp_sm_set_parameters_idx],
	&apis[ble_rsp_sm_passkey_entry_idx],
	&apis[ble_rsp_sm_get_bonds_idx],
	&apis[ble_rsp_sm_set_oob_data_idx],
	&apis[ble_rsp_sm_whitelist_bonds_idx],
};
static const struct ble_msg* const ble_class_sm_evt_handlers[]=
{
	&apis[ble_evt_sm_smp_data_idx],
	&apis[ble_evt_sm_bonding_fail_idx],
	&apis[ble_evt_sm_passkey_display_idx],
	&apis[ble_evt_sm_passkey_request_idx],
	&apis[ble_evt_sm_bond_status_idx],
};
static const struct ble_msg* const ble_class_gap_rsp_handlers[]=
{
	&apis[ble_rsp_gap_set_privacy_flags_idx],
	&apis[ble_rsp_gap_set_mode_idx],
	&apis[ble_rsp_gap_discover_idx],
	&apis[ble_rsp_gap_connect_direct_idx],
	&apis[ble_rsp_gap_end_procedure_idx],
	&apis[ble_rsp_gap_connect_selective_idx],
	&apis[ble_rsp_gap_set_filtering_idx],
	&apis[ble_rsp_gap_set_scan_parameters_idx],
	&apis[ble_rsp_gap_set_adv_parameters_idx],
	&apis[ble_rsp_gap_set_adv_data_idx],
	&apis[ble_rsp_gap_set_directed_connectable_mode_idx],
};
static const struct ble_msg* const ble_class_gap_evt_handlers[]=
{
	&apis[ble_evt_gap_scan_response_idx],
	&apis[ble_evt_gap_mode_changed_idx],
};
static const struct ble_msg* const ble_class_hardware_rsp_handlers[]=
{
	&apis[ble_rsp_hardware_io_port_config_irq_idx],
	&apis[ble_rsp_hardware_set_soft_timer_idx],
	&apis[ble_rsp_hardware_adc_read_idx],
	&apis[ble_rsp_hardware_io_port_config_direction_idx],
	&apis[ble_rsp_hardware_io_port_config_function_idx],
	&apis[ble_rsp_hardware_io_port_config_pull_idx],
	&apis[ble_rsp_hardware_io_port_write_idx],
	&apis[ble_rsp_hardware_io_port_read_idx],
	&apis[ble_rsp_hardware_spi_config_idx],
	&apis[ble_rsp_hardware_spi_transfer_idx],
	&apis[ble_rsp_hardware_i2c_read_idx],
	&apis[ble_rsp_hardware_i2c_write_idx],
	&apis[ble_rsp_hardware_set_txpower_idx],
	&apis[ble_rsp_hardware_timer_comparator_idx],
	&apis[ble_rsp_hardware_io_port_irq_enable_idx],
	&apis[ble_rsp_hardware_io_port_irq_direction_idx],
	&apis[ble_rsp_hardware_analog_comparator_enable_idx],
	&apis[ble_rsp_hardware_analog_comparator_read_idx],
	&apis[ble_rsp_hardware_analog_comparator_config_irq_idx],
	&apis[ble_rsp_hardware_set_rxgain_idx],
	&apis[ble_rsp_hardware_usb_enable_idx],
};
static const struct ble_msg* const ble_class_hardware_evt_handlers[]=
{
	&apis[ble_evt_hardware_io_port_status_idx],
	&apis[ble_evt_hardware_soft_timer_idx],
	&apis[ble_evt_hardware_adc_result_idx],
	&apis[ble_evt_hardware_analog_comparator_status_idx],
};
static const struct ble_msg* const ble_class_test_rsp_handlers[]=
{
	&apis[ble_rsp_test_phy_tx_idx],
	&apis[ble_rsp_test_phy_rx_idx],
	&apis[ble_rsp_test_phy_end_idx],
	&apis[ble_rsp_test_phy_reset_idx],
	&apis[ble_rsp_test_get_channel_map_idx],
	&apis[ble_rsp_test_debug_idx],
	&apis[ble_rsp_test_channel_mode_idx],
};
static const struct ble_msg* const ble_class_dfu_rsp_handlers[]=
{
	&apis[ble_rsp_dfu_reset_idx],
	&apis[ble_rsp_dfu_flash_set_address_idx],
	&apis[ble_rsp_dfu_flash_upload_idx],
	&apis[ble_rsp_dfu_flash_upload_finish_idx],
};
static const struct ble_msg* const ble_class_dfu_evt_handlers[]=
{
	&apis[ble_evt_dfu_boot_idx],
};
const struct ble_class_handler_t ble_class_rsp_handlers[ble_cls_last]=
{
	{ble_class_system_rsp_handlers,18},
	{ble_class_flash_rsp_handlers,9},
	{ble_class_attributes_rsp_handlers,6},
	{ble_class_connection_rsp_handlers,9},
	{ble_class_attclient_rsp_handlers,12},
	{ble_class_sm_rsp_handlers,8},
	{ble_class_gap_rsp_handlers,11},
	{ble_class_hardware_rsp_handlers,21},
	{ble_class_test_rsp_handlers,7},
	{ble_class_dfu_rsp_handlers,4},
};
const struct ble_class_handler_t ble_class_evt_handlers[ble_cls_last]=
{
	{ble_class_system_evt_handlers,7},
	{ble_class_flash_evt_handlers,1},
	{ble_class_attributes_evt_handlers,3},
	{ble_class_connection_evt_handlers,5},
	{ble_class_attclient_evt_handlers,7},
	{ble_class_sm_evt_handlers,5},
	{ble_class_gap_evt_handlers,2},
	{ble_class_hardware_evt_handlers,4},
	{NULL,0},
	{ble_class_dfu_evt_handlers,1},
};
