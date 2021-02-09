#include "EthernetNetworkAdapter.h"
#include "Endianess.h"
#include <Clib.h>

void EthernetNetworkAdapter::send_frame(ProtocolType type, const MACAddress& destination, const BufferView& data)
{
	Buffer ethernet_frame_raw(data, ETHERNET_HEADER_SIZE);
	EthernetFrame& ethernet_frame = ethernet_frame_raw.convert_to<EthernetFrame>();

	m_mac.copy(ethernet_frame.src_mac_addr);
	destination.copy(ethernet_frame.dst_mac_addr);

	ethernet_frame.type = to_big_endian(static_cast<u16>(type));

	send_ethernet_frame(ethernet_frame_raw);
}

void EthernetNetworkAdapter::handle_received_ethernet_frame(const BufferView& data)
{
	const EthernetFrame& ethernet_frame = data.const_convert_to<EthernetFrame>();

	handle_received_frame(static_cast<ProtocolType>(to_big_endian(ethernet_frame.type)),
	                      BufferView{data, ETHERNET_HEADER_SIZE});
}