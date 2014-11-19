class NoCopy {
public:
	NoCopy(){}
private:
	NoCopy(const NoCopy& copy){}
	NoCopy & operator=(const NoCopy& copy){}
};

#if 0
int main()
{
	NoCopy a;
	NoCopy b;
	b = a;
}
#endif 
