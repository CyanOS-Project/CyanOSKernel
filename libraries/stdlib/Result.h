#pragma once
#include "Algorithms.h"
#include "Assert.h"
class ResultError
{
  public:
	unsigned m_error;
	explicit ResultError(unsigned error) : m_error(error) {}

	ResultError(const ResultError& other) : m_error(other.m_error) {}

	ResultError& operator=(const ResultError& other)
	{
		m_error = other.m_error;
		return *this;
	}

	~ResultError() {}
};

template <typename T> class Result
{
  private:
	ResultError m_error;
	union {
		T m_storage;
		const char m_storage_empty[sizeof(T)];
	};

  public:
	Result(const Result<T>& other) : m_error(other.m_error), m_storage(other.m_storage) {}

	Result(Result<T>&& other) : m_error(other.m_error), m_storage(move(other.m_storage)) {}

	Result<T>& operator=(const Result<T>& result)
	{
		m_error = result.m_error;
		m_storage = result.m_storage;
		return *this;
	}

	Result<T>& operator=(Result<T>&& result)
	{
		m_error = result.m_error;
		m_storage = move(result.m_storage);
		return *this;
	}

	Result(const ResultError& error) : m_error(error) {}

	Result(const T& result) : m_error(ResultError(0)), m_storage(result) {}

	Result(T&& result) : m_error(ResultError(0)), m_storage(move(result)) {}

	bool is_error() const { return m_error.m_error != 0; }

	unsigned error() const { return m_error.m_error; }

	T& value()
	{
		ASSERT(!is_error());
		return m_storage;
	}

	~Result() {}
};

template <typename T> class Result<T&>
{
  private:
	ResultError m_error;
	T* const m_storage;

  public:
	Result(const Result<T&>& other) : m_error(other.m_error), m_storage(other.m_storage) {}

	Result(Result<T&>&& other) : m_error(other.m_error), m_storage(move(other.m_storage)) {}

	Result<T&>& operator=(const Result<T&>& result)
	{
		m_error = result.m_error;
		m_storage = result.m_storage;
		return *this;
	}

	Result<T&>& operator=(Result<T&>&& result)
	{
		m_error = result.m_error;
		m_storage = move(result.m_storage);
		return *this;
	}

	Result(const ResultError& error) : m_error(error), m_storage(nullptr) {}

	Result(T& result) : m_error(ResultError(0)), m_storage(&result) {}

	Result(T&& result) = delete;

	bool is_error() const { return m_error.m_error != 0; }

	unsigned error() const { return m_error.m_error; }

	T& value()
	{
		ASSERT(!is_error());
		return *m_storage;
	}

	~Result() {}
};

template <> class Result<void>
{
  private:
	ResultError m_error;

  public:
	Result(const Result<void>& other) : m_error(other.m_error) {}

	Result<void>& operator=(const Result<void>& other)
	{
		m_error = other.m_error;
		return *this;
	}

	Result(const ResultError& error) : m_error(error) {}

	bool is_error() const { return m_error.m_error != 0; }

	unsigned error() const { return m_error.m_error; }

	~Result() {}
};