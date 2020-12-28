template <typename T> concept is_ForwardIterator = requires(T x, T y)
{
	x == y;
};

template <typename T> struct ForwardIterator {
	~ForwardIterator() {}
};

struct MyCustomIterator {
	bool operator==(const MyCustomIterator& other) const;
	static_assert(is_ForwardIterator<MyCustomIterator>, "It's not valid iterator");
};

int main(int argc, char** argv)
{
	return 0;
}
