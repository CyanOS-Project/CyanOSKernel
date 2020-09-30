#include "StringView.h"
#include "Types.h"

class ArgumentParser
{
  private:
	StringView m_argument;
	size_t m_count;

  public:
	ArgumentParser(StringView argument);
	~ArgumentParser() = default;
	size_t count() const;
	StringView operator[](int) const;
};
