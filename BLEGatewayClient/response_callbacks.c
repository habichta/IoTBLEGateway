#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "cmd_def.h"
#include "tpl.h" //modified by Arthur Habicht
#include "debug.h"

extern int send_message_to_tcpsocket(char* buffer, size_t length);void ble_default(const void*v) {}
void ble_rsp_hardware_set_rxgain(const void*nul) {
}

void ble_rsp_system_aes_setkey(const void*nul) {

}

void ble_rsp_system_aes_encrypt(
	const struct ble_msg_system_aes_encrypt_rsp_t*msg) {

}

void ble_rsp_system_aes_decrypt(
	const struct ble_msg_system_aes_decrypt_rsp_t*msg) {

}

void ble_rsp_flash_read_data(const struct ble_msg_flash_read_data_rsp_t*msg) {

}

void ble_rsp_test_channel_mode(const void*nul) {

}

void ble_evt_system_protocol_error(
	const struct ble_msg_system_protocol_error_evt_t*msg) {

}

void ble_evt_system_no_license_key(const void*nul) {

}

void ble_evt_attributes_status(
	const struct ble_msg_attributes_status_evt_t *msg) {

}

void ble_rsp_attclient_indicate_confirm(
	const struct ble_msg_attclient_indicate_confirm_rsp_t*msg) {

}

void ble_rsp_test_debug(const struct ble_msg_test_debug_rsp_t*msg) {

}

void ble_rsp_hardware_set_txpower(const void *nul) {

}

void ble_rsp_attclient_read_multiple(
	const struct ble_msg_attclient_read_multiple_rsp_t *msg) {

}

void ble_evt_attclient_read_multiple_response(
	const struct ble_msg_attclient_read_multiple_response_evt_t *msg) {

}

void ble_rsp_attclient_prepare_write(
	const struct ble_msg_attclient_prepare_write_rsp_t *msg) {

}

void ble_rsp_attclient_execute_write(
	const struct ble_msg_attclient_execute_write_rsp_t *msg) {

}

void ble_evt_attributes_user_read_request(
	const struct ble_msg_attributes_user_read_request_evt_t *msg) {

}

void ble_rsp_attributes_user_read_response(const void * nul) {

}

void ble_rsp_attributes_user_write_response(const void * nul) {

}

void ble_rsp_system_whitelist_clear(const void *nul) {

}

void ble_rsp_attclient_write_command(
	const struct ble_msg_attclient_write_command_rsp_t *msg) {

}

void ble_rsp_attclient_reserved(const void *nul) {

}

void ble_rsp_attclient_read_long(
	const struct ble_msg_attclient_read_long_rsp_t *msg) {

}

void ble_rsp_system_whitelist_append(
	const struct ble_msg_system_whitelist_append_rsp_t *msg) {

}

void ble_rsp_sm_set_parameters(const void *nul) {

}

void ble_rsp_sm_passkey_entry(const struct ble_msg_sm_passkey_entry_rsp_t *msg) {

}

void ble_rsp_sm_get_bonds(const struct ble_msg_sm_get_bonds_rsp_t *msg) {

}

void ble_rsp_sm_set_oob_data(const void *nul) {

}

void ble_rsp_sm_whitelist_bonds(
	const struct ble_msg_sm_whitelist_bonds_rsp_t *msg) {

}

void ble_rsp_sm_set_security_mode(const void *nul) {

}

void ble_rsp_gap_set_filtering(
	const struct ble_msg_gap_set_filtering_rsp_t *msg) {

}

void ble_rsp_gap_set_adv_parameters(
	const struct ble_msg_gap_set_adv_parameters_rsp_t *msg) {

}

void ble_rsp_hardware_io_port_write(
	const struct ble_msg_hardware_io_port_write_rsp_t *msg) {

}

void ble_rsp_hardware_io_port_read(
	const struct ble_msg_hardware_io_port_read_rsp_t *msg) {

}

void ble_rsp_hardware_timer_comparator(
	const struct ble_msg_hardware_timer_comparator_rsp_t *msg) {

}

void ble_rsp_hardware_spi_config(
	const struct ble_msg_hardware_spi_config_rsp_t *msg) {

}

void ble_rsp_hardware_spi_transfer(
	const struct ble_msg_hardware_spi_transfer_rsp_t *msg) {

}

void ble_rsp_hardware_i2c_read(
	const struct ble_msg_hardware_i2c_read_rsp_t *msg) {

}

void ble_rsp_hardware_i2c_write(
	const struct ble_msg_hardware_i2c_write_rsp_t *msg) {

}

void ble_rsp_hardware_io_port_irq_enable(
	const struct ble_msg_hardware_io_port_irq_enable_rsp_t *msg) {

}

void ble_rsp_hardware_io_port_irq_direction(
	const struct ble_msg_hardware_io_port_irq_direction_rsp_t *msg) {

}

void ble_rsp_hardware_analog_comparator_enable(const void *nul) {

}

void ble_rsp_hardware_analog_comparator_read(
	const struct ble_msg_hardware_analog_comparator_read_rsp_t *msg) {

}

void ble_rsp_hardware_analog_comparator_config_irq(
	const struct ble_msg_hardware_analog_comparator_config_irq_rsp_t *msg) {

}

void ble_rsp_test_get_channel_map(
	const struct ble_msg_test_get_channel_map_rsp_t *msg) {

}

void ble_evt_attributes_value(const struct ble_msg_attributes_value_evt_t *msg) {

}

void ble_evt_sm_bonding_fail(const struct ble_msg_sm_bonding_fail_evt_t *msg) {

}

void ble_evt_sm_passkey_display(
	const struct ble_msg_sm_passkey_display_evt_t *msg) {

}

void ble_evt_sm_passkey_request(
	const struct ble_msg_sm_passkey_request_evt_t *msg) {

}

void ble_evt_sm_bond_status(const struct ble_msg_sm_bond_status_evt_t *msg) {

}

void ble_rsp_gap_set_adv_data(const struct ble_msg_gap_set_adv_data_rsp_t *msg) {

}

void ble_rsp_gap_set_scan_parameters(
	const struct ble_msg_gap_set_scan_parameters_rsp_t *msg) {
	DebugPrint("%s\n","Callback Set Scan Parameters");

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_rsp_gap_set_scan_parameters_idx;
char* buffer;
size_t length;
tpl_node* tn;
if ((tn = tpl_map("vv", &id, &(msg->result))) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint("error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed
}

void ble_rsp_gap_set_directed_connectable_mode(
	const struct ble_msg_gap_set_directed_connectable_mode_rsp_t *msg) {

}

void ble_rsp_hardware_io_port_config_direction(
	const struct ble_msg_hardware_io_port_config_direction_rsp_t *msg) {

}

void ble_rsp_hardware_io_port_config_pull(
	const struct ble_msg_hardware_io_port_config_pull_rsp_t *msg) {

}

void ble_rsp_hardware_io_port_config_function(
	const struct ble_msg_hardware_io_port_config_function_rsp_t *msg) {

}

void ble_rsp_gap_set_privacy_flags(const void *nul) {

}

void ble_rsp_gap_connect_selective(
	const struct ble_msg_gap_connect_selective_rsp_t *msg) {

}

void ble_rsp_system_whitelist_remove(
	const struct ble_msg_system_whitelist_remove_rsp_t *msg) {

}

void ble_rsp_system_reset(const void* nul) {
}

void ble_rsp_system_hello(const void* nul) {
}

void ble_rsp_system_address_get(
	const struct ble_msg_system_address_get_rsp_t *msg) {
}

void ble_rsp_system_reg_write(const struct ble_msg_system_reg_write_rsp_t *msg) {
}

void ble_rsp_system_reg_read(const struct ble_msg_system_reg_read_rsp_t *msg) {
}

void ble_rsp_system_get_counters(
	const struct ble_msg_system_get_counters_rsp_t *msg) {
}

void ble_rsp_system_get_connections(
	const struct ble_msg_system_get_connections_rsp_t *msg) {
DebugPrint("%s\n","Callback Get Connections");
//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_rsp_system_get_connections_idx;
char* buffer;
size_t length;
tpl_node* tn;
if ((tn = tpl_map("vc", &id, &(msg->maxconn))) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint("error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_rsp_system_read_memory(
	const struct ble_msg_system_read_memory_rsp_t *msg) {
}

void ble_rsp_system_get_info(const struct ble_msg_system_get_info_rsp_t *msg) {
	DebugPrint("%s\n","Callback Get Info Response");

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_rsp_system_get_info_idx;
char* buffer;
size_t length;
tpl_node* tn;

uint16 build = msg->build;
uint8 hw = msg->hw;
uint16 ll_version = msg->ll_version;
uint16 major = msg->major;

uint16 minor = msg->minor;
uint16 patch = msg->patch;
uint8 protocol_version = msg->protocol_version;


if ((tn = tpl_map("vvvvvvcc", &id, &build,&ll_version,&major,&minor,&patch,&hw,&protocol_version)) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);
send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_rsp_system_endpoint_tx(
	const struct ble_msg_system_endpoint_tx_rsp_t *msg) {
}

void ble_rsp_flash_ps_defrag(const void* nul) {
}

void ble_rsp_flash_ps_dump(const void* nul) {
}

void ble_rsp_flash_ps_erase_all(const void* nul) {
}

void ble_rsp_flash_ps_save(const struct ble_msg_flash_ps_save_rsp_t *msg) {
}

void ble_rsp_flash_ps_load(const struct ble_msg_flash_ps_load_rsp_t *msg) {
}

void ble_rsp_flash_ps_erase(const void* nul) {
}

void ble_rsp_flash_erase_page(const struct ble_msg_flash_erase_page_rsp_t *msg) {
}

void ble_rsp_flash_write_data(const struct ble_msg_flash_write_data_rsp_t * msg) {
}

void ble_rsp_attributes_write(const struct ble_msg_attributes_write_rsp_t *msg) {
}

void ble_rsp_attributes_send(const struct ble_msg_attributes_send_rsp_t *msg) {
}

void ble_rsp_attributes_read(const struct ble_msg_attributes_read_rsp_t *msg) {
}

void ble_rsp_attributes_read_type(
	const struct ble_msg_attributes_read_type_rsp_t *msg) {
}

void ble_rsp_connection_disconnect(
	const struct ble_msg_connection_disconnect_rsp_t *msg) {


DebugPrint( "%s\n", "Callback Disconnect Response");
//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_rsp_connection_disconnect_idx;
char* buffer;
size_t length;
tpl_node* tn;

uint8_t con = msg->connection;
uint16_t res = msg->result;

if ((tn = tpl_map("vcv", &id, &con, &res)) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_rsp_connection_get_rssi(
	const struct ble_msg_connection_get_rssi_rsp_t *msg) {
}

void ble_rsp_connection_update(
	const struct ble_msg_connection_update_rsp_t *msg) {

	DebugPrint( "%s\n", "Callback Connection Update Response");

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_rsp_connection_update_idx;
char* buffer;
size_t length;
tpl_node* tn;

if ((tn = tpl_map("vcv", &id, &(msg->connection), &(msg->result))) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint("error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_rsp_connection_version_update(
	const struct ble_msg_connection_version_update_rsp_t *msg) {
}

void ble_rsp_connection_channel_map_get(
	const struct ble_msg_connection_channel_map_get_rsp_t *msg) {
}

void ble_rsp_connection_channel_map_set(
	const struct ble_msg_connection_channel_map_set_rsp_t *msg) {
}

void ble_rsp_connection_features_get(
	const struct ble_msg_connection_features_get_rsp_t *msg) {
}

void ble_rsp_connection_get_status(
	const struct ble_msg_connection_get_status_rsp_t *msg) {

	DebugPrint( "%s\n", "Callback Connection Get Status Response");

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_rsp_connection_get_status_idx;
char* buffer;
size_t length;
tpl_node* tn;
if ((tn = tpl_map("vc", &id, &(msg->connection))) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_rsp_connection_raw_tx(
	const struct ble_msg_connection_raw_tx_rsp_t *msg) {
}

void ble_rsp_attclient_find_by_type_value(
	const struct ble_msg_attclient_find_by_type_value_rsp_t *msg) {
}

void ble_rsp_attclient_read_by_group_type(
	const struct ble_msg_attclient_read_by_group_type_rsp_t *msg) {
	DebugPrint( "%s\n", "Callback Read By Group Response");

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_rsp_attclient_read_by_group_type_idx;
char* buffer;
size_t length;
tpl_node* tn;
if ((tn = tpl_map("vcv", &id, &(msg->connection), &(msg->result))) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_rsp_attclient_read_by_type(
	const struct ble_msg_attclient_read_by_type_rsp_t *msg) {
	DebugPrint( "%s\n", "Callback Read By Type Response");

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_rsp_attclient_read_by_type_idx;
char* buffer;
size_t length;
tpl_node* tn;
if ((tn = tpl_map("vcv", &id, &(msg->connection), &(msg->result))) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint("error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_rsp_attclient_find_information(
	const struct ble_msg_attclient_find_information_rsp_t *msg) {
	DebugPrint( "%s\n", "Callback Find Information");


//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_rsp_attclient_find_information_idx;
char* buffer;
size_t length;
tpl_node* tn;
if ((tn = tpl_map("vcv", &id, &(msg->connection), &(msg->result))) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));
}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_rsp_attclient_read_by_handle(
	const struct ble_msg_attclient_read_by_handle_rsp_t *msg) {
	DebugPrint( "%s\n", "Read by Handle Response");
//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_rsp_attclient_read_by_handle_idx;
char* buffer;
size_t length;
tpl_node* tn;
if ((tn = tpl_map("vcv", &id, &(msg->connection), &(msg->result))) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));
}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_rsp_attclient_attribute_write(
	const struct ble_msg_attclient_attribute_write_rsp_t *msg) {

	DebugPrint( "%s\n", "Write to Handle Response");


//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_rsp_attclient_attribute_write_idx;
char* buffer;
size_t length;
tpl_node* tn;
if ((tn = tpl_map("vcv", &id, &(msg->connection), &(msg->result))) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));
}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_rsp_sm_encrypt_start(const struct ble_msg_sm_encrypt_start_rsp_t *msg) {
}

void ble_rsp_sm_set_bondable_mode(const void* nul) {
}

void ble_rsp_sm_delete_bonding(
	const struct ble_msg_sm_delete_bonding_rsp_t *msg) {
}

void ble_rsp_gap_set_address_mode(const void* nul) {
}

void ble_rsp_gap_set_mode(const struct ble_msg_gap_set_mode_rsp_t *msg) { //TODO
}

void ble_rsp_gap_discover(const struct ble_msg_gap_discover_rsp_t *msg) {

	DebugPrint( "%s\n", "Discover Response");

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_rsp_gap_discover_idx;
char* buffer;
size_t length;
tpl_node* tn;
if ((tn = tpl_map("vv", &id, &(msg->result))) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_rsp_gap_connect_direct(
	const struct ble_msg_gap_connect_direct_rsp_t *msg) {
	DebugPrint( "%s\n", "Callback Connect Response");

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_rsp_gap_connect_direct_idx;
char* buffer;
size_t length;
tpl_node* tn;
if ((tn = tpl_map("vvc", &id, &(msg->result),&(msg->connection_handle))) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_rsp_gap_end_procedure(
	const struct ble_msg_gap_end_procedure_rsp_t *msg) {
	DebugPrint( "%s\n", "Callback End Procedure Response");

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_rsp_gap_end_procedure_idx;
char* buffer;
size_t length;
tpl_node* tn;
if ((tn = tpl_map("vv", &id, &(msg->result))) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_rsp_hardware_io_port_config_irq(
	const struct ble_msg_hardware_io_port_config_irq_rsp_t *msg) {
}

void ble_rsp_hardware_set_soft_timer(
	const struct ble_msg_hardware_set_soft_timer_rsp_t *msg) {
}

void ble_rsp_hardware_adc_read(
	const struct ble_msg_hardware_adc_read_rsp_t *msg) {
}

void ble_rsp_test_phy_tx(const void* nul) {
}

void ble_rsp_test_phy_rx(const void* nul) {
}

void ble_rsp_test_phy_end(const struct ble_msg_test_phy_end_rsp_t *msg) {
}

void ble_rsp_test_phy_reset(const void* nul) {
}

void ble_evt_system_boot(const struct ble_msg_system_boot_evt_t *msg) {
}

void ble_evt_system_debug(const struct ble_msg_system_debug_evt_t *msg) {
}

void ble_evt_system_endpoint_watermark_rx(
	const struct ble_msg_system_endpoint_watermark_rx_evt_t *msg) {
}

void ble_evt_system_endpoint_watermark_tx(
	const struct ble_msg_system_endpoint_watermark_tx_evt_t *msg) {
}

void ble_rsp_system_endpoint_set_watermarks(
	const struct ble_msg_system_endpoint_set_watermarks_rsp_t*msg) {

}

void ble_rsp_hardware_usb_enable(
	const struct ble_msg_hardware_usb_enable_rsp_t *msg) {
}

void ble_rsp_system_endpoint_rx(
	const struct ble_msg_system_endpoint_rx_rsp_t*msg) {

}

void ble_evt_system_script_failure(
	const struct ble_msg_system_script_failure_evt_t *msg) {
}

void ble_evt_flash_ps_key(const struct ble_msg_flash_ps_key_evt_t *msg) {
}

void ble_evt_connection_status(
	const struct ble_msg_connection_status_evt_t *msg) {
	DebugPrint( "%s\n", "Callback Connection Status Event");


bd_addr mac_address = msg->address;

uint8 address_type = msg->address_type, flags = msg->flags, connection_handle =
		msg->connection, bonding = msg->bonding;
uint16 conn_interval = msg->conn_interval, slave_latency = msg->latency,
		timeout = msg->timeout;

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_evt_connection_status_idx;
char* buffer;
size_t length;
tpl_node* tn;
if ((tn = tpl_map("vS(c#)cccvvvc", &id, &mac_address, MAC_SIZE,
		&connection_handle, &flags, &address_type, &conn_interval, &timeout,
		&slave_latency, &bonding)) == NULL) //serialize to "id|msg" bytestream
{

	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}

tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_evt_connection_version_ind(
	const struct ble_msg_connection_version_ind_evt_t *msg) {
}

void ble_evt_connection_feature_ind(
	const struct ble_msg_connection_feature_ind_evt_t *msg) {
}

void ble_evt_connection_raw_rx(
	const struct ble_msg_connection_raw_rx_evt_t *msg) {
}

void ble_evt_connection_disconnected(
	const struct ble_msg_connection_disconnected_evt_t *msg) {

DebugPrint( "%s\n", "Callback Connection Disconnected Event");
//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_evt_connection_disconnected_idx;
char* buffer;
size_t length;
tpl_node* tn;
if ((tn = tpl_map("vcv", &id, &(msg->connection),&(msg->reason))) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_evt_attclient_indicated(
	const struct ble_msg_attclient_indicated_evt_t *msg) {
}

void ble_evt_attclient_procedure_completed(
	const struct ble_msg_attclient_procedure_completed_evt_t *msg) {
	DebugPrint( "%s\n", "Callback Procedure Completed Event");

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_evt_attclient_procedure_completed_idx;
char* buffer;
size_t length;
tpl_node* tn;
if ((tn = tpl_map("vcvv", &id, &(msg->connection),&(msg->result), &(msg->chrhandle))) == NULL) //serialize to "id|msg" bytestream
{
	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);

send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_evt_attclient_group_found(
	const struct ble_msg_attclient_group_found_evt_t *msg) {
	DebugPrint( "%s\n", "Callback Group Found Event");

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_evt_attclient_group_found_idx, start = msg->start, end =
		msg->end;
char* buffer;
size_t length;
tpl_node* tn;
tpl_bin tb;
//unpack data, since tpl doesn't support fixed arrays in nested structures

uint8 connection_handle = msg->connection;

if ((tn = tpl_map("vcvvB", &id, &connection_handle, &start, &end, &tb)) == NULL) //serialize, 6= length of MAC Address in bytes
{
	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tb.sz = msg->uuid.len;
tb.addr = msg->uuid.data;
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);
send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_evt_attclient_attribute_found(
	const struct ble_msg_attclient_attribute_found_evt_t *msg) {
}

void ble_evt_attclient_find_information_found(
	const struct ble_msg_attclient_find_information_found_evt_t *msg) {
	DebugPrint("%s\n", "Callback Information Found Event");

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_evt_attclient_find_information_found_idx, attribute_handle =
		msg->chrhandle;
char* buffer;
size_t length;
tpl_node* tn;
tpl_bin tb;
//unpack data, since tpl doesn't support fixed arrays in nested structures

uint8 connection_handle = msg->connection;

if ((tn = tpl_map("vcvB", &id, &connection_handle, &attribute_handle, &tb))
		== NULL) //serialize, 6= length of MAC Address in bytes
{
	DebugPrint( "error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tb.sz = msg->uuid.len;
tb.addr = msg->uuid.data;
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);
send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_evt_attclient_attribute_value(
	const struct ble_msg_attclient_attribute_value_evt_t *msg) {
	DebugPrint("%s\n", "Callback Attribute Value Event");

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_evt_attclient_attribute_value_idx, attribute_handle =
		msg->atthandle;
uint8 connection_handle = msg->connection, attribute_type = msg->type;

char* buffer;
size_t length;
tpl_node* tn;
tpl_bin tb;
//unpack data, since tpl doesn't support fixed arrays in nested structures

if ((tn = tpl_map("vcvcB", &id, &connection_handle, &attribute_handle,
		&attribute_type, &tb)) == NULL) //serialize, 6= length of MAC Address in bytes
{
	DebugPrint("error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tb.sz = msg->value.len;
tb.addr = msg->value.data;
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);
send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed

}

void ble_evt_sm_smp_data(const struct ble_msg_sm_smp_data_evt_t *msg) {
}

void ble_evt_gap_scan_response(
	const struct ble_msg_gap_scan_response_evt_t *msg) {
	DebugPrint("%s\n", "Callback Scan Response Event");

//serializing data for transmission over sockets, little-endian to big-endian conversion
uint16_t id = ble_evt_gap_scan_response_idx;
char* buffer;
size_t length;
tpl_node* tn;
tpl_bin tb;
//unpack data, since tpl doesn't support fixed arrays in nested structures
bd_addr mac_address = msg->sender;
uint8* data_pointer = msg->data.data;
int8 rssi = msg->rssi;
uint8 packet_type = msg->packet_type;
uint8 address_type = msg->address_type;
uint8 bond = msg->bond;
if ((tn = tpl_map("vS(c#)ccccB", &id, &mac_address, MAC_SIZE, &rssi,
		&packet_type, &address_type, &bond, &tb)) == NULL) //serialize, 6= length of MAC Address in bytes
{
	DebugPrint("error %d, response_callback could not serialize data: %s ",
			errno, strerror(errno));

}
tb.sz = msg->data.len;
tb.addr = data_pointer;
tpl_pack(tn, 0);
tpl_dump(tn, TPL_MEM, &buffer, &length);
tpl_free(tn);
send_message_to_tcpsocket(buffer, length);

free(buffer); //tpl allocates data in heap, so must be freed
}

void ble_evt_gap_mode_changed(const struct ble_msg_gap_mode_changed_evt_t *msg) { //TODO
}

void ble_evt_hardware_io_port_status(
	const struct ble_msg_hardware_io_port_status_evt_t *msg) {
}

void ble_evt_hardware_soft_timer(
	const struct ble_msg_hardware_soft_timer_evt_t *msg) {
}

void ble_evt_hardware_adc_result(
	const struct ble_msg_hardware_adc_result_evt_t *msg) {
}

void ble_evt_hardware_analog_comparator_status(
	const struct ble_msg_hardware_analog_comparator_status_evt_t *msg) {
}

/**Reset system**/
void ble_rsp_dfu_reset(const void *nul) {
}

/**set address for flashing**/
void ble_rsp_dfu_flash_set_address(
	const struct ble_msg_dfu_flash_set_address_rsp_t *msg) {
}

/**Upload binary for flashing. Address will be updated automatically.**/
void ble_rsp_dfu_flash_upload(const struct ble_msg_dfu_flash_upload_rsp_t *msg) {
}
/**Uploading is finished.**/
void ble_rsp_dfu_flash_upload_finish(
	const struct ble_msg_dfu_flash_upload_finish_rsp_t *msg) {
}
/**Device booted up in dfu, and is ready to receive commands**/
void ble_evt_dfu_boot(const struct ble_msg_dfu_boot_evt_t *msg) {
}
