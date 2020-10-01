#include "Assert.h"
#include "String.h"
#include "StringView.h"
#include "Types.h"
#include "Vector.h"

class ArgumentParser
{
  private:
	String m_data;
	Vector<StringView> m_argument_list;

  public:
	ArgumentParser(StringView argument);
	~ArgumentParser() = default;
	size_t count() const;
	StringView operator[](int) const;
};
