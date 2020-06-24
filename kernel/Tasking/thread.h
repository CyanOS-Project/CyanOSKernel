class Thread
{
  private:
	/* data */
  public:
	Thread(/* args */);
	~Thread();
	void block(int reason);
	void suspend(int reason);
	void resume(int reason);
	void get_process();
	void get_tid();
};
