#pragma once
#include <Types.h>

class NetworkAdapter
{

  public:
	NetworkAdapter() = default;
	virtual void send_frame(const void* data, size_t size) = 0;

  protected:
	void handle_received_frame(const void* data, size_t size);

  private:
};
