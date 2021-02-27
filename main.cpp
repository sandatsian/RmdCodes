//#include "stdafx.h"

#include <time.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <bitset>
#include <vector>
#include <map>
#include <math.h>
#include <fstream>
#include <sys/timeb.h>
#include <windows.h>
#include <fstream>
#include <functional>
#include "image.h"

using namespace std;
#pragma optimize( "", off )

typedef unsigned char uchar;
typedef unsigned int uint;
typedef vector<int> vi;
typedef map<int, string> um;

//============== i235 ===================
struct tabsi235 {
	int p1,p2,p3,p4;
	struct tabsi235* al;
};

struct tabsi2351 {
	int p1,p2,p3,p4,n;
	struct tabsi2351* al;
};

unsigned int experT[256][256];
struct tabsi235 T235[1000000];
struct tabsi2351 T235_1[1000000];
const int Lmax=30,max_i235=30000;

int nlk[Lmax + 8][40], clen, ilen, s1, s2, r1, r2, r3, r4;
int nl[Lmax];

struct ai_state {
	int st,L,p,res;
};

//============= rpbc ====================
const int R = 256, R2 = 256 * 256, R3 = 256 * 256 * 256;
int suffix[500000],first[500000];
int v1,v2,v3,v4;
//=======================================

LARGE_INTEGER tstart, tend, tfreq;
const int N=610000,L=30000000,globL=3600000;
unsigned int rank_after0=0,ranks[L],ranks_scdc[L],_0ranks[N],S=145,C=256-S,base[100],i235[300000],sumd,sum1,rand_test[1000000];
unsigned char codes[L*4],codes_d235[L*4],codes_scdc[L*4];
uint decoded[1],decoded_fib[1],decoded_scdc[1],rz_esi,rz_esp,decoded_c[1];
string *out235[20000000],*out_i235[20000000],Dict_i235[1000000],*outSCDC[20000000];
string out_i235s[20000000];
float sumi,sums;

char* glob_res;
//dictionary
string Dict[N*2],DictSCDC[N*2];
map<string,int> dic_map,i235_map,i235_map_sorted,d235_map_sorted;
map<string,int> dic_map_scdc;
map<int,string> dic_map1;
map<int,pair<int,int> > d235_map;
int dic_size,dic_size_scdc;
//text
int Nwords,diff_words;
string out_text,out_text_scdc;

double points[N+1],maxpoi,entropy;
int t=sizeof(unsigned int)*8,cur_byte(0),cur_bit(7);
bool w3_output;

const int sw=256,sr=10;
uint f[sr][sw],w1[sr][sw],w2[sr][sw],w3[sr][sw],w4[sr][sw],r[sr][sw],s[sr][sw];
struct tabs {
	uint f,w1,w2,w3,w4,r,s;
};

struct tabs* TARR[10][256];

uint TAB[10*256];

struct FibT {
	uchar pl,pv,s,sl,sv,p;
};

struct FibT Ftab[255][3];

uint Fib3[150][28],Start[32];


uint decode_d235_base() {
uint out=0x80000000,c_bit_in,c_byte_in=0,c_bit_out=30,in_mask=128,x,ones;
	cur_byte=0;
	x=codes[0];
	while(cur_byte<clen) {
		while(!(x&in_mask)) { //read sequence of 0s
			c_bit_out--;
			if(in_mask>1)
				in_mask>>=1;
			else {
				x=codes_d235[++c_byte_in];
				in_mask=128;
			}
		}
		ones=0;
		while(x&in_mask) { //read sequence of 1s
			ones++;
			c_bit_out--;
			if(in_mask>1)
				in_mask>>=1;
			else {
				x=codes_d235[++c_byte_in];
				in_mask=128;
			}
		}
		switch(ones) { //write sequence of ones
			case 2: out>>=c_bit_out;
					out235[cur_byte++]=Dict+out;
					break;
			case 1:	out|=(1<<c_bit_out);
					c_bit_out--;
					break;
			case 4: out|=(3<<(c_bit_out-1));
					c_bit_out-=2;
					break;
			case 3: out|=(7<<(c_bit_out-2));
					out>>=(c_bit_out-3);
					out235[cur_byte++]=Dict+out;
					break;
			case 5: out|=(31<<(c_bit_out-4));
					out>>=(c_bit_out-5);
					out235[cur_byte++]=Dict+out;
					break;
			default:out|=((1<<(ones-3))-1)<<(c_bit_out-(ones-3));
					c_bit_out-=ones-4;
		}
	}
return out;
}

void decodeFib() {
	struct FibT x={3,71,11,4,121,19};
uint j=0,index,i=0;
uchar oldSL=0,oldSV=0,p=0;
	oldSL=0,oldSV=0,p=0;
	j=0;
	while(i<cur_byte) {
		x=Ftab[codes[i]][p];
		i++;
		p=x.p;
		if(!(i&3)) {
			oldSV=oldSV+Fib3[x.pv][oldSL];
			oldSL=oldSL+x.pl;
		} else {
			decoded[j]=oldSV+Fib3[x.pv][oldSL]+Start[3+oldSL+x.pl];
			j++;
			if((i&3)==3) {
				decoded[j]=x.s;
				j++;
			}
			oldSV=x.sv;
			oldSL=x.sl;
		}
	}
}

class bn{
public:
	uint x;
	int last_bit;
	bn(char*);
	bn(uint a):x(a),last_bit(8){}; // right-shifted byte value constructor
	bn():x(0),last_bit(0){}; //right-shifted value constructor
	void set(uint a) {x=a;last_bit=8;};
	void set1(){x=1;last_bit=1;};
	void init(){x=0;last_bit=0;};
	void add(char*,int);
	void prepend(char*);
	void print();
	void form_nevr(bn);
	uint toint();
	uint get_len();
	uint transform_to_nevr_value();
};

uint bn::toint() {
	return x;
}

uint bn::get_len() {
	return last_bit+1;
}

int get_size(unsigned int x) {
int s=0;
	while(x>0) {
		x>>=1;
		s++;
	}
	return s;
}

void insert_1(unsigned int& x,int pos) {
unsigned int mask=0,mask1=0;
	for(int i=0;i<pos-1;i++)
		mask|=(1<<i);
	for(int i=pos-1;i<t;i++)
		mask1|=(1<<i);
	x=(mask1&x)|((mask&x)>>1)|(1<<(pos-2));
}

int d235_length(uint x) {
uint k;
	for(unsigned int j=1<<(t-1),p=t-1;j;j>>=1,p--)
		if(x&j)
			k=p;
	return t-k+1;
}

//==================== Encode ===================
//obtain code shifted to left boundary of 32-bit word
unsigned int get_code(unsigned int x) {
int s=get_size(x),k_od=0,j;
uint a,b;
bool f=false;
	x<<=(t-s);
	x-=(1<<31);
	x<<=1;
	if(!x)
		x|=(6<<(t-s-2));
	else {
		if((x>>(t-s+1)==0xe) || (x>>(t-s+1)==0x3e))
			f=true;
		if(!f) {
			int last_bit=t+1-s;
			for(j=t;j>=last_bit;j--)
				if(x&(1<<(j-1)))
					k_od++;
				else {
					if(k_od>=2 && !(j==last_bit+1 &&(k_od==3 || k_od==5))) {
						insert_1(x,j+1);
						insert_1(x,j+1);
						last_bit-=2;
						j-=2;
					}
					if(k_od>=3 && !(j==last_bit+1 &&(k_od==3 || k_od==5))) {
						insert_1(x,j+1);
						last_bit--;
						j--;
					}
					if(!(j==last_bit+1 &&(k_od==3 || k_od==5)))
						k_od=0;
					else
						j-=2;
				}
				if(k_od!=3 && k_od!=5) {
					if(k_od==2) {
						insert_1(x,j+1);
						last_bit--;
					}
					if(last_bit>=4)
						x|=(6<<(last_bit-4));
					else {
						x&=0xfffffff6;
						x|=6;
					}
				}
		}
	}
	return x;
}

void flush_to_byte(unsigned int x) {
int k(0),p;
//find the rightmost 1. Codeword ends one bit right to it
	for(unsigned int j=1<<(t-1),p=t-1;j;j>>=1,p--)
		if(x&j)
			k=p;
	for(unsigned int j=1<<(t-1);j>(1<<(k-2));j>>=1) {
		if(j&x)
			codes_d235[cur_byte]|=(1<<cur_bit);
		cur_bit=cur_bit==0?7:cur_bit-1;
		if(cur_bit==7)
			cur_byte++;
	}
	if(cur_byte>162954)
		cur_byte=cur_byte;
}

void flush_to_byte_i235(unsigned int x) {
int k(0),p;
unsigned int j;
//find the leftmost 1. Codeword starts 1 bit left of it
	for(j=1<<(t-1),p=t-1;!(x&j) && j;j>>=1,p--);
	for(j<<=1;j>0;j>>=1) {
		if(j&x)
			codes[cur_byte]|=(1<<cur_bit);
		cur_bit=cur_bit==0?7:cur_bit-1;
		if(cur_bit==7)
			cur_byte++;
	}
}

uint invert_d235(uint x) {
uint l,i;
uint res=0;
	for(i=1<<(t-1),l=t-1;i>0;i>>=1,l--)
		if(i&x)
			break;
	for(uint i=(1<<l),j=1;i>0;i>>=1,j<<=1)
		if(i&x)
			res|=j;
	return(res);
}

uint inverted_d235[150000];

void make_inverted_d235() {

}

void encodeC235(int max_rank,uint* ranks) {
int i;
	cur_byte=0; cur_bit=7;
	for(i=0;i<max_rank;i++)
		flush_to_byte(get_code(ranks[i]));
	clen=cur_byte;
}

void encodeI235(int max_rank,uint* ranks) {
int i;
	cur_byte=0; cur_bit=7;
	for(i=0;i<max_rank;i++)
		/*if(i>=1 && ranks[i-1]==0)
			if(ranks[i]==rank_after0)
				flush_to_byte_i235(i235[0]);
			else
				if(ranks[i]==0)
					flush_to_byte_i235(i235[rank_after0]);
				else
					flush_to_byte_i235(i235[ranks[i]]);
			flush_to_byte_i235(i235[(ranks[i]-rank_after0)%diff_words]);
		else*/
			flush_to_byte_i235(i235[ranks[i]]);
	ilen=cur_byte;
}
//========================================================

//prepends str to the left of right-shifted bn
void bn::prepend(char* str) {
int k=strlen(str);
	for(int j=k-1;j>=0;j--) {
		if(str[j]=='1')
			x|=(1<<last_bit);
		last_bit++;
	}
}


//appends str to the right of right-shifted bn s times
void bn::add(char* str, int s) {
int k=strlen(str);
	for(int j=0;j<s;j++) {
		x<<=k;
		last_bit+=k;
		for(int i=0;i<k;i++)
			if(str[i]=='1')
				x|=(1<<(k-i-1));
	}
}

int cmp_bit(bn x,char u) {
	if(x.x&(1<<(x.last_bit-1)))
		return u=='1'?1:0;
	else
		return u=='0'?1:0;
}

//cmp part of string from current position
int cmp_(bn x,char* str) {
int k=strlen(str),i;
	if(k>1)
		if(str[1]=='s') {
			for(i=0;cmp_bit(x,str[0]) && x.last_bit>0;i++,x.last_bit--);
			return i;
		};
	for(i=0;i<k && x.last_bit>0;i++,x.last_bit--)
		if(!cmp_bit(x,str[i]))
			return 0;
	return i==k;
}

bool cmp_rest(bn x,char* str) {
int k=strlen(str),i;
	if(str[k-1]=='s')
		k--;
	for(i=0;i<k && x.last_bit>0;i++,x.last_bit--)
		if(!cmp_bit(x,str[i]))
			return 0;
	if(str[k]=='s')
		while(x.last_bit>0 && cmp_bit(x,str[k-1]))
			x.last_bit--;
	return x.last_bit==0 && i==k;
}

//return: 2 - end of byte; 1 - end with vnevr; 0 - end of codeword
int fw(bn& x,bn& res,bool fin_flag) {
	if(x.last_bit<=0)
		return 2;
	if(cmp_rest(x,"1") || cmp_rest(x,"11") || (!fin_flag && (cmp_rest(x,"0") || cmp_rest(x,"01") || cmp_rest(x,"011"))))
		return 1;
	else {
		if(cmp_(x,"110")) {
			x.last_bit-=3;
			return 0;
		} else
		if(cmp_(x,"0110")) {
			x.last_bit-=4;
			return 0;
		} else
		if(cmp_(x,"1110")){
			res.add("1110",1);
			x.last_bit-=4;
			return 0;
		} else
		if(cmp_(x,"01110")){
			res.add("01110",1);
			x.last_bit-=5;
			return 0;
		} else
		if(cmp_(x,"111110")){
			res.add("111110",1);
			x.last_bit-=6;
			return 0;
		} else
		if(cmp_(x,"0111110")){
			res.add("0111110",1);
			x.last_bit-=7;
			return 0;
		} else {
			int s=cmp_(x,"1s");
			if(s==3) {
				res.add("1",s-1);
				x.last_bit-=s;
			} else
			if(s>3 && s<6) {
				res.add("1",s-2);
				x.last_bit-=s;
			} else
			if(s>=6) {
				res.add("1",s-3);
				x.last_bit-=s;
			} else
			if(cmp_(x,"10")) {
				res.add("1",1);
				x.last_bit--;
			} else
			if(cmp_(x,"0")) {
				res.add("0",1);
				x.last_bit--;
			} else
				return 1;
		}
		return fw(x,res,false);
	}
}


// 0 - stopped with vnevr
// -1 - simply finish previous w, don't append anything
// -2 - codeword succesfully extracted
int get_w1(uint nevr,bn& x,bn& y) { // x - input byte, y - output w1
int s;
	switch(nevr) {
		case 0: if(x.last_bit>0) {
					if(cmp_(x,"110")) { //starting new codeword
						x.last_bit-=3;
						return -2;
					}
					if(cmp_(x,"0110")) {
						x.last_bit-=4;
						y.add("0",1);
						return -2;
					}
					s=cmp_(x,"0s");
					y.add("0",s);
					x.last_bit-=s;
				} else
					return 0;
				break;
		case 1: if(cmp_(x,"110")) { // undecoded sequence contains one '0' bit
					x.last_bit-=3;
					return -1;
				}
				s=cmp_(x,"0s");
				y.add("0",s);
				x.last_bit-=(s-1);
				break;
		case 2: if(cmp_(x,"10")) { //w ends with one '1'
					x.last_bit-=2;
					return -1;
				}
				x.prepend("1");
				break;
		case 3: if(cmp_(x,"10")) { //w ends with '01', where '0' is undecoded
					x.last_bit-=2;
					return -1;
				}
				x.prepend("01");
				break;
		case 4: if(cmp_(x,"0")) { //w ends with '11'
					x.last_bit--;
					return -1;
				}
				x.prepend("11");
				break;
		case 5: if(cmp_(x,"0")) { //w ends with '011', where '0' is undecoded
					x.last_bit--;
					return -1;
				}
				x.prepend("011");
				break;
		case 6: s=cmp_(x,"1s"); //w ends with '111', where '11' are decoded
				if(s==0) {
					x.last_bit--;
					y.prepend("10");
					return -2;
				}
				if(s==2) {
					x.last_bit-=3;
					y.prepend("1110");
					return -2;
				}
				if(s==1)
					x.last_bit--;
				else {
					y.add("1",s-2);
					x.last_bit-=s;
				}
				break;
		case 7: s=cmp_(x,"1s"); //w ends with '1111', where '11' are decoded
				if(s==1) {
					x.last_bit-=2;
					y.prepend("1110");
					return -2;
				}
				if(s==0) {
					if(cmp_(x,"0110")) {
						x.last_bit-=4;
						return -2;
					}
					y.prepend("0");
					x.last_bit--;
				} else {
					y.add("1",s-1);
					x.last_bit-=s;
				}
				break;
		case 8:	s=cmp_(x,"1s"); //w ends with '11111', where '111' are decoded
				if(s==0) {
					x.last_bit--;
					y.prepend("110");
					return -2;
				}
				y.add("1",s-1);
				x.last_bit-=s;
				break;
		case 9: s=cmp_(x,"1s"); //w ends with s>=6 decoded ones
				y.add("1",s);
				x.last_bit-=s;
				break;
			}
	s=fw(x,y,false);
	if(!s) {
		return -2;
	} else
		return 0;
}

void bn::print() {
	for(int i=last_bit-1;i>=0;i--)
		printf("%d",x&(1<<i)?1:0);
	printf(" ");
}

void bn::form_nevr(bn a) {
	last_bit=a.last_bit;
	for(int i=7;i>=0;i--)
		x|=(a.x&(1<<i));
}

uint bn::transform_to_nevr_value() {
uint t,k;
	for(t=1,k=0;x&t;t<<=1,k++);
	if(k<3)
		if(last_bit<=k)
			return 2*k;
		else
			return 2*k+1;
	if(k>6)
		return 9;
	return k+3;
}

void print_bin_char(uchar c) {
	for(uchar i=128;i;i>>=1)
		printf("%d",c&i?1:0);
	printf(" ");
}

int write_table0(bn w1,bn nv) {
uint x=(1<<31);              //f1=1
	x|=(w1.x<<(16-w1.last_bit));
	x|=w1.last_bit;
	x|=(nv.transform_to_nevr_value()<<24);
	return x;
}

int write_table1(bn w1,bn w2,bn nv) {
uint x=(1<<30);
	x|=(w1.x<<(16-w1.last_bit));
	x|=16-w1.last_bit;
	x|=(w2.x<<16);
/*	if(w2.last_bit==0)
		x|=(1<<16);
	else
		x|=(1<<(16+w2.last_bit));*/
	x|=(nv.transform_to_nevr_value()<<24);
	return x;
}

int write_table2(bn w1,bn w2,bn w3,bn nv) {
uint x=0;
	x|=(w1.x<<(16-w1.last_bit));
	x|=16-w1.last_bit;
	x|=(w2.x<<16);
//	x|=(1<<(16+w2.last_bit));
//	x|=(1<<(24+w3.last_bit));
	if(!w3_output) {
		x|=(w3.x<<24);
		x|=(1<<29);
		x|=(nv.transform_to_nevr_value()<<5);
	} else {
		x|=(w3.x<<22);
		if(nv.last_bit>0)  // if w4 exists
			if(nv.x&1) {
				x|=(1<<27);
				x|=(1<<6);
			} else
				x|=(1<<28);
		else
			x|=(1<<27);	// start empty w4 for next step
	}
	return x;
}

int write_table01(bn w1,bn nv,struct tabs* v) {
uint x=(1<<31);              //f1=1
	v->f=0;
	x|=(w1.x<<(16-w1.last_bit));
	x|=w1.last_bit;
	v->s=w1.get_len()-1;
	x|=(nv.transform_to_nevr_value()<<24);
	v->r=nv.transform_to_nevr_value();
	return x;
}

int write_table11(bn w1,bn w2,bn nv,struct tabs* v) {
uint x=(1<<30);
	v->f=1;
	x|=(w1.x<<(16-w1.last_bit));
	x|=16-w1.last_bit;
	v->s=w1.get_len()-1;
	x|=(w2.x<<16);
	x|=(nv.transform_to_nevr_value()<<24);
	v->r=nv.transform_to_nevr_value();
	return x;
}

int write_table21(bn w1,bn w2,bn w3,bn nv,struct tabs* v) {
uint x=0;
	x|=(w1.x<<(16-w1.last_bit));
	x|=16-w1.last_bit;
	x|=(w2.x<<16);
	v->s=w1.get_len()-1;
	if(!w3_output) {
		x|=(w3.x<<24);
		v->w3=w3.x;
		x|=(1<<29);
		v->f=2;
		x|=(nv.transform_to_nevr_value()<<5);
		v->r=nv.transform_to_nevr_value();
	} else {
		v->f=3;
		v->r=0;
		x|=(w3.x<<22);
		if(nv.last_bit>0)  // if w4 exists
			if(nv.x&1) {
				x|=(1<<27);
				x|=(1<<6);
				v->w4=1;
			} else {
				x|=(1<<28);
				v->w4=2;
			}
		else {
			x|=(1<<27);	// start empty w4 for next step
			v->w4=1;
		}
	}
	return x;
}



void initTAB1() {
uint i,j;
int fin,mv3=0;
bn x,w1,w2,w3,nv;
	for(i=0;i<10;i++) {
		for(j=0;j<256;j++) {
			//i=0;j=0xbd;
			TARR[i][j]=new struct tabs;
			TARR[i][j]->s=0;
			TARR[i][j]->r=0;
			x.set(j);
			w1.init();
			nv.init();
			fin=get_w1(i,x,w1);
			TARR[i][j]->w1=w1.toint();
			/*if(i==0)
				TARR[i][j]->w1|=(1<<w1.last_bit);*/
			if(fin) {
				w2.init();
				w2.add("1",1);
				fin=get_w1(0,x,w2);
				TARR[i][j]->w2=w2.toint();
				if(fin) {
					w3.init();
					w3.add("1",1);
					fin=get_w1(0,x,w3);
					TARR[i][j]->w3=w3.toint();
					w3_output=fin==-2;
					nv.form_nevr(x);
					write_table21(w1,w2,w3,nv,TARR[i][j]);
				} else {
					nv.form_nevr(x);
					write_table11(w1,w2,nv,TARR[i][j]);
				}
			} else {
				nv.form_nevr(x);
				write_table01(w1,nv,TARR[i][j]);
			}
			if(TARR[i][j]->f>2)
				i=i;
		}
		j=j;
}
}

void initTAB() {
uint i,j;
int fin,mv3=0;
bn x,w1,w2,w3,nv;
	for(i=0;i<10;i++) {
		for(j=0;j<256;j++) {
	//		i=4;j=0x6d;
			x.set(j);
			w1.init();
			nv.init();
			fin=get_w1(i,x,w1);
		//	printf("j=%d w1=%x l_bit=%d\n",j,w1.x,w1.last_bit);
			/*printf("nevr=%d ",i);
			print_bin_char(j);
			w1.print();
			printf("%d %d ",w1.last_bit,fin?1:0);*/
			if(fin) {
				w2.init();
				w2.add("1",1);
				fin=get_w1(0,x,w2);
				/*printf("\tw2: ");
				w2.print();
				printf("%d %d ",w2.last_bit,fin?1:0);*/
				if(fin) {
					w3.init();
					w3.add("1",1);
					fin=get_w1(0,x,w3);
					w3_output=fin==-2;
					/*printf("\tw3: ");
					w3.print();
					printf("%d %d ",w3.last_bit,fin?1:0);*/
					nv.form_nevr(x);
					TAB[i*256+j]=write_table2(w1,w2,w3,nv);
				} else {
					nv.form_nevr(x);
					TAB[i*256+j]=write_table1(w1,w2,nv);
				}
			} else {
				nv.form_nevr(x);
				TAB[i*256+j]=write_table0(w1,nv);
			}
			/*printf("\tnevr: ");
			nv.print();
			printf(" %d",nv.transform_to_nevr_value());
			printf("\n");*/
		}
		//getch();
}
/*	printf("mv=%d",mv3);
	getch();*/
}

const int block_size = 8;
const int M = 148;
const int wcount = 5*1e6;
const int numShifts = 20;
int cur_dec;

vector<int> fib3, start;
um dictionary;

struct entry {
    short PL, PV, SL, SV, p;
    vector<int> S;
};

int getIndex(int n, int l) {
    int ans = start[l];
    for (int i = l - 1; i >= 0; i--)
        if (n & 1 << i)
            ans += fib3[l - 1 - i];
    return ans;
}

int f2(int n,int l) {
    if (l < 0)
        return M - l;
    return getIndex(n, l) - start[l];
}

entry** build_partial_decoding_table() {
    entry **Table = new entry*[3];
    for (int p = 0; p < 3; p++) {
        Table[p] = new entry[1 << block_size];
        for (int i = 0; i < 1 << block_size; i++) {
            entry *Element = &Table[p][i];
            Element->PL = block_size; Element->SL = 0;
            Element->p = 0;
            int pos = 0, prefix = 0, suffix = 0;
            for (int j = block_size - 1; j >= 2; j--) {
                int match = (((1 << (j + 1)) - 1) & i) >> (j - 2);
                int match2 = i >> (block_size - 3 + p);
                if (Element->PL == block_size) {
                    if (match2 + 1 == 1 << (3 - p)) {
                        prefix = i >> (j + 1);
                        Element->PL = -p;
                        Element->PV = f2(prefix, Element->PL);
                        suffix = ((1 << (j - 2 + p)) - 1) & i;
                        Element->SL = j - 2 + p;
                        Element->SV = f2(suffix, Element->SL);
                        j -= (2 - p);
                        pos = j - 1;
                    }
                    else if (match == 7) {
                        prefix = i >> (j + 1);
                        Element->PL = block_size - j - 1;
                        Element->PV = f2(prefix, Element->PL);
                        suffix = ((1 << (j - 2)) - 1) & i;
                        Element->SL = j - 2;
                        Element->SV = f2(suffix, Element->SL);
                        j -= 2;
                        pos = j - 1;
                    }
                }
                else if (match == 7) {
                    int len = pos - j, num = (i >> (j + 1)) & ((1 << len) - 1);
                    suffix = ((1 << (j - 2)) - 1) & i;
                    Element->SL = j - 2;
                    Element->SV = f2(suffix, Element->SL);
                    Element->S.push_back(getIndex(num, len));
                    j -= 2;
                    pos = j - 1;
                }
            }
            int sf = Element->PL == block_size ? i : suffix;
            if (Element->PL == block_size)
                Element->PV = f2(i, Element->PL);
            if ((sf & 3) == 3)
                Element->p = 2;
            else if (sf & 1)
                Element->p = 1;
        }
    }
    return Table;
}

int** build_Fib3() {
	fib3.push_back(1);fib3.push_back(2);fib3.push_back(4);
	start.push_back(1);start.push_back(2);start.push_back(3);start.push_back(5);start.push_back(9);

    for (int i = 3; fib3.back() <= wcount; i++)
        fib3.push_back(fib3[i - 3] + fib3[i - 2] + fib3[i - 1]),
        start.push_back(start.back() + fib3.back());

    int **Table = new int*[M + 3];
    Table[0] = new int[numShifts];
    for (int shift = 0; shift < numShifts; shift++)
        Table[0][shift] = 0;
    for (int i = 1; i <= M; i++) {
        Table[i] = new int[numShifts];
        for (int f = fib3.size() - 1, j = i; f>=0; f--)
            if (j >= fib3[f]) {
                j -= fib3[f];
                for (int shift = 0; shift < numShifts && f + shift < fib3.size(); shift++)
                    Table[i][shift] = Table[j][shift] + fib3[f + shift];
                break;
            }
    }
    Table[M + 1] = new int[numShifts];
    Table[M + 2] = new int[numShifts];
    for (int j = 1; j < numShifts; j++)
        Table[M + 1][j] = -fib3[j - 1];
    for (int j = 2; j < numShifts; j++)
        Table[M + 2][j] = - fib3[j - 1] - fib3[j - 2];
    return Table;
}

void dict() {
    fstream re;
    re.open("dic", ios::in);
    string name, codeS;
    int code, cnt = 0;
    while (re>>name>>codeS) {
        code = 0;
        cnt ++;
        for (int i = 0; i < codeS.size() - 3; i++) {
            int pw = codeS.size() - 4 - i;
            if (codeS[i] == '1')
                code |= 1 << pw;
        }
        int cd = getIndex(code, codeS.size() - 3);
        dictionary[cd] = name;
    }
    re.close();
}

void full_Fib3_decode() {
	int **Fib3 = build_Fib3();
    entry **Tab = build_partial_decoding_table();
    dict();

    fstream in, out;
    in.open("encodedBinaryFib", ios::in|ios::binary|ios::ate);

    int size = in.tellg();
    char *memblock = new char[size];

    in.seekg(0, ios::beg);
    in.read(memblock, size);
    in.close();

    //out.open("output.txt");
    int oldSL(0), oldSV(0), p(0);
    entry e;

	//size=900000;
    //clock_t begin = clock();
	//QueryPerformanceFrequency(&tfreq);
	QueryPerformanceCounter(&tstart);
    for (int i = 0; i < size; i++) {
        unsigned char code = memblock[i];
        e = Tab[p][code];
        p = e.p;
        if (e.PL == block_size) {
            oldSV += Fib3[e.PV][oldSL];
            oldSL += e.PL;
        }
        else {
            int index = oldSV + Fib3[e.PV][oldSL] + start[oldSL + e.PL];
			decoded_fib[cur_dec++]=index;
			for(int i=0;i<e.S.size();i++) decoded_fib[cur_dec++]=e.S[i];
           //out<<dictionary[index]<<" ";
           //for (int el: e.S);
                //out<<dictionary[el]<<" ";
            oldSV = e.SV;
            oldSL = e.SL;
        }
	//	cur_dec++;
    }
	QueryPerformanceCounter(&tend);
	cout<<double(tend.QuadPart - tstart.QuadPart) * 1000<<"\n";

/*    clock_t end = clock();

    cout<<double(end - begin) / CLOCKS_PER_SEC<<"\n";*/

    //out<<"\n";
    //out.close();
    delete[] memblock;
}

void dic_to_file() {
fstream out;
	out.open("kjv_formatted_fin", ios::out);
	for(int i=0;i<300000;i++)
		out<<dictionary[decoded_fib[i]]<<" ";
	out.close();
}

int scdc_encode(int L,uint* numbers,uchar* outp) {
int i,cur,x,j=0;
uchar temp[10];
	for(i=0;i<L;i++) {
		cur=9;
		temp[cur--]=C+numbers[i]%S;
		x=numbers[i]/S;
		while(x>0) {
			x--;
			temp[cur--]=x%C;
			x/=C;
		}
		for(int t=cur+1;t<10;t++)
			outp[j++]=temp[t];
	}
	cout<<"j="<<j<<"\n";
	return j;
}

int rpbc_encode(int L,uint* numbers,uchar* outp) {
int i,cur,x,j=0;
	for(i=0;i<L;i++) {
		x=numbers[i];
		if(x<v1)
			outp[j++]=x;
		else
			if(x<v1+v2*R) {
				outp[j++]=(int)((x-v1)/R)+v1;
				outp[j++]=(x-v1)%R;
			} else
				if(x<v1+v2*R+v3*R2) {
					t=x-v1-v2*R;
					outp[j++]=(int)(t/R2)+v1+v2;
					outp[j++]=(int)((t%R2)/R);
					outp[j++]=t%R;
				} else {
						t=x-v1-v2*R-v3*R2;
						outp[j++]=(int)(t/R3)+v1+v2+v3;
						outp[j++]=(int)((t%R3)/R2);
						outp[j++]=(int)((t%R2)/R);
						outp[j++]=t%R;
					}
	}
	return j;
}

#pragma optimize( "", off )
int scdc_decode(uchar* code, uint* out,int L_coded) {
int tot,byte,cur,temp,base,m,k;
uchar* x;
	/*//generate base
	base[0]=0;
	tot=S;
	out_text="";*/
/*	for(int i=1;i<5;i++) {
		base[i]=base[i-1]+tot;
		tot*=C;
	}*/
	//decode
	x=code;m=0;s1=s2=0;
	//cout<<"L_coded="<<L_coded;
	for(k=0,byte=0,m=0;m<L_coded;m+=byte+1) {
		//s1++;
		x=codes_scdc+m;
		cur=byte=base=0;
		tot=S;
		//cout<<codes_scdc[byte]<<"  ";
		while(x[byte] < C) {
			//s2++;
            cur = cur * C + x[byte];
            byte++;
			base=base+tot;
			tot=tot*C;
		}
		cur = cur * S + x[byte] - C;
		//temp=cur+base;
		outSCDC[k++]=Dict_i235+cur+base;
		//out_text=out_text+DictSCDC[cur + base[byte]];
		//decoded_scdc[k]=cur+base[byte];
		//outSCDC[k++]=DictSCDC[cur+base[byte]];
	}
	return m;
}

#pragma optimize( "", off )
void set_v1_v4() {
	v1=220;v2=35;v3=1;v4=0;
}

void create_tables() {
int start=0;
set_v1_v4();
	for(int i=0;i<v1;i++,start++) {
		suffix[i]=0;
		first[i]=start;
	}
	for(int i=v1;i<v1+v2;i++,start+=R) {
		suffix[i]=1;
		first[i]=start;
	}
	for(int i=v1+v2;i<v1+v2+v3;i++,start+=R2) {
		suffix[i]=2;
		first[i]=start;
	}
	for(int i=v1+v2+v3;i<R;i++,start+=R3) {
		suffix[i]=3;
		first[i]=start;
	}
}

int rpbc_decode(uchar* code, uint* out,int L_coded) {
int b,k=0,offset;
	QueryPerformanceCounter(&tstart);
	for(int i=0;i<L_coded;i++) {
		b=code[i];
		offset=0;
		for(int j=1;j<suffix[b];j++,i++)
			offset=offset*R+code[i];
		//outSCDC[k++]=Dict_i235+offset+first[b];
		//out_i235s[k++]=*(Dict_i235+offset+first[b]);
		rand_test[k++]=offset+first[b];
	}
	QueryPerformanceCounter(&tend);
	return k;
}

void print_codeword(uint x) {
uint k,p;
	for(unsigned int j=1<<(t-1),p=t-1;j;j>>=1,p--)
		if(x&j)
			k=p;
	for(p=t-1;p>=t-k+1;p--)
		if(x&(1<<p))
			printf("1");
		else
			printf("0");
	printf("\n");
}



void obtain_codes235() {
const int L=120000;
uint C[L],Lengths[L],UpToL[200],k,p,summ;
	for(int i=1;i<L;i++) {
		int x=get_code(i);
		C[i]=x;
		Lengths[i]=d235_length(x);
	}
}


//f=0 - w1 not output, f=1 - output w1, not w2, f=2 - output w3, no w4
#pragma optimize( "", off )
int decode_C() {
uint w,r,k,i;
struct tabs *v;
	r=0; w=1; k=0;
	out_text="";
	for(i=0;i<clen;i++) {
		v=TARR[r][codes_d235[i]];
		w=(w<<v->s)|v->w1;
		if(v->f) {
			out235[k++]=Dict+w;
			//glob_res=(char*)(Dict+w);
			if(v->f>1) {
				out235[k++]=Dict+v->w2;
				//glob_res=(char*)(Dict+v->w2);
				if(v->f>2) {
					out235[k++]=Dict+v->w3;
					//glob_res=(char*)(Dict+v->w3);
					w=v->w4;
				} else
					w=v->w3;
			} else
				w=v->w2;
		}
		r=v->r;
	}
	return r;
}

void dict_from_file() {
int i=0;
ifstream input("dic.txt");
while(getline(input, Dict[i])) {
	if(input) {
	/*	if(i>=15000)*/
			dic_map1.insert(make_pair(i,Dict[i]));
	//	else
			dic_map.insert(make_pair(Dict[i],i));

	}
	i++;
}
dic_size=i;
}

void dict_from_file_scdc() {
int i=0;
ifstream input("dic_scdc.txt");
while(input>>DictSCDC[i]) {
	if(input) {
		dic_map_scdc.insert(make_pair(DictSCDC[i],i));
		i++;
	}

	}
dic_size_scdc=i;
}

void text_to_ranks(map<string,int> dic_map,const char* file_name,unsigned int* ranks){
int i=0,j=0, len=0;
string word;
char x;
	for(int i=0;i<diff_words;i++)
		_0ranks[i]=0;
	ifstream file(file_name);
	//ofstream fout("result.txt");
	while ( ! file.eof() ) {
			file>>word;
			if(dic_map.find(word)!=dic_map.end()) {
				ranks[i++]=dic_map[word];
				len++;
			}else
				i=i;
			if(i>=2 && ranks[i-2]==0) {
				_0ranks[ranks[i-1]]++;
			}
			if(i%100000==0)
				cout<<i<<" ";
    }
	/*for(int i=0;i<diff_words;i++)
		if(_0ranks[i]>_0ranks[rank_after0])
			rank_after0=i;*/

	//cout<<"rank["<<rank_after0<<"]="<<_0ranks[rank_after0]<<"\n ";
	//fout.close();
	/*for(int i=0;i<len;i++) {
        cout << ranks[i] << ' ';
	}*/
	cout<<"Text to ranks done";
	Nwords=i;
}

bool d235_in_i235(uint x) {
int i;
	x=invert_d235(x);
	for(i=0;i<30000 && i235[i]!=x;i++);
	return i<30000;
}

void d235_codeword_map(int L) { //generates map <n,<code(n),length(code(n))>>
uint x,k(0),p;
	for(int i=1;i<L;i++) {
		x=get_code(i);
		d235_map.insert(make_pair(i,make_pair(x,d235_length(x))));
//find the rightmost 1. Codeword ends one bit right to it
/*		if(i==31)
			i=i;
		for(unsigned int j=1<<(t-1),p=t-1;j;j>>=1,p--)
			if(x&j)
				k=p;
		x>>=k-1;
		if(!d235_in_i235(x))
			x=get_code(i);*/
	}
}

int state_max=7;
struct ai_state a_step(ai_state prev,int bit) {
ai_state r={0,prev.L+1,prev.p,-1};
	if(bit==0) {
		switch(prev.st) {
			case 0: r.p=prev.p+nlk[prev.L][0];
					break;
			case 1: r.p=prev.p+nlk[prev.L][1];
					break;
			case 2: r.res=prev.p; r.p=0; r.L=4;
					break;
			case 3: r.p=prev.p+nlk[prev.L][2];
					break;
			case 4: r.res=prev.p; r.p=4; r.L=6;
					break;
			case 5: r.res=prev.p; r.p=9; r.L=7;
					break;
		}
	} else {
		r.st=prev.st<6?prev.st+1:6;
		if(prev.st==5)
			r.p=prev.p+nlk[r.L][3];
		if(prev.st==6)
			r.p=prev.p+nlk[r.L][4];
	}
	return r;
}

// new automaton modelling
struct ai_state a_step1(ai_state prev,int bit) {
ai_state r={0,prev.L+1,prev.p,-1};
	if(bit==0) {
		switch(prev.st) {
			case 0: r.p=prev.p+nlk[prev.L][0];
					break;
			case 1: r.p=prev.p+nlk[prev.L][1];
					break;
			case 2: r.p=nl[r.L]-1;
					break;
		}
	} else {
		r.st=prev.st<2?prev.st+1:2;
		if(prev.st==1) {
			r.L=3;
			r.res=0;
		}
	}
	return r;
}

void decodeI235(int N) {
ai_state state={0,1,-1,-1};
	for(int i=0,j=0;i<N;i++) {
		uint byte=codes[i];
		for(uint k=128;k>0;k>>=1) {
			state=a_step(state,byte&k);
			if(state.res!=-1)
				out_i235[j++]=Dict_i235+state.res;
		}
	}
}

// new decoding method - slow; for testing
void decodeI235_1(int N) {
ai_state state={0,1,-1,-1};
	for(int i=0,j=0;i<N;i++) {
		uint byte=codes[i];
		for(uint k=128;k>0;k>>=1) {
			state=a_step1(state,byte&k);
			if(state.res!=-1)
				out_i235[j++]=Dict_i235+state.p;
				rand_test[j++]=state.p;
		}
	}
}

int nf[100000];
int nf1[100000];
int nf2[100000];
int nf3[100000];
int nf4[100000];

void formT235_inner(int L,ai_state state) {
tabsi235 v;
ai_state a=state;
	for(uint x=0;x<256;x++) {
		state=a;
		v.p1=v.p2=v.p3=v.p4=-1;
		for(uint k=128;k>0;k>>=1) {
			state=a_step(state,x&k);
			if(state.res!=-1)
				if(v.p1==-1)
					v.p1=state.res;
				else
					if(v.p2==-1)
						v.p2=state.res;
					else
						v.p3=state.res;
		}
		if(v.p1==-1)
			v.p1=state.p;
		else
			if(v.p2==-1)
				v.p2=state.p;
			else
				if(v.p3==-1)
					v.p3=state.p;
				else
					v.p4=state.p;
		/*v.al=(struct tabsi235*)(T235+256*state_max*state.L+256*state.st);
		*(T235+256*state_max*a.L+256*a.st+x)=v;*/
		v.al=(struct tabsi235*)(T235+256*Lmax*state.st+256*state.L);
		*(T235+256*Lmax*a.st+256*a.L+x)=v;
	/*	nf[256*Lmax*a.st+256*a.L+x]=256*Lmax*state.st+256*state.L;
		nf1[256*Lmax*a.st+256*a.L+x]=v.p1;
		nf2[256*Lmax*a.st+256*a.L+x]=v.p2;
		nf3[256*Lmax*a.st+256*a.L+x]=v.p3;
		nf4[256*Lmax*a.st+256*a.L+x]=v.p4;*/
	}
}

uint pack(tabsi2351 v, ai_state s) {
uint res=state_max*s.L+s.st;
//uint res=Lmax*s.st+s.L;
	res|=(v.p1<<7);
	if(v.p2!=-1)
		res|=(v.p2<<22);
	if(v.p3>0) {
		res|=(v.p3<<25);
		res|=0x80000000;
	}
	if(v.n>0)
		res|=0x200000;
	if(v.n>1)
		res|=0x20000000;
	if(v.n>2)
		res|=0x40000000;

	return res;
}

int mx=0;
void formT235_inner1(int L,ai_state state) {
tabsi2351 v;
ai_state a=state;
	for(uint x=0;x<256;x++) {
		state=a;
		v.p1=v.p2=v.p3=v.p4=-1;
		v.n=0;
		for(uint k=128;k>0;k>>=1) {
			state=a_step1(state,x&k);
			if(state.res!=-1) {
				if(v.n==0) {
					v.p1=state.p;
					v.p2=0;
				} else
					if(v.n==1) {
						v.p2=state.p;
						v.p3=0;
					} else {
						v.p3=state.p;
						v.p4=0;
					}
				v.n++;
			}
		}
		if(state.st<2)
			if(v.n==0)
				v.p1=state.p;
			else
				if(v.n==1)
					v.p2=state.p;
				else
					if(v.n==2)
						v.p3=state.p;
					else
						v.p4=state.p;
		/*if(v.p3>0)
			cout << v.p2 <<" "<< v.p3 << " | ";*/
		v.al=(struct tabsi2351*)(T235_1+256*state_max*state.L+256*state.st);
		*(T235_1+256*state_max*a.L+256*a.st+x)=v;
		/*v.al=(struct tabsi2351*)(T235_1+256*Lmax*state.st+256*state.L);
		*(T235_1+256*Lmax*a.st+256*a.L+x)=v;*/
		experT[state_max*a.L+a.st][x]=pack(v,state);
		//experT[Lmax*a.st+a.L][x]=pack(v,state);
	}
}

void formT2351() {
	state_max=3;
	for(int a=0;a<state_max;a++)
		for(int L=0;L<Lmax;L++) {
			ai_state state={a,L,0,-1};
			formT235_inner1(L,state);
		}
	/*for(int j=0;j<4096;j++)
		for(int i=0;i<256;i++)
			experT[j][i]=rand();*/
}

void formT235() {
	for(int a=0;a<state_max;a++)
		for(int L=0;L<Lmax;L++) {
			ai_state state={a,L,0,-1};
			formT235_inner(L,state);
		}
	/*for(int j=0;j<4096;j++)
		for(int i=0;i<256;i++)
			experT[j][i]=rand();*/
}

//====================================== test function pointer
int glob_=0;

void f1() {
	glob_++;
	out_i235[glob_]=Dict_i235+113;
}

void f2() {
	glob_++;
	out_i235[glob_]=Dict_i235+113;
}

void (*actions[2])()={f1,f2};
//============================================================

int decode_i235_fast_orig() {
uint p=0,k=0,i=0,temp=0,x,y,numb=256,p1,p2,p3,p4;
uint real_len;
struct tabsi235 t={0,0,0,0,(struct tabsi235*) T235+256},*v1;
unsigned char* u=(unsigned char*)codes,*end=(unsigned char*)codes+ilen;
int *w,*w1;
	r1=r2=r3=r4=0;
	//for(;u<end;u++) {
	for(i=0;i<ilen;i++) {	//21
		//actions[i&1]();
		//r1++;
		//suml+=(t.al-T235)/sizeof(struct tabsi235);
		real_len=((t.al-T235)%(256*Lmax))/256;
		cout << ((t.al-T235)-real_len*256)/(236*Lmax) << " ";
		t=t.al[codes[i]];//v=v->al+codes[i];
		p+=t.p1;//p+=v->p1;
		//if(v->p2>=0) {
		if(t.p2>=0) {
			//r2++;
			out_i235[k++]=Dict_i235+p;
			//out_i235s[k++]=*(Dict_i235+p);
			if(t.p3>=0) {
				//r3++;
				out_i235[k++]=Dict_i235+t.p2;
				//out_i235s[k++]=*(Dict_i235+t.p2);
				if(t.p4>=0) {
					//r4++;
					out_i235[k++]=Dict_i235+t.p3;
					//out_i235s[k++]=*(Dict_i235+t.p3);
					p=t.p4;
				} else
					p=t.p3;
			} else
				p=t.p2;
		}
	}
//	cout << "suml=" << (float)(suml/ilen) << "\n";
	return p;
}

//#pragma optimize( "", off )
// new decoding method - fast
int decode_i235_fast() {
uint p=0,k=0,i=0,temp=0,x,y,numb=256,p1,p2,p3,p4,tt,cds,ptr=11;
struct tabsi2351 t={0,0,0,0,0,(struct tabsi2351*) T235_1+256*11},*v1;
unsigned char* u=(unsigned char*)codes,*end=(unsigned char*)codes+ilen,j=11;
int *w,*w1;
uint* e=(uint*)experT;
	QueryPerformanceCounter(&tstart);
	r1=r2=r3=r4=0;
	for(i=0;i<ilen;i+=4) {
		cds=*(uint*)(codes+i);
		ptr=experT[ptr&0x7f][(uchar)cds];
		tt=ptr>>7;
		if(tt<0x4000)
			p+=tt;
		else {
			p+=tt&0x3fff;
			rand_test[k++]=p;
			tt>>=15;
			if(tt<128)
				p=tt;
			else {
				if(tt<512) {
					rand_test[k++]=tt&0x7f;
					p=0;
				} else {
					rand_test[k++]=tt&0x7;
					tt>>=3;
					if(tt&0x20) {
						rand_test[k++]=tt&0xf;
						p=0;
					} else
						p=tt&0xf;
				}
			}
		}

		ptr=experT[ptr&0x7f][(uchar)(cds>>8)];
		tt=ptr>>7;
		if(tt<0x4000)
			p+=tt;
		else {
			p+=tt&0x3fff;
			rand_test[k++]=p;
			tt>>=15;
			if(tt<128)
				p=tt;
			else {
				if(tt<512) {
					rand_test[k++]=tt&0x7f;
					p=0;
				} else {
					rand_test[k++]=tt&0x7;
					tt>>=3;
					if(tt&0x20) {
						rand_test[k++]=tt&0xf;
						p=0;
					} else
						p=tt&0xf;
				}
			}
		}

		ptr=experT[ptr&0x7f][(uchar)(cds>>16)];
		tt=ptr>>7;
		if(tt<0x4000)
			p+=tt;
		else {
			p+=tt&0x3fff;
			rand_test[k++]=p;
			tt>>=15;
			if(tt<128)
				p=tt;
			else {
				if(tt<512) {
					rand_test[k++]=tt&0x7f;
					p=0;
				} else {
					rand_test[k++]=tt&0x7;
					tt>>=3;
					if(tt&0x20) {
						rand_test[k++]=tt&0xf;
						p=0;
					} else
						p=tt&0xf;
				}
			}
		}
		ptr=experT[ptr&0x7f][(uchar)(cds>>24)];
		tt=ptr>>7;
		if(tt<0x4000)
			p+=tt;
		else {
			p+=tt&0x3fff;
			rand_test[k++]=p;
			tt>>=15;
			if(tt<128)
				p=tt;
			else {
				if(tt<512) {
					rand_test[k++]=tt&0x7f;
					p=0;
				} else {
					rand_test[k++]=tt&0x7;
					tt>>=3;
					if(tt&0x20) {
						rand_test[k++]=tt&0xf;
						p=0;
					} else
						p=tt&0xf;
				}
			}
		}
		/*tt=experT[j][codes[i]];
		j=(uchar)tt&0xff;
		tt>>=8;
		if(tt<0x4000)
			p+=tt;
		else {
			p+=tt&0x3fff;
			rand_test[k++]=p;
			tt>>=15;
			if(tt<128)
				p=tt;
			else {
				rand_test[k++]=p&0x7;
				tt>>=3;
				p=tt&0xf;
				if(tt&0x20)
					rand_test[k++]=0;
			}
		}*/
	}
	QueryPerformanceCounter(&tend);
	return p;
}


int k[20]={0,1,100}; //numbers which are not delimiters, the last = biggest delimiter+1
int kmax=3,Lmin=3; //number of elements in the array k; length of the shortest codeword

void gen_reverse() {
    int L,n,i,j,L1_long=-1,L1_long_prev=-1,t=2;
    uint seq=0,s=0;
	for(L=3,n=0;L<Lmax+8 && n<=max_i235;L++) {
		nl[L]=n;
		//cout<<"L="<<L-1<<"; Cn="<<n<<" s="<<s<<"\n";
		for(i=0;L-k[i]>Lmin && i<kmax;i++) {
			if(i==kmax-1 && L1_long==-1)
				L1_long=n;
			nlk[L][i]=n-nl[L-k[i]-1];
			for(j=nl[L-k[i]-1];j<nl[L-k[i]] && n<=max_i235;j++,n++) {
				seq=0xFFFF>>(16-k[i]);
				i235[n]=(i235[j]<<(k[i]+1))|seq;
			}
		}
		if(L1_long_prev>-1) {
			nlk[L][kmax]=n-L1_long_prev;
			for(j=L1_long_prev;j<nl[L] && n<=max_i235;j++,n++)
				i235[n]=(i235[j]<<1)|1;
		}
		if(t<kmax && L-1!=k[t]) {
			i235[n]=(1<<(L-1))-1;
			n++;
		} else
			t++;
		L1_long_prev=L1_long;
		L1_long=-1;
	}
	seq=1;
}

int word_frequences(char* s) {
	double pi;
	string word;
	int size=0;
	ifstream file(s);
	while ( ! file.eof() ) {
		file>>word;
		if(i235_map.find(word)!=i235_map.end())
			i235_map[word]++;
		else
			i235_map.insert(make_pair(word,1));
		size++;
	}
	int i=0;
	multimap<int,string> freq_i235;
	map<string,int> :: iterator it;
	multimap<int,string> :: iterator it1,it3;
	entropy=0;
	diff_words=0;
	for(it=i235_map.begin();it!=i235_map.end();it++,diff_words++) {
		freq_i235.insert(make_pair(it->second,it->first));
		pi=(double)it->second/size;
		entropy-=pi*(long double)log((long double)pi)/(long double)log((long double)2);
	}

    // put frequencies to file
    ofstream fo("freqs13.txt");
    for (auto it = freq_i235.begin(); it != freq_i235.end(); it++) {
        fo << it->second <<  ' ' << it->first << endl;
    }

    fo.close();
    // end freq

	for(it1=freq_i235.end(),it1--,i=0;it1!=freq_i235.begin();it1--,i++) {
		i235_map_sorted.insert(make_pair(it1->second,i)); //<word of text,number>
		Dict_i235[i]=it1->second;
	}
	i235_map_sorted.insert(make_pair(it1->second,i));//insert freq_i235.begin()
	Dict_i235[i]=it1->second;
// D235 uses i235 frequences map ======================================
	multimap<int,int> length_map;
	multimap<int,int> :: iterator it2;
	map<int,pair<int,int> > :: iterator it4;
	d235_codeword_map(100000);
	for(it4=d235_map.begin();it4!=d235_map.end();it4++)
		length_map.insert(make_pair((it4->second).second,it4->first)); //<lenth(code(n)),n>
	for(it3=freq_i235.end(),it2=length_map.begin(),it3--,i=0;it3!=freq_i235.begin();it3--,it2++,i++) {
		d235_map_sorted.insert(make_pair(it3->second,it2->second)); //<word,n>
		Dict[it2->second]=it3->second;
	}

	return size;

}

void write_to_file_i235(int N,string** indices,char* fname) {
string word;
	//ifstream in_file("kjv_formatted_fin.txt");
	ofstream file(fname);
	for(int i=1;i<N;i++) {
		file<<*(indices[i])<<" ";
	//	in_file>>word;
	}
	file.close();
}

void i235_test(int N,char* fname) {
string word;
/*ifstream in_file(fname);
	for(int i=1;i<N;i++) {
		in_file>>word;
		if(word!=*out_i235[i])
			cout<<"!!!!!!!!!!!!!!!!!!!!!! "<<i<<" !!!!!!!!!!!!!!!!!!!!!!!!!!!";
	}*/
	for(int i=0;i<30000;i++)
		for(int j=i+1;j<30000;j++)
			if(i235[i]==i235[j])
				i=i;
}

void write_to_file_binary(uchar* code,const char* filename,int N) {
string word;
	ofstream file(filename);
	for(int i=0;i<N;i++)
		file<<code[i];
	file.close();
}

void createEncodedFile(string t) {
    FILE* out = fopen(("res"+t+".bmp").c_str(), "wb");
    for (int i = 0; i < cur_byte; i++){
        fputc(codes[i], out);
    }
    fclose(out);
}

void process(string num) {
    int scdc_L=0;
	int ilen=1000;
	double harm=0,u=0,su=100;
	unsigned int i, j, maxi4 = 100000, iter = 100, uu = 0, size, t, v, min_i, min_j, min_x, min_z, min_y;
	string str="out"+num+".txt";
	char* fname = &str[0];
		QueryPerformanceFrequency(&tfreq);
		size = word_frequences(fname);		// get the word frequences in the text
		gen_reverse();		// generate the reverse multidelimiter codeword set
		formT2351();		// preprocess table for the fast decoding method

	cout<<"   "<<"entropy="<<entropy<<"\n";
	cout<<"R="<<(float)cur_byte*8/Nwords<<"  ";

	cout<<"formed!!!!!!!!!!!!!!!!!!!!!!!!";

	sumi=sumd=sums=sum1=0;
	S=223;
	C=256-S;

	text_to_ranks(i235_map_sorted,fname,ranks);	// generate the array of word indices

	/*for (auto it = i235_map_sorted.begin(); it !=i235_map_sorted.end(); it++) {
		cout << it->first <<  ' ' << it->second << endl;
	}
	for (int i=0; i < Nwords; i++)
        cout << ranks[i] << ' ';*/

	encodeI235(Nwords,ranks);					// generate the R2x code
		//decodeI235_1(ilen+1);
    createEncodedFile(num); // creates encoded file

	cout <<"size:"<<size<<"different words:"<<diff_words<<"\n"<<"I235 bytes: "<<cur_byte<<", I235 av codeword length: "<<(float)cur_byte*8/Nwords<<"; entropy="<<entropy<<"\n";
	create_tables();
	scdc_L=rpbc_encode(Nwords,ranks,codes_scdc);
	cout<<"S="<<S<<" scdc_L= "<<scdc_L<<" SCDC av codeword length: "<<(float)scdc_L*8/Nwords<<"\n";
	for(int iter=0;iter<10;iter++) {
		u+=decode_i235_fast();
		sumi+=(float)(tend.QuadPart - tstart.QuadPart)/tfreq.QuadPart;

		uu+=rpbc_decode(codes_scdc,decoded_scdc,scdc_L);

		sums+=(float)(tend.QuadPart - tstart.QuadPart)/tfreq.QuadPart;


	}
	cout<<"s1="<<s1<<"s2="<<s2<<"r1="<<r1<<"r2="<<r2<<"r3="<<r3<<"r4="<<r4;
	cout<<"decode_SCDC time: "<<(float)sums<<" scdc_L="<<scdc_L;
	cout/*<<" decode_C_fast time: "<<(float)sumd/tfreq.QuadPart*/<<" decode_I_fast time: "<<(float)sumi<<"\n";
	//i235_test(Nwords,fname);
	cout<<"uu="<<uu*2<<' '<<u<<"\n";
	cout << *out235 << endl;
	getch();
}

int main() {
    image im;
    string num = "13";
    im.getBytes(num);
    im.createBinaryCodeSnake();
    im.putToTxtSnake(num);
    /*im.createBinaryCodeDiffs(num);
    im.createImageDiffs(num);*/
    //im.createImageDecoding(num, codes_scdc, Nwords);
    process(num);
}
