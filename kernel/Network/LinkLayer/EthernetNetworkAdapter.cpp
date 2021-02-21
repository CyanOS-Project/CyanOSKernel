#include "EthernetNetworkAdapter.h"
#include <Clib.h>
#include <NetworkAlgorithms.h>

void EthernetNetworkAdapter::send_frame(ProtocolType type, MACAddress destination, const BufferView& data)
{
	Buffer ethernet_frame_raw(data, ETHERNET_HEADER_SIZE);
	EthernetFrame& ethernet_frame = ethernet_frame_raw.convert_to<EthernetFrame>();

	m_mac.copy(ethernet_frame.src_mac_addr);
	destination.copy(ethernet_frame.dst_mac_addr);

	ethernet_frame.type = network_word16(static_cast<u16>(type));

	send_ethernet_frame(ethernet_frame_raw);
}

void EthernetNetworkAdapter::handle_received_ethernet_frame(const BufferView& data)
{
	Buffer data_copy{data};
	const EthernetFrame& ethernet_frame = data_copy.const_convert_to<EthernetFrame>();

	handle_received_frame(static_cast<ProtocolType>(network_word16(ethernet_frame.type)),
	                      BufferView{data_copy, ETHERNET_HEADER_SIZE});
}