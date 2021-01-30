#include "TypeTraits.h"
#include "UniquePointer.h"

template <typename ReturnType, typename... ParamsType> class Function;

template <typename ReturnType, typename... ParamsType> class Function<ReturnType(ParamsType...)>
{
  private:
	class CallableWrapperBase
	{
	  public:
		virtual ~CallableWrapperBase() = default;
		virtual ReturnType call(ParamsType&&... u) = 0;
	};

	template <typename CallableType> class CallableWrapper : public CallableWrapperBase
	{
	  private:
		CallableType m_callable_object;

	  public:
		~CallableWrapper() override = default;
		CallableWrapper(CallableType&& callable_object) : m_callable_object{move(callable_object)} {};
		CallableWrapper(const CallableType& callable_object) : m_callable_object{callable_object} {};
		ReturnType call(ParamsType&&... u) override { return m_callable_object(forward<ParamsType>(u)...); }
	};
	UniquePointer<CallableWrapperBase> m_function;

  public:
	template <typename CallableType>
	Function(CallableType function) :
	    m_function{UniquePointer<CallableWrapper<CallableType>>::make_unique(move(function))}
	{
	}
	ReturnType operator()(ParamsType&&... u) { return m_function->call(forward<ParamsType>(u)...); }
};
