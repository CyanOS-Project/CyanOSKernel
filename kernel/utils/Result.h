#pragma once

class ResultError
{
	unsigned m_error;

  public:
	ResultError(unsigned error) : m_error(error)
	{
	}
	~ResultError()
	{
	}

	inline bool is_error()
	{
		return m_error != 0;
	}

	inline unsigned error()
	{
		return m_error;
	}

  private:
};

template <typename T> class Result
{
  private:
	ResultError m_error;
	T ret_value;

  public:
	Result(const Result& other)
	{
		m_error = other.m_error;
		ret_value = other.ret_value;
	}

	Result(ResultError&& error) : m_error(error)
	{
	}

	Result(T& result) : ret_value(result), m_error(ResultError(0))
	{
	}

	Result(T&& result) : ret_value(result), m_error(ResultError(0))
	{
	}

	bool is_error()
	{
		return m_error.is_error();
	}

	unsigned error()
	{
		return m_error.error();
	}

	T& value()
	{
		return ret_value;
	}

	~Result()
	{
	}
};