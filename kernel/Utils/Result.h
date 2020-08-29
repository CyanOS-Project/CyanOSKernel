#pragma once
#include "Algorithms.h"
#include "Assert.h"
class ResultError
{
  public:
	const unsigned m_error;
	explicit ResultError(unsigned error) : m_error(error)
	{
	}

	ResultError(const ResultError& other) : m_error(other.m_error)
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
	union {
		T m_storage;
		const char m_storage_empty[sizeof(T)];
	};

  public:
	Result(const ResultError& error) : m_error(error)
	{
	}

	Result(const Result& other) : m_error(other.m_error), m_storage(other.m_storage)
	{
	}

	Result(Result&& other) : m_error(other.m_error), m_storage(move(other.m_storage))
	{
	}

	Result(const T& result) : m_error(ResultError(0)), m_storage(result)
	{
	}

	Result(T&& result) : m_error(ResultError(0)), m_storage(move(result))
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
		ASSERT(!is_error());
		return m_storage;
	}

	~Result()
	{
	}
};

template <typename T> class Result<T&>
{
  private:
	const ResultError m_error;
	T* const m_storage;

  public:
	Result(const ResultError& error) : m_error(error), m_storage(nullptr)
	{
	}

	Result(const Result& other) : m_error(other.m_error), m_storage(other.m_storage)
	{
	}

	Result(T& result) : m_error(ResultError(0)), m_storage(&result)
	{
	}

	Result(T&& result) = delete;

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
		ASSERT(!is_error());
		return *m_storage;
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
	Result(const ResultError& error) : m_error(error)
	{
	}

	Result(const Result& other) : m_error(other.m_error)
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