#include "myMath.h"

MyMath::MyMath() :
	_int_min(-2147483647), _int_max(2147483647), _unsigned_int_max(std::numeric_limits<unsigned int>::max()), _divideBy_min_float((float)0.00001)
{
}
unsigned int MyMath::safeIncrement_uint_ref(unsigned int &num) {
	if ((num + 1) < num) {
		num = _unsigned_int_max;
	}
	else {
		++num;
	}

	return num;
}
unsigned int MyMath::safeDecrement_uint_ref(unsigned int &num) {
	if (num > 0) {
		--num;
	}
	else {
		num = 0;
	}

	return num;
}
void MyMath::safeAdd_intRef(int &num, int add) {
	if (add >= 0) {
		//RAISING
		if ((num + add) < num) {
			num = _int_max;
		}
		else {
			num += add;
		}
	}
	else {
		//LOWERING
		if ((num + add) > num) {
			num = _int_min;
		}
		else {
			num += add;
		}
	}
}
unsigned int MyMath::safeAdd_int(int num, int add) {
	if (add >= 0) {
		//RAISING
		if ((num + add) < num) {
			return _int_max;
		}
		else {
			return num + add;
		}
	}
	else {
		//LOWERING
		if ((num + add) > num) {
			return _int_min;
		}
		else {
			return num + add;
		}
	}
}
void MyMath::safeAdd_intRef(int &num, unsigned int add) {
	//RAISING
	if ((num + static_cast<int>(add)) < num) {
		num = _int_max;
	}
	else {
		num += static_cast<int>(add);
	}
}
unsigned int MyMath::safeAdd_int(int num, unsigned int add) {
	//RAISING
	if ((num + static_cast<int>(add)) < num) {
		return _int_max;
	}
	else {
		return num + static_cast<int>(add);
	}
}
void MyMath::safeSubtract_intRef(int &num, int subtract) {
	if (subtract < 0) {
		//RAISING
		if ((num - subtract) < num) {
			num = _int_max;
		}
		else {
			num -= subtract;
		}
	}
	else {
		//LOWERING
		if ((num - subtract) > num) {
			num = _int_min;
		}
		else {
			num -= subtract;
		}
	}
}
unsigned int MyMath::safeSubtract_int(int num, int subtract) {
	if (subtract < 0) {
		//RAISING
		if ((num - subtract) < num) {
			return _int_max;
		}
		else {
			return num - subtract;
		}
	}
	else {
		//LOWERING
		if ((num - subtract) > num) {
			return _int_min;
		}
		else {
			return num - subtract;
		}
	}
}
void MyMath::safeSubtract_intRef(int &num, unsigned int subtract) {
	//LOWERING
	if ((num - static_cast<int>(subtract)) > num) {
		num = _int_min;
	}
	else {
		num -= static_cast<int>(subtract);
	}
}
unsigned int MyMath::safeSubtract_int(int num, unsigned int subtract) {
	//LOWERING
	if ((num - static_cast<int>(subtract)) > num) {
		return _int_min;
	}
	else {
		return num - static_cast<int>(subtract);
	}
}
void MyMath::safeAdd_uintRef(unsigned int &num, unsigned int add) {
	if ((num + add) < num) {
		num = _unsigned_int_max;
	}
	else {
		num += add;
	}
}
unsigned int MyMath::safeAdd_uint(unsigned int num, unsigned int add) {
	if ((num + add) < num) {
		return _unsigned_int_max;
	}
	else {
		return num + add;
	}
}
void MyMath::safeAdd_uintRef(unsigned int &num, int add) {
	if (add > 0) {
		if ((num + add) < num) {
			num = _unsigned_int_max;
		}
		else {
			num += add;
		}
	}
	else {
		if ((num + add) > num) {
			num = 0;
		}
		else {
			num += add;
		}
	}
}
unsigned int MyMath::safeAdd_uint(unsigned int num, int add) {
	if (add > 0) {
		if ((num + add) < num) {
			return _unsigned_int_max;
		}
		else {
			return num + add;
		}
	}
	//ADDING NEGATIVES
	else {
		if ((num + add) > num) {
			return 0;
		}
		else {
			return num + add;
		}
	}
}
void MyMath::safeSubtract_uintRef(unsigned int &num, unsigned int subtract) {
	if ((num - subtract) > num) {
		num = 0;
	}
	else {
		num -= subtract;
	}
}
unsigned int MyMath::safeSubtract_uint(unsigned int num, unsigned int subtract) {
	if ((num - subtract) > num) {
		return 0;
	}
	else {
		return num - subtract;
	}
}
void MyMath::safeSubtract_uintRef(unsigned int &num, int subtract) {
	if (subtract > 0) {
		if ((num - subtract) > num) {
			num = 0;
		}
		else {
			num -= subtract;
		}
	}
	//SUBTRACTING NEGATIVES
	else {
		if ((num - subtract) < num) {
			num = _unsigned_int_max;
		}
		else {
			num -= subtract;
		}
	}
}
unsigned int MyMath::safeSubtract_uint(unsigned int num, int subtract) {
	if (subtract > 0) {
		if ((num - subtract) > num) {
			return 0;
		}
		else {
			return num - subtract;
		}
	}
	//SUBTRACTING NEGATIVES
	else {
		if ((num - subtract) < num) {
			return _unsigned_int_max;
		}
		else {
			return num - subtract;
		}
	}
}
unsigned int MyMath::safeLimit_uint(unsigned int num, unsigned int max) {
	return (num > max) ? max : num;
}
unsigned int MyMath::difference(int n1, int n2) {

	return std::abs((n1 - n2));
}
unsigned int MyMath::difference(int n1, unsigned int n2) {

	return std::abs((n1 - static_cast<int>(n2)));
}
unsigned int MyMath::difference(unsigned int n1, int n2) {

	return std::abs((static_cast<int>(n1) - n2));
}
unsigned int MyMath::difference(unsigned int n1, unsigned int n2) {

	return std::abs((static_cast<int>(n1) - static_cast<int>(n2)));
}
void MyMath::normalize_withinRange_intRef(int &num, int min, int max) {
	if (num < min) {
		num = min;
	}
	else if (num > max) {
		num = max;
	}
}
void MyMath::add_withinRange_intRef(int &num, int add, int min, int max) {
	num += add;
	normalize_withinRange_intRef(num, min, max);
}
void MyMath::subtract_withinRange_intRef(int &num, int subtract, int min, int max) {
	num -= subtract;
	normalize_withinRange_intRef(num, min, max);
}
void MyMath::set_withinRange_intRef(int &num, int to, int min, int max) {
	num = to;
	normalize_withinRange_intRef(num, min, max);
}
unsigned int MyMath::safeMultiply_uint(unsigned int num, float times) {
	if (times < 0) {
		return 0;
	}
	return static_cast<unsigned int>(static_cast<float>(num) * times);
}
unsigned int MyMath::safeMultiply_uint(unsigned int num, unsigned int times) {
	return num * times;
}
float MyMath::safeDivide_float(float num, float divideBy) {
	if (divideBy <= _divideBy_min_float) {
		return num / _divideBy_min_float;
	}
	else {
		return num / divideBy;
	}
}
void MyMath::safeDivide_floatRef(float &num, float divideBy) {
	if (divideBy <= _divideBy_min_float) {
		num /= _divideBy_min_float;
	}
	else {
		num /= divideBy;
	}
}
float MyMath::safeDivide_float(unsigned int num, unsigned int divideBy) {
	if (divideBy <= _divideBy_min_float) {
		return num / _divideBy_min_float;
	}
	else {
		return static_cast<float>(num) / static_cast<float>(divideBy);
	}
}
unsigned int MyMath::safeDivide_uint(unsigned int num, unsigned int divideBy) {
	if (divideBy == 0) {
		return 0;
	}
	else {
		return num / divideBy;
	}
}
void MyMath::safeDivide_uintRef(unsigned int &num, unsigned int divideBy) {
	if (divideBy == 0) {
		num = 0;
	}
	else {
		num /= divideBy;
	}
}
int MyMath::safeDivide_int(unsigned int num, unsigned int divideBy) {
	if (divideBy == 0) {
		return 0;
	}
	else {
		return (int)num / (int)divideBy;
	}
}
unsigned int MyMath::safeRemainder_uint(unsigned int num, unsigned int divideBy) {
	if (divideBy == 0) {
		return 0;
	}
	else {
		return num % divideBy;
	}
}
double MyMath::parabolaFindA(double x1, double y1, double x2, double y2, double x3, double y3) {
	//this function finds the variable a in the standard quadratic equation for a parabola
	double a = 0;
	//A = [(Y2-Y1)(X1-X3) + (Y3-Y1)(X2-X1)]/[(X1-X3)(X2^2-X1^2) + (X2-X1)(X3^2-X1^2)]
	a = (((y2 - y1) * (x1 - x3)) + ((y3 - y1) * (x2 - x1))) / (((x1 - x3) * (pow(x2, 2) - pow(x1, 2))) + ((x2 - x1) * (pow(x3, 2) - pow(x1, 2))));
	//cout << "A: " << a << endl;
	return a;
}
double MyMath::parabolaFindB(double a, double x1, double y1, double x2, double y2) {
	//this finds b in the standard form once a is found
	//B = [(Y2 - Y1) - A(X2^2 - X1^2)] / (X2-X1)
	double b = 0;
	b = (((y2 - y1) - (a * (pow(x2, 2) - pow(x1, 2)))) / (x2 - x1));
	//cout << "B: " << b << endl;
	return b;
}
double MyMath::parabolaFindC(double a, double b, double x1, double y1) {
	//this finds c once a and b are found
	//C = Y1 - AX1^2 - BX1
	double c = 0;
	c = y1 - (a * pow(x1, 2)) - (b * x1);
	//cout << "C: " << c << endl;
	return c;
}
double MyMath::parabolaFindY(double a, double b, double c, double x) {
	//this function takes all three (a,b,&c) coefficients, and plugs the x-half of a point along that parabola to find the corresponding y-half
	//y = ax^2 + bx + c
	double y = (a * pow(x, 2)) + (b * x) + c;
	//cout << "Y: " << y << endl;
	return y;
}

double MyMath::parabolaFindYFromX(double x1, double y1, double x2, double y2, double x3, double y3, double findX) {
	//this mathematical solution takes 3 coordinates, then calculates the a, b, and c coefficients for the standard formula for parabolas .. in quadratic I guess
	//plug in 3 coordinates that are known to be on the parabola.  Then plug in an x coordinate along that parabola for which you would like to find the cooresponding y
	double a = parabolaFindA(x1, y1, x2, y2, x3, y3);
	double b = parabolaFindB(a, x1, y1, x2, y2);
	double c = parabolaFindC(a, b, x1, y1);
	double y = parabolaFindY(a, b, c, findX);
	//cout << "Found new Y: " << y << endl;
	return y;
}