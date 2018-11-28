struct complex{	
	float real;
	float im;
};

complex mul(complex c0, complex c1){
	complex c;
	c.real = c0.real * c1.real - c0.im * c1.im;
	c.im   = c0.real * c1.im + c0.im * c1.real;
	return c;
}

complex add(complex c0, complex c1)
{
	complex c;
	c.real = c0.real + c1.real;
	c.im   = c0.im   + c1.im;
	return c;
}

complex conj(complex c){
	complex c_conj = complex(c.real, -c.im);
	
	return c_conj;
}