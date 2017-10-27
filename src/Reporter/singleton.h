#ifndef	SINGLETON_H
#define SINGLETON_H

template <class T>
class CSingleton 
{
public:
	static T& get() 
	{
		static T _instance;
		return _instance;
	};
protected:
	CSingleton(void) {};
	virtual ~CSingleton(void) {};

	CSingleton(const CSingleton<T>&); //��ʵ��
	CSingleton<T>& operator= (const CSingleton<T> &); //��ʵ��
}; 

#endif
