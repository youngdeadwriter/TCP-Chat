#ifndef CHAT_LOG_H
#define CHAT_LOG_H

//FORWARD DECLARATIONS

//INCLUDE DEPENDENCIES
#include <math.h>
#include <limits>

class MyMath {
private:
	int _int_min;
	int _int_max;
	unsigned int _unsigned_int_max;
	float _divideBy_min_float;
public:
	unsigned int safeIncrement_uint_ref(unsigned int &num);
	unsigned int safeDecrement_uint_ref(unsigned int &num);

	void safeAdd_intRef(int &num, int add);
	unsigned int safeAdd_int(int num, int add);
	void safeAdd_intRef(int &num, unsigned int add);
	unsigned int safeAdd_int(int num, unsigned int add);

	void safeSubtract_intRef(int &num, int subtract);
	unsigned int safeSubtract_int(int num, int subtract);
	void safeSubtract_intRef(int &num, unsigned int subtract);
	unsigned int safeSubtract_int(int num, unsigned int subtract);

	void safeAdd_uintRef(unsigned int &num, unsigned int add);
	unsigned int safeAdd_uint(unsigned int num, unsigned int add);
	void safeAdd_uintRef(unsigned int &num, int add);
	unsigned int safeAdd_uint(unsigned int num, int add);

	void safeSubtract_uintRef(unsigned int &num, unsigned int subtract);
	unsigned int safeSubtract_uint(unsigned int num, unsigned int subtract);
	void safeSubtract_uintRef(unsigned int &num, int subtract);
	unsigned int safeSubtract_uint(unsigned int num, int subtract);

	unsigned int difference(int n1, int n2);
	unsigned int difference(int n1, unsigned int n2);
	unsigned int difference(unsigned int n1, int n2);
	unsigned int difference(unsigned int n1, unsigned int n2);

	void normalize_withinRange_intRef(int &num, int min, int max);
	void add_withinRange_intRef(int &num, int add, int min, int max);
	void subtract_withinRange_intRef(int &num, int subtract, int min, int max);
	void set_withinRange_intRef(int &num, int to, int min, int max);

	unsigned int safeMultiply_uint(unsigned int num, float times);
	unsigned int safeMultiply_uint(unsigned int num, unsigned int times);

	float safeDivide_float(float num, float divideBy);
	void safeDivide_floatRef(float &num, float divideBy);

	float safeDivide_float(unsigned int num, unsigned int divideBy);

	unsigned int safeDivide_uint(unsigned int num, unsigned int divideBy);
	void safeDivide_uintRef(unsigned int &num, unsigned int divideBy);

	int safeDivide_int(unsigned int num, unsigned int divideBy);

	unsigned int safeRemainder_uint(unsigned int num, unsigned int divideBy);

	unsigned int safeLimit_uint(unsigned int num, unsigned int max);


	double parabolaFindA(double x1, double y1, double x2, double y2, double x3, double y3);
	double parabolaFindB(double a, double x1, double y1, double x2, double y2);
	double parabolaFindC(double a, double b, double x1, double y1);
	double parabolaFindY(double a, double b, double c, double x);
	double parabolaFindYFromX(double x1, double y1, double x2, double y2, double x3, double y3, double findX);


	MyMath();
};



#endif 

