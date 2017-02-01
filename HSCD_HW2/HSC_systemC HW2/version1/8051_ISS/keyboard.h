SC_MODULE(keyboard)
{

	sc_in<sc_uint<3> >   row; 
    sc_in<sc_uint<5> >   column;
	sc_in<bool>  shift;
	sc_in<bool>  ctrl;
	sc_in<bool>  alt;
	sc_out<int>  numberofkey;
	sc_out<int>  specialkey;

  SC_CTOR(keyboard)
  {
	SC_METHOD(lookup);
	sensitive<<row<<column;
  }
	void lookup(); 
};

