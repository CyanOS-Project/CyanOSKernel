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

	void trim_arguments();

  public:
	ArgumentParser(StringView argument);
	~ArgumentParser() = default;
	size_t count() const;
	StringView operator[](size_t) const;
};
