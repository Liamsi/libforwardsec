#ifndef RELIC_API_H
#define RELIC_API_H
#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include <bitset> 
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/binary_object.hpp>
#include <boost/serialization/split_member.hpp>

// define classes
#ifdef __cplusplus
	// gmp.h uses __cplusplus to decide if it's right to include c++ headers.
	// At last on osx  causes error: conflicting types for 'operator<<'.
	// undefinning __cplusplus "FIXES" this.
	//#include <gmpxx.h>
		#define ___cplusplus __cplusplus
		#undef __cplusplus
	extern "C" {
#endif
   #include <relic/relic.h>
   #include <relic/relic_conf.h>

#ifdef ___cplusplus
}
	#define __cplusplus ___cplusplus
	#undef  ___cplusplus
#endif

extern "C" {
	   #include "common.h"
}

using namespace std;

#define convert_str(a)  a /* nothing */
void ro_error(void);

class CharmList;
class CharmListZR;

class ZR
{
    friend class boost::serialization::access;
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
		uint8_t data[BN_BYTES];
		memset(data, 0, BN_BYTES);
		bn_write_bin(data, BN_BYTES, z);
		ar &  boost::serialization::make_binary_object(data,BN_BYTES);
		ar & isInit;
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version){
		uint8_t data[BN_BYTES];
    	ar & boost::serialization::make_binary_object(data, BN_BYTES);
    	bn_read_bin(z,data,BN_BYTES);
		ar & isInit;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	bn_t z;
	bn_t order;
	bool isInit;
	ZR() 	 { bn_inits(z); bn_inits(order); g1_get_ord(order); isInit = true;bn_set_dig(z,1); }
	ZR(int);
	ZR(char*);
	ZR(bn_t y) { bn_inits(z); bn_inits(order); g1_get_ord(order); isInit = true; bn_copy(z, y); }
	ZR(const ZR& w) { bn_inits(z); bn_inits(order); bn_copy(z, w.z); bn_copy(order, w.order); isInit = true; }
	~ZR() { bn_free(z); bn_free(order); }
	ZR& operator=(const ZR& w)
	{
		if (isInit == true) { bn_copy(z, w.z); bn_copy(order, w.order); }
		else ro_error();
		return *this;
	}
	bool ismember();
	const ZR inverse();


	friend ZR hashToZR(string);
	friend ZR power(const ZR&, int);
	friend ZR power(const ZR&, const ZR&);
	friend ZR operator-(const ZR&);
	friend ZR operator-(const ZR&,const ZR&);
	friend ZR operator+(const ZR&,const ZR&);
	friend ZR operator*(const ZR&,const ZR&);
	friend ZR operator/(const ZR&,const ZR&);
    friend ZR operator&(const ZR&, const ZR&);  // bitwise-AND
//    friend ZR operator|(const ZR&, const ZR&);  // bitwise-OR
//    friend ZR operator^(const ZR&, const ZR&);  // bitwise-XOR
   friend ZR operator<<(const ZR&, int);
   friend ZR operator>>(const ZR&, int);

   friend ostream& operator<<(ostream&, const ZR&);
	friend bool operator==(const ZR& x,const ZR& y)
      {if(bn_cmp(x.z, y.z) == CMP_EQ) return true; else return false; }
	friend bool operator!=(const ZR& x,const ZR& y)
      {if (bn_cmp(x.z, y.z) != CMP_EQ) return true; else return false; }
};

class G1
{
public:
	g1_t g;
	bool isInit;
    G1()   { g1_inits(g); isInit = true; g1_set_infty(g); }
	G1(const G1& w) { g1_inits(g); g1_copy(g, w.g); isInit = true; }
	~G1()  { g1_free(g);  }

	G1& operator=(const G1& w)
	{
		if (isInit == true) g1_copy(g, w.g);
		else ro_error();
		return *this;
	}
	bool ismember(bn_t);
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
    	unsigned int l  = g1_size_bin(g,POINT_COMPRESS);
		uint8_t data[l];
		memset(data, 0, l);
		g1_write_bin(data, l, g,POINT_COMPRESS);
		ar & l;
		ar &  boost::serialization::make_binary_object(data,l);
		ar & isInit;
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version){
    	unsigned int l;
    	ar & l;
    	uint8_t data[l];
    	ar & boost::serialization::make_binary_object(data, l);
    	g1_read_bin(g,data,l);
		ar & isInit;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
    friend class boost::serialization::access;

	friend G1 hashToG1(string);
	friend G1 power(const G1&, const ZR&);
	friend G1 operator-(const G1&);
	friend G1 operator-(const G1&,const G1&);
	friend G1 operator+(const G1&,const G1&);
   friend ostream& operator<<(ostream&, const G1&);
	friend bool operator==(const G1& x,const G1& y)
      {return g1_cmp(x.g, y.g) == CMP_EQ; }
	friend bool operator!=(const G1& x,const G1& y)
      {return g1_cmp(x.g, y.g) != CMP_EQ; }
};

class G2
{
public:
	g2_t g;
	bool isInit;
    G2()   { g2_inits(g); isInit = true; g2_set_infty(g); }
	G2(const G2& w) { g2_inits(g); g2_copy(g, const_cast<G2&>(w).g); isInit = true; }
	~G2()  { g2_free(g);  }

	G2& operator=(const G2& w)
	{
		if (isInit == true) g2_copy(g,  const_cast<G2&>(w).g);
		else ro_error();
		return *this;
	}
	bool ismember(bn_t);

    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {

		G2 gg(*this);
    	unsigned int l  = g2_size_bin(gg.g,POINT_COMPRESS);
    	ar & l;
		uint8_t data[l];
		memset(data, 0, l);
		g2_write_bin(data, l,gg.g,POINT_COMPRESS);
		ar &  boost::serialization::make_binary_object(data,l);
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version){
		unsigned int l;
		ar & l;
		uint8_t data[l];
    	ar & boost::serialization::make_binary_object(data, l);
    	g2_read_bin(g,data,l);
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
    friend class boost::serialization::access;

	friend G2 hashToG2(string);
	friend G2 power(const G2&, const ZR&);
	friend G2 operator-(const G2&);
	friend G2 operator-(const G2&,const G2&);
	friend G2 operator+(const G2&,const G2&);
	friend ostream& operator<<(ostream& s, const G2&);
	friend bool operator==(const G2& x,const G2& y)
      {return g2_cmp( const_cast<G2&>(x).g,  const_cast<G2&>(y).g) == CMP_EQ;}
	friend bool operator!=(const G2& x,const G2& y)
      {return g2_cmp( const_cast<G2&>(x).g,  const_cast<G2&>(y).g) != CMP_EQ;}
};

class GT
{
public:
	gt_t g;
	bool isInit;
    GT()   { gt_inits(g); isInit = true; gt_set_unity(g); }
    GT(const GT& x) { gt_inits(g); isInit = true; gt_copy(g, const_cast<GT&>(x).g); }
    ~GT()  { gt_free(g); }

	GT& operator=(const GT& x)
	{
		if (isInit == true) gt_copy(g, const_cast<GT&>(x).g);
		else ro_error();
		return *this;
	}
	bool ismember(bn_t);
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
		uint8_t data[GT_LEN];
		memset(data, 0, GT_LEN);
		GT gg(*this);
		gt_write_bin(data, GT_LEN, gg.g,POINT_COMPRESS);
		ar &  boost::serialization::make_binary_object(data,GT_LEN);
		ar & isInit;
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version){
		uint8_t data[GT_LEN];
    	ar & boost::serialization::make_binary_object(data, GT_LEN);
    	gt_read_bin(g,data,GT_LEN);

		ar & isInit;
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()
    friend class boost::serialization::access;
	friend GT pairing(const G1&, const G1&);
	friend GT pairing(const G1&, const G2&);
	friend GT power(const GT&, const ZR&);
	friend GT operator-(const GT&);
	friend GT operator/(const GT&,const GT&);
	friend GT operator*(const GT&,const GT&);
	friend ostream& operator<<(ostream& s, const GT&);
	friend bool operator==(const GT& x,const GT& y)
      { return gt_cmp(const_cast<GT&>(x).g, const_cast<GT&>(y).g) == CMP_EQ;}
	friend bool operator!=(const GT& x,const GT& y)
      { return  gt_cmp(const_cast<GT&>(x).g, const_cast<GT&>(y).g) != CMP_EQ;}
};

class PairingGroup
{
public:
	PairingGroup();
	PairingGroup(int);
	// PairingGroup(int ptype,bool init,bn_t o){
	// 	pairingType = ptype;
	// 	isInit = init;
	// 	bn_copy(o,grp_order);
	// }

	~PairingGroup();
	void setCurve(int sec_level);

	void init(ZR&, char*);
	void init(ZR&, int);
	void init(G1&, int);
	void init(GT&, int);

	ZR init(ZR_type);
	ZR init(ZR_type, int);
	G1 init(G1_type);
	G1 init(G1_type, int);
	GT init(GT_type);
	GT init(GT_type, int);
	ZR random(ZR_type);
	G1 random(G1_type);
	GT random(GT_type);
	//template<typename T> T random12(){ T foo; return foo;};


//	bool ismember(CharmMetaListZR&);
//	bool ismember(CharmMetaListG1&);
//	bool ismember(CharmMetaListG2&);
//	bool ismember(CharmMetaListGT&);
//	bool ismember(CharmListStr&);
//	bool ismember(CharmListZR&);
//	bool ismember(CharmListG1&);
//	bool ismember(CharmListG2&);
//	bool ismember(CharmListGT&);
	bool ismember(ZR&);
	bool ismember(G1&);
	bool ismember(GT&);
	bool ismember(G2&);

	G2 init(G2_type);
	G2 init(G2_type, int);
	void init(G2&, int);
	G2 random(G2_type);
	G2 mul(G2, G2);
	G2 div(G2, G2);
	G2 exp(G2, ZR);
	G2 exp(G2, int);
	GT pair(G1, G2);
	GT pair(G2, G1);
	ZR order(); // returns the order of the group

	// hash -- not done
	ZR hashListToZR(string);
	G1 hashListToG1(string);
	G2 hashListToG2(string);

	GT pair(G1, G1);
	int mul(int, int);
	ZR mul(ZR, ZR);
	G1 mul(G1, G1);
	GT mul(GT, GT);
	int div(int, int);
	ZR div(int, ZR);
	ZR div(ZR, ZR);
	G1 div(G1, G1);
	GT div(GT, GT);

	ZR exp(ZR, int);
	ZR exp(ZR, ZR);
	G1 exp(G1, ZR);
	G1 exp(G1, int);
	GT exp(GT, ZR);
	GT exp(GT, int);

	ZR add(ZR, ZR);
	int add(int, int);

	int sub(int, int);
	ZR sub(ZR, ZR);
	ZR neg(ZR);
	ZR inv(ZR);
	G1 inv(G1);
	G2 inv(G2);
	GT inv(GT);
	string aes_key(GT & g);

private:
	int pairingType; // defined by above #defines SYMMETRIC or ASYMMETRIC (for now)
	bool isInit;
	bn_t grp_order;
};

//template<> 
//ZR PairingGroup::random12<ZR>(){return this->random(ZR_t);}
std::bitset<256> intToBits(ZR id);

#endif
