#include "assert.h"
#include "string.h"
#include "string_view.h"
#include "types.h"
#include "vector.h"

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
