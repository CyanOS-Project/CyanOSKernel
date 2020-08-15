template <typename T> class ScopedLock
{
  private:
	T& m_lock;

  public:
	explicit ScopedLock(T& lock) : m_lock{lock}
	{
		m_lock.acquire();
	};
	void release()
	{
		m_lock.release();
	}
	~ScopedLock()
	{
		release();
	}
};
