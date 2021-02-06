#include "EthernetNetworkAdapter.h"
#include "Endianess.h"
#include <Clib.h>

void EthernetNetworkAdapter::send_frame(ProtocolType type, const MACAddress& destination, const void* data, size_t len)
{
	uint8_t* ethernet_frame_raw = new uint8_t[len + ETHERNET_HEADER_SIZE];
	EthernetFrame* ethernet_frame = reinterpret_cast<EthernetFrame*>(ethernet_frame_raw);

	m_mac.copy(ethernet_frame->src_mac_addr);
	destination.copy(ethernet_frame->dst_mac_addr);

	memcpy(ethernet_frame->data, data, len);
	ethernet_frame->type = to_big_endian(static_cast<uint16_t>(type));

	send_ethernet_frame(ethernet_frame_raw, len + ETHERNET_HEADER_SIZE);

	delete ethernet_frame_raw;
}

void EthernetNetworkAdapter::handle_received_ethernet_frame(const void* data, size_t size)
{
	const EthernetFrame* ethernet_frame = reinterpret_cast<const EthernetFrame*>(data);
	handle_received_frame(static_cast<ProtocolType>(to_big_endian(ethernet_frame->type)), ethernet_frame->data,
	                      size - ETHERNET_HEADER_SIZE);
}