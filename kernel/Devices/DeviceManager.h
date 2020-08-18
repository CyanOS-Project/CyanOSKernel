#pragma once

#include "DeviceDescription.h"
#include "DeviceNode.h"
#include "utils/IterationDecision.h"
#include "utils/List.h"
#include "utils/Result.h"
#include "utils/types.h"

class DeviceManager
{
  public:
	static void setup();
	static Result<void> add_device(DeviceNode* node);
	static Result<void> remove_device(const StringView& node);
	static Result<DeviceNode&> get_device(const StringView& device_name);
	static Result<DeviceDescription> open(const StringView& path, int mode, int flags);

	template <typename Callback> static void for_each_device(Callback callback)
	{
		auto&& device = m_devices->begin();
		while (device != m_devices->end()) {
			auto iterator_copy = device++;
			auto ret = callback(*iterator_copy);
			if (ret == IterationDecision::Break) {
				break;
			}
		}
	}

  private:
	static List<DeviceNode*>* m_devices; // FIXME: Should contigious array.
};
