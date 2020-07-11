#pragma once

class ResultError
{
  public:
	const unsigned m_error;
	ResultError(unsigned error) : m_error(error)
	{
	}
	~ResultError()
	{
	}
};

template <typename T> class Result
{
  private:
	const ResultError m_error;
	T ret_value;

  public:
	Result(const Result& other) : m_error(other.m_error), ret_value(other.ret_value)
	{
	}

	Result(const ResultError& error) : m_error(error)
	{
	}

	Result(const T& result) : m_error(ResultError(0)), ret_value(result)
	{
	}

	const bool is_error() const
	{
		return m_error.m_error != 0;
	}

	const unsigned error() const
	{
		return m_error.m_error;
	}

	T& value()
	{
		return ret_value;
	}

	~Result()
	{
	}
};

template <typename T> class Result<T&>
{
  private:
	const ResultError m_error;
	T* ret_value;

  public:
	Result(const Result& other) : m_error(other.m_error), ret_value(other.ret_value)
	{
	}

	Result(const ResultError& error) : m_error(error)
	{
	}

	Result(T& result) : m_error(ResultError(0)), ret_value(&result)
	{
	}

	bool is_error() const
	{
		return m_error.m_error != 0;
	}

	unsigned error() const
	{
		return m_error.m_error;
	}

	T& value()
	{
		return *ret_value;
	}

	~Result()
	{
	}
};

template <> class Result<void>
{
  private:
	const ResultError m_error;

  public:
	Result(const Result& other) : m_error(other.m_error)
	{
	}

	Result(const ResultError& error) : m_error(error)
	{
	}

	bool is_error() const
	{
		return m_error.m_error != 0;
	}

	unsigned error() const
	{
		return m_error.m_error;
	}

	~Result()
	{
	}
};