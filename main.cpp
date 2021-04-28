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
#include <functional>
#include "../rmd/image.h"

using namespace std;

typedef unsigned char uchar;
typedef unsigned int uint;

const int reg_len=13,sub_reg_len=64-reg_len,tab_size=1<<reg_len,mask=tab_size-1;

const int N=610000,L=30000000,shLen=1000000; // maximal dictionary size and encoded text length

//============================ SCDC/RPBC externals
extern void create_tables();
extern int rpbc_encode(int,uint*,uchar*);
extern int rpbc_decode(uchar*, uint*,int);
extern int mulka_rpbc_decode(uchar*, uint*,int);
extern int mulka_rpbc_decode1(uchar*, uint*,int);
extern int rpbc_encode(int,uint*,uchar*);
extern int scdc_decode(uchar*, uint*,int);
extern int scdc_encode(int,uint*,uchar*);
extern void generate_ternary(int);
extern int encode_ternary(int,uint*);
extern int decode_ternary(uchar*, short unsigned int*,int);
extern int decode_ternary_long(uchar*, unsigned int*,int);
extern int decode_ternary1(uchar*, short unsigned int*,int);
extern int decode_ternary8(uchar*, short unsigned int*,int);
extern int decode_ternary16(uchar*, short unsigned int*,int);

extern int v1,v2,v3,v4,s1,s2;
extern unsigned char codes_scdc[L*4];
extern unsigned int decoded_scdc[20000000];
extern uint ternary_code[1000000];
extern uchar coded_t[L];
// here
uchar shift[shLen];
unsigned short Lptrs[100000];
unsigned short Lptr[100000];
unsigned short P[shLen];
unsigned short ternary[100000];


//============== rmd ===================
struct tabsrmd {   // Element of fast decoding table
	int p1,p2,p3,p4;
	struct tabsrmd* al;
};

struct tabsrmd1 {
	int p1,p2,p3,p4,n;
	struct tabsrmd1* al;
};

struct tabs_packed {
    struct tabs_packed *ptr;
    unsigned short p1;
    uchar p2p3;
    uchar flag;
};

const int pack_max=200;
unsigned int experT[200][256]; // New fast decoding table
struct tabs_packed experT3[pack_max*256];
struct tabsrmd Trmd[1000000]; // Fast decoding table
struct tabsrmd1 Trmd_1[1000000];
const int Lmax=30,max_rmd=30000; // maximum codeword length and maximum number of codewords


int nlk[Lmax+8][40]; // array used in the decoding
int code_len; // length of code in bytes
int nl[Lmax]; // nl[i] = number of codewords of length i
uint freqs[256][256],cd_len[2][30];

uchar randarr1[256];
uint randarr[1500],randarr2[256];
ULONG64 randarr_long[256];


// The decoding automaton state
struct ai_state {
	int st,L,p,res;
};

//=============== Values used to measure timing ==============
LARGE_INTEGER tstart, tend, tfreq;
double sumi,sums;

unsigned int rank_after0=0,ranks[L],rmd[300000],sumd,sum1,totali,total;
unsigned char codes[L],r_codes[L];
unsigned short outrmd[L*2];
unsigned int outrmd_long[L*2];
string *out_rmd[20000000],Dict_rmd[1000000];
string result;

string Dict[N*2]; //dictionary
map<string,int> rmd_map,rmd_map_sorted;
multimap<int,string> freq_rmd;
int Nwords,diff_words; // Total number of words and number of different words in text

double entropy; // Shannon entropy
int t=sizeof(unsigned int)*8,cur_byte(0),cur_bit(7);


//==================== Encode ===================

// writes a codeword to the end of a code bitstream
// x is a codeword aligned to the right edge of a 32-bit word
void flush_to_byte_rmd(unsigned int x) {
int k(0),p;
unsigned int j;
// Find the leftmost '1' bit in the last byte of a code. Codeword starts 1 bit left to it
	for(j=1<<(t-1),p=t-1;!(x&j) && j;j>>=1,p--);
	cd_len[0][p+2]++;
	for(j<<=1;j>0;j>>=1) {
		if(j&x)
			codes[cur_byte]|=(1<<cur_bit);
		cur_bit=cur_bit==0?7:cur_bit-1;
		if(cur_bit==7)
			cur_byte++;
	}
}

void encode_rmd(int max_rank,uint* ranks) {
int i;
    /*for(i=0;i<30;i++)
        cd_len[0][i]=cd_len[1][i]=0;*/
	cur_byte=0; cur_bit=7;
	//uint TERN=0;
	for(i=0;i<max_rank;i++) {
		flush_to_byte_rmd(rmd[ranks[i]]); // write current codeword to the end of a code bitstream
	//	TERN+=ternary[ranks[i]];
	}
	//cout<<"TERN="<<TERN<<endl<<"Av codeword length="<<(float)TERN/max_rank;
	code_len=cur_byte;
}

void revert_bytes() {
uchar x;
    for(int i=0;i<code_len;i++) {
        r_codes[i]=0;
        for(uchar j=128,k=1;j>0;j>>=1,k<<=1)
            if(codes[i]&j)
                r_codes[i]|=k;
    }
}

//Mapping text into integers = indices of words in the array ordered by the descending order of their frequences,
//i.e. in the map dic_map<word,index>
void text_to_ranks(map<string,int> dic_map, Image image, unsigned int* ranks){
int i=0,j=0;
string word;
char x;

	for (auto v : image.rv) {
		word = to_string(v);
		if(dic_map.find(word)!=dic_map.end())
			ranks[i++]=dic_map[word];
		if(i%100000==0)
			cout<<i<<" ";
    }

    for (auto v : image.gv) {
		word = to_string(v);
		if(dic_map.find(word)!=dic_map.end())
			ranks[i++]=dic_map[word];
		if(i%100000==0)
			cout<<i<<" ";
    }

    for (auto v : image.bv) {
		word = to_string(v);
		if(dic_map.find(word)!=dic_map.end())
			ranks[i++]=dic_map[word];
		if(i%100000==0)
			cout<<i<<" ";
    }
	cout<<"\nMapping text to integers done\n";
}

int word_frequences(Image image) {
double pi;
string word;
int size=0,i=0;

    // Create the map <word,frequency> - rmd_map
	for (auto v : image.rv) {
		word = to_string(v);
		if(rmd_map.find(word)!=rmd_map.end())
			rmd_map[word]++;
		else
			rmd_map.insert(make_pair(word,1));
		size++;
		if(size%1000000==0)
			printf("%d ",size);
	}

	for (auto v : image.gv) {
		word = to_string(v);
		if(rmd_map.find(word)!=rmd_map.end())
			rmd_map[word]++;
		else
			rmd_map.insert(make_pair(word,1));
		size++;
		if(size%1000000==0)
			printf("%d ",size);
	}

	for (auto v : image.bv) {
		word = to_string(v);
		if(rmd_map.find(word)!=rmd_map.end())
			rmd_map[word]++;
		else
			rmd_map.insert(make_pair(word,1));
		size++;
		if(size%1000000==0)
			printf("%d ",size);
	}
	printf("file processed\n");

	// Create the multimap <frequency,word> consisting all different words - freq_rmd
map<string,int> :: iterator it;
multimap<int,string> :: iterator it1,it3;
	entropy=0;
	diff_words=0;
	for(it=rmd_map.begin();it!=rmd_map.end();it++,diff_words++) {
		freq_rmd.insert(make_pair(it->second,it->first));
		// Calculate Shannon entropy
		pi=(double)it->second/size;
		entropy-=pi*(long double)log((long double)pi)/(long double)log((long double)2);
	}

	// Create the map <word,index> which maps words of text to integers according to descending order of their frequences
	// and ordered dictionary Dict_rmd
	for(it1=freq_rmd.end(),it1--,i=0;it1!=freq_rmd.begin();it1--,i++) {
		rmd_map_sorted.insert(make_pair(it1->second,i));//<word of text,number>
		Dict_rmd[i]=it1->second;
	}
	rmd_map_sorted.insert(make_pair(it1->second,i));//insert freq_rmd.begin()
	Dict_rmd[i]=it1->second;
	printf("map built\n");
	return size;
}

int new_code_size() {
multimap<int,string> :: iterator it1,it3;
int k=1,j=4,size=0,i;
    for(it1=freq_rmd.end(),it1--,i=0;it1!=freq_rmd.begin();it1--,i++) {
        size+=it1->first*j;
        if(i==k-1) {
            k<<=1;
            j++;
        }
       /* cout<<"i="<<i<<" word freq"<<it1->first<<" k="<<k<<" size="<<size;
        system("pause");*/
    }
    return size;
}

//============== Decode ==============
// R2,3,5 automaton modelling
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

// R2-infinity automaton modelling
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
			r.res=prev.p;
			r.p=0;
		}
	}
	return r;
}

uint pack(tabsrmd1 v, ai_state s) {
uint res=state_max*s.L+s.st;
	res|=(v.p1<<7);
	if(v.p2!=-1) {
		res|=(v.p2<<22);
		res|=0x200000;
	}
	if(v.p3>-1) {
        res|=0x20000000;
        if(v.p3>0) {
            res|=0x40000000;
            res|=(v.p3<<25);
        } else
            res|=(v.p3<<27);
        if(v.p4>-1)
            res|=0x80000000;
	}
	return res;
}

uint pack1(tabsrmd1 v, ai_state s) {
uint res=state_max*s.L+s.st;
    res|=(v.p1<<7);
	if(v.p2!=-1) {
		res|=(v.p2<<22); // >0 result
		res|=0x20000000;
    	if(v.p3>-1) {
            if(v.p3>0) {
                res|=0x40000000;
                res|=(v.p3<<25);
            } else
                res|=(v.p3<<27);
            if(v.p4>-1)
                res|=0x80000000;
        } else
            res|=0x00200000;
	}
 	return res;
}

uint pack2(tabsrmd1 v, ai_state s) {
uint res=state_max*s.L+s.st;
//uint res=Lmax*s.st+s.L;
    res|=(v.p1<<7);
	if(v.p2!=-1) {
		res|=(v.p2<<21); // >0 result
		if(v.p3>-1) {
            res|=0x80000000;
            res|=(v.p3<<26);
            if(v.p4>-1)
                res|=0xc0000000;
        }
	} else
        res|=0x40000000; // 0 results
 	return res; //highest bits: 00 - 1 res, 01 - 0 res, 10 - 2 res, 11 - 3 res
}

tabs_packed pack3(tabsrmd1 v, ai_state s) {
struct tabs_packed res;
    res.ptr=(struct tabs_packed*)experT3+(state_max*s.L+s.st)*256;
    res.p1=v.p1;
    res.flag=res.p2p3=0;
    if(v.p2!=-1) {
		res.p2p3=v.p2; // >0 result
		if(v.p3>-1) {
            res.flag=2;
            res.p2p3|=(v.p3<<5);
            if(v.p4>-1)
                res.flag=3;
        }
	} else
        res.flag=1; // 0 results
 	return res; //highest bits: 00 - 1 res, 01 - 0 res, 10 - 2 res, 11 - 3 res
}

// Creates all fast decoding table elements for given codeword length and decoding automaton state
void formTrmd_inner(int L,ai_state state) {
tabsrmd v;
ai_state a=state;
	for(uint x=0;x<256;x++) {
		state=a;
		v.p1=v.p2=v.p3=v.p4=-1;
		for(uint k=128;k>0;k>>=1) {
			state=a_step1(state,x&k); // The function depends on code parameters
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
		v.al=(struct tabsrmd*)(Trmd+256*Lmax*state.st+256*state.L);
		*(Trmd+256*Lmax*a.st+256*a.L+x)=v;
	}
}

int mx=0;
void formTrmd_inner1(int L,ai_state state) {
tabsrmd1 v;
ai_state a=state;
	for(uint x=0;x<256;x++) {
		state=a;
		v.p1=v.p2=v.p3=v.p4=-1;
		for(uint k=128;k>0;k>>=1) {
			state=a_step1(state,x&k); // The function depends on code parameters
			if(state.res!=-1) {
				if(v.p1==-1)
					v.p1=state.res;
				else
					if(v.p2==-1)
						v.p2=state.res;
					else
						v.p3=state.res;
                v.n++;
			}
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
        experT[state_max*a.L+a.st/*Lmax*a.st+a.L*/][x]=pack2(v,state);
	}
}

void formTrmd_inner1_rev(int L,ai_state a) {
uint sh,global_sh,prev_sh;
tabsrmd1 v;
ai_state state;
uchar res0=0;
    prev_sh=L?256*((L>>3)*3+a.st-2):0;
	for(uint x=0;x<256;x++) {
		state=a;
		v.p1=v.p2=v.p3=v.p4=-1;
		sh=(L?0:3);
        for(uint k=L?1:8;k<256 && state.res==-1;k<<=1,sh++)
			state=a_step1(state,x&k); // The function depends on code parameters
        if(state.res!=-1)
            global_sh=0;
        else
            global_sh=L?256*((L>>3)*3+state.st-2):256*(state.st+1);
        if(state.res==-1) {
            Lptr[prev_sh+x]=global_sh;
            shift[prev_sh+x]=8;
            //Lptr[prev_sh+x]=8+(global_sh<<16);
        } else {
            Lptr[prev_sh+x]=0;
            shift[prev_sh+x]=sh;
            //Lptr[prev_sh+x]=sh;
        }
	}
}

void formTrmd_inner2_rev(int L,ai_state a) {
uint sh,global_sh,prev_sh;
tabsrmd1 v;
ai_state state;
uchar res0=0;
    prev_sh=L?0x100000*((L/14)*3+a.st-2):0;
	for(uint x=0;x<0x100000;x++) {
		state=a;
		v.p1=v.p2=v.p3=v.p4=-1;
		/*sh=(L?0:3);
        for(uint k=L?1:8;k<0x100000 && state.res==-1;k<<=1,sh++)*/
        sh=0;
        for(uint k=1;k<0x100000 && state.res==-1;k<<=1,sh++)
			state=a_step1(state,x&k); // The function depends on code parameters
        if(state.res!=-1)
            global_sh=0;
        else
            global_sh=L?0x100000*((L/14)*3+state.st-2):0x100000*(state.st+1);
        if(state.res==-1) {
            Lptr[prev_sh+x]=global_sh;
            shift[prev_sh+x]=21;
            P[prev_sh+x]=state.p;
            //Lptr[prev_sh+x]=8+(global_sh<<16);
        } else {
            //Lptr[prev_sh+x]=0;
            shift[prev_sh+x]=sh;
 /*           if(L==0)
                cd_len[1][sh]++;*/
            P[prev_sh+x]=state.res;
            //Lptr[prev_sh+x]=sh;
        }

	}
}



void formTrmd_inner3_rev(int L,ai_state a) {
uint sh,global_sh,prev_sh;
tabsrmd1 v;
ai_state state;
uchar res0=0;
    prev_sh=L?tab_size*((L/reg_len)*3+a.st-2):0;
	for(uint x=0;x<tab_size;x++) {
		state=a;
		v.p1=v.p2=v.p3=v.p4=-1;
        sh=0;
        for(uint k=1;k<tab_size && state.res==-1;k<<=1,sh++)
			state=a_step1(state,x&k); // The function depends on code parameters
        if(state.res!=-1)
            global_sh=0;
        else
            global_sh=L?tab_size*((L/reg_len)*3+state.st-2):tab_size*(state.st+1);
        if(state.res==-1) {
            Lptr[prev_sh+x]=global_sh;
            shift[prev_sh+x]=reg_len+1;
            P[prev_sh+x]=state.p;
        } else {
            shift[prev_sh+x]=sh;
            P[prev_sh+x]=state.res;
        }

	}
}

void formTrmd_inner3(int L,ai_state state) {
tabsrmd1 v;
ai_state a=state;
	for(uint x=0;x<256;x++) {
		state=a;
		v.p1=v.p2=v.p3=v.p4=-1;
		for(uint k=128;k>0;k>>=1) {
			state=a_step1(state,x&k); // The function depends on code parameters
			if(state.res!=-1) {
				if(v.p1==-1)
					v.p1=state.res;
				else
					if(v.p2==-1)
						v.p2=state.res;
					else
						v.p3=state.res;
                v.n++;
			}
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
        experT3[(state_max*a.L+a.st)*256+x]=pack3(v,state);
	}
}

void formTrmd1() {
	state_max=3;
	for(int a=0;a<state_max;a++)
		for(int L=0;L<Lmax;L++) {
			ai_state state={a,L,0,-1};
			formTrmd_inner1(L,state);
		}
}

void formTrmd3() {
	state_max=3;
	for(int a=0;a<state_max;a++)
		for(int L=0;L<Lmax;L++) {
			ai_state state={a,L,0,-1};
			formTrmd_inner3(L,state);
		}
}

// Forms the table for fast decoding
void formTrmd() {
	for(int a=0;a<state_max;a++)    // All decoding automaton states
		for(int L=0;L<Lmax;L++) {   // All codeword lengths
			ai_state state={a,L,0,-1};
			formTrmd_inner(L,state);
		}
}

void formTrmd_rev() {
    for(int L=0;L<24;L+=8)   // All codeword lengths
        if(L)
            for(int a=0;a<state_max;a++) {   // All decoding automaton states
                ai_state state={a,L,0,-1};
                formTrmd_inner1_rev(L,state);
            }
        else {
            ai_state state={2,2,0,-1};
            formTrmd_inner1_rev(0,state);
        }
}

void formTrmd2_rev() {
    for(int L=0;L<28;L+=140)   // All codeword lengths
        if(L)
            for(int a=0;a<state_max;a++) {   // All decoding automaton states
                ai_state state={a,L+3,0,-1};
                formTrmd_inner2_rev(L,state);
            }
        else {
            ai_state state={2,3,0,-1};
            formTrmd_inner2_rev(0,state);
        }
}

void formTrmd3_rev() {
    for(int L=0;L<3*reg_len;L+=reg_len)   // All codeword lengths
        if(L)
            for(int a=0;a<state_max;a++) {   // All decoding automaton states
                ai_state state={a,L+3,0,-1};
                formTrmd_inner3_rev(L,state);
            }
        else {
            ai_state state={2,3,0,-1};
            formTrmd_inner3_rev(0,state);
        }
}

//======================= Fast decoding
// Forms the array of dictionary indices of words of a text
int decode_rmd_fast_orig() {
uint p=0,k=0,i=0,temp=0,x,y,numb=256,p1,p2,p3,p4;
uint real_len;
struct tabsrmd t={0,0,0,0,(struct tabsrmd*) Trmd+256}; // Decoding table element
	for(i=0;i<code_len;i++) {
		t=t.al[codes[i]];
		p+=t.p1;
		if(t.p2>=0) {
			outrmd[k++]=p;
			if(t.p3>=0) {
				outrmd[k++]=t.p2;
				if(t.p4>=0) {
					outrmd[k++]=t.p3;
					p=t.p4;
				} else
					p=t.p3;
			} else
				p=t.p2;
		}
	}
    return p;
}

int mulka_rmd_fast_orig() {
uint p=0,k=0,i=0,temp=0,x,y,numb=256,p1,p2,p3,p4;
uint real_len;
struct tabsrmd t={0,0,0,0,(struct tabsrmd*) Trmd+256}; // Decoding table element
	for(i=0;i<code_len;i++) {
		//real_len=((t.al-Trmd)%(256*Lmax))/256;
		t=t.al[codes[i]];
		p+=t.p1;
		if(t.p2>=0) {
			outrmd[k++]=p;
			if(t.p3>=0) {
				outrmd[k++]=t.p2;
				if(t.p4>=0) {
					outrmd[k++]=t.p3;
					p=t.p4;
				} else
					p=t.p3;
			} else
				p=t.p2;
		}
	}
    return p;
}

int decode_rmd_fast_3() {
uint p=0,k=0,i=0,temp=0,x,y,numb=256,p1,p2,p3,p4,tt,cds,ptr=0,out4;
//struct tabsrmd1 t={0,0,0,0,0,(struct tabsrmd1*) T235_1+256*11},*v1;
//unsigned char* u=(unsigned char*)codes,*end=(unsigned char*)codes+ilen,j=11;
int *w,*w1;
uint* e=(uint*)experT;
	for(i=0;i<code_len;i+=4) {
		cds=*(uint*)(codes+i);
		ptr=experT[ptr&0x7f][(uchar)cds];
		tt=ptr>>7;
		if(tt<0x4000)
			p+=tt;
		else {
			p+=tt&0x3fff;
			outrmd[k++]=p;
            //result+=Dict_rmd[p];
			tt>>=15;
			if(tt<128)
				p=tt;
			else {
                if(tt&256) {
                    outrmd[k++]=tt&0x7; //output 3-bit p2
                    //result+=Dict_rmd[tt&0x7];
                    out4=(tt>>3)&0xf;
                } else {
                    outrmd[k++]=tt&0x1f; //output 5-bit p2
                    //result+=Dict_rmd[tt&0x1f];
                    out4=(tt>>5)&0x3;
                }
				if(tt&512) {
                    outrmd[k++]=out4;
                    //result+=Dict_rmd[out4];
					p=0;
				} else
				    p=out4;
            }
		}

		ptr=experT[ptr&0x7f][(uchar)(cds>>8)];
		tt=ptr>>7;
		if(tt<0x4000)
			p+=tt;
		else {
			p+=tt&0x3fff;
			outrmd[k++]=p;
            //result+=Dict_rmd[p];
			tt>>=15;
			if(tt<128)
				p=tt;
			else {
                if(tt&256) {
                    outrmd[k++]=tt&0x7; //output 3-bit p2
                    //result+=Dict_rmd[tt&0x7];
                    out4=(tt>>3)&0xf;
                } else {
                    outrmd[k++]=tt&0x1f; //output 5-bit p2
                    //result+=Dict_rmd[tt&0x1f];
                    out4=(tt>>5)&0x3;
                }
				if(tt&512) {
                    outrmd[k++]=out4;
                    //result+=Dict_rmd[out4];
					p=0;
				} else
				    p=out4;
            }
		}
		ptr=experT[ptr&0x7f][(uchar)(cds>>16)];
		tt=ptr>>7;
		if(tt<0x4000)
			p+=tt;
		else {
			p+=tt&0x3fff;
			outrmd[k++]=p;
            //result+=Dict_rmd[p];
			tt>>=15;
			if(tt<128)
				p=tt;
			else {
                if(tt&256) {
                    outrmd[k++]=tt&0x7; //output 3-bit p2
                    //result+=Dict_rmd[tt&0x7];
                    out4=(tt>>3)&0xf;
                } else {
                    outrmd[k++]=tt&0x1f; //output 5-bit p2
                    //result+=Dict_rmd[tt&0x1f];
                    out4=(tt>>5)&0x3;
                }
				if(tt&512) {
                    outrmd[k++]=out4;
                    //result+=Dict_rmd[out4];
					p=0;
				} else
				    p=out4;
            }
		}

		ptr=experT[ptr&0x7f][(uchar)(cds>>24)];
		tt=ptr>>7;
		if(tt<0x4000)
			p+=tt;
		else {
			p+=tt&0x3fff;
			outrmd[k++]=p;
            //result+=Dict_rmd[p];
			tt>>=15;
			if(tt<128)
				p=tt;
			else {
                if(tt&256) {
                    outrmd[k++]=tt&0x7; //output 3-bit p2
                    //result+=Dict_rmd[tt&0x7];
                    out4=(tt>>3)&0xf;
                } else {
                    outrmd[k++]=tt&0x1f; //output 5-bit p2
                    //result+=Dict_rmd[tt&0x1f];
                    out4=(tt>>5)&0x3;
                }
				if(tt&512) {
                    outrmd[k++]=out4;
                    //result+=Dict_rmd[out4];
					p=0;
				} else
				    p=out4;
            }
		}


	}
	return p;
}

int decode_rmd_fast_1() {
uint p=0,k=0,i=0,temp=0,x,y,numb=256,p1,p2,p3,p4,tt,cds=0,ptr=0,out4;
//struct tabsrmd1 t={0,0,0,0,0,(struct tabsrmd1*) T235_1+256*11},*v1;
//unsigned char* u=(unsigned char*)codes,*end=(unsigned char*)codes+ilen,j=11;
int *w,*w1;
uint* e=(uint*)experT;
	for(i=0;i<code_len;i++) {
        ptr=experT[ptr&0x7f][*(codes+i)];
		tt=ptr>>7;
		if(tt<0x8000)
			p+=tt;
		else {
			p+=tt&0x3fff;
			outrmd[k++]=p;
            //result+=Dict_rmd[p];
			tt>>=15;
			if(tt<128)
				p=tt;
			else {
			    //totali++;
                if(tt&256) {
                    outrmd[k++]=tt&0x7; //output 3-bit p2
                    //result+=Dict_rmd[tt&0x7];
                    out4=(tt>>3)&0xf;
                } else {
                    outrmd[k++]=tt&0x1f; //output 5-bit p2
                    //result+=Dict_rmd[tt&0x1f];
                    out4=(tt>>5)&0x3;
                }
				if(tt&512) {
                    outrmd[k++]=out4;
                    //result+=Dict_rmd[out4];
					p=0;
				} else
				    p=out4;
            }
		}
	}
	return cds;
}

int decode_rmd_fast_2() {
uint p=0,k=0,i=0,temp=0,x,y,numb=256,p1,p2,p3,p4,tt,cds=0,ptr=0,out4;
//struct tabsrmd1 t={0,0,0,0,0,(struct tabsrmd1*) T235_1+256*11},*v1;
//unsigned char* u=(unsigned char*)codes,*end=(unsigned char*)codes+ilen,j=11;
int *w,*w1;
uint* e=(uint*)experT;
    totali=total=0;
	for(i=0;i<code_len;i++) {
        //totali++;
        ptr=experT[ptr&0x7f][*(codes+i)];
		tt=ptr>>7;
		//cds+=tt;
		if(tt&0x4000) { // 1 result
			outrmd[k++]=p+tt&0x3fff;
			p=(tt>>15);
		} else {
			if(tt&0x400000)
				p+=tt; // 0 results
			else {
                tt>>=15;
                if(tt&256) {
                    outrmd[k++]=tt&0x7; //output 3-bit p2
                    //result+=Dict_rmd[tt&0x7];
                    out4=(tt>>3)&0xf;
                } else {
                    outrmd[k++]=tt&0x1f; //output 5-bit p2
                    //result+=Dict_rmd[tt&0x1f];
                    out4=(tt>>5)&0x3;
                }
				if(tt&512) {
                    outrmd[k++]=out4;
                    //result+=Dict_rmd[out4];
					p=0;
				} else
				    p=out4;
            }
		}
	}
	return cds;
}

int decode_rmd_fast_2a(short unsigned int* out) {
uint p=0,k=0,i=0,i1=code_len/2,temp=0,x,y,numb=256,p1,p2,p3,p4,tt,cds=0,ptr=0,out4,fin=code_len-500;
//struct tabsrmd1 t={0,0,0,0,0,(struct tabsrmd1*) T235_1+256*11},*v1;
//unsigned char* u=(unsigned char*)codes,*end=(unsigned char*)codes+ilen,j=11;
    totali=total=0;
    for(i=0;i<code_len;i++) {
        ptr=experT[ptr&0x7f][*(codes+i)];
        tt=ptr>>7;
        cds=ptr&0xc0000000;
        if(!cds) {
            out[k++]=p+tt&0x3fff;
            p=(tt>>14);

        } else
            if(cds<0x80000000) {

                p+=tt;

            } else {

                if(cds<0xc0000000) {
                    out[k++]=p+tt&0x3fff;
                    out[k++]=(tt>>14)&0x1f;
                    p=(tt>>19)&0xf;
                } else {
                    out[k++]=p+tt&0x3fff;
                    out[k++]=(tt>>14)&0x1f;
                    out[k++]=(tt>>19)&0xf;
                    p=0;
        }
            }
    }
    return temp;
}



int mulka_rmd_fast_2a() {
uint k=0,i=0,i1=code_len/2,temp=0,x,y,numb=256,p1,p2,p3,p4,tt,cds=0,ptr=0,out4,tail=0;
uchar *u=randarr1;
ULONG64 t,p;
//struct tabsrmd1 t={0,0,0,0,0,(struct tabsrmd1*) T235_1+256*11},*v1;
//unsigned char* u=(unsigned char*)codes,*end=(unsigned char*)codes+ilen,j=11;
    totali=total=0;
    for(i=0;i<code_len;i+=8) {
        t=*((ULONG64*)(codes+i));
        for(int j=0;j<8;j++,t>>=8) {
            x=t&0xff;
            p=randarr_long[x];
            p+=(p&0xff)*randarr2[i]+tail;
            *(ULONG64*)(outrmd+i)=p;
            tail=randarr[x];
            i+=randarr1[x];
        }

    }
    return temp;
}

int mulka_rmd_fast_2b() {
uint p=0,k=0,i=0,i1=code_len/2,temp=0,x,y=0,numb=256,p1,p2,p3,p4,tt,cds=0,ptr=0,out4,fin=code_len-500,sh,j=3;
ULONG64 t;
    totali=total=0;
    for(i=0;i<code_len;i+=(j>>3)) {
        t=*((ULONG64*)(r_codes+i));
        j&=0x7;
        t>>=j;
        x=t&mask;
        sh=shift[x];
        t>>=sh;
        /*j+=sh;
        outrmd[k++]=P[x];
        x=t&mask;
        sh=shift[x];
        t>>=sh;*/
        for(j+=sh;j<64;t>>=sh) {
//            if(sh<=reg_len)
            while(sh<=reg_len) {
                outrmd[k++]=P[x];
                x=t&mask;
                sh=shift[x];
                j+=sh;
                t>>=sh;
            }
            if(j<sub_reg_len) {
                x=t&mask;
                sh=shift[x];
                j+=sh;
                t>>=sh;
            } else {
                sh+=reg_len;
                j+=reg_len;
            }

        }
        j-=sh;
    }
    return temp;
}

int mulka_rmd_fast_2e() {
uint p=0,k=0,i=0,i1=code_len/2,temp=0,x,y=0,numb=256,p1,p2,p3,p4,tt,cds=0,ptr=0,out4,fin=code_len-500,sh,j=3;
ULONG64 t;
    totali=total=0;
    for(i=0;i<code_len;i+=8) {
        t=*((ULONG64*)(r_codes+i));
        for(j=0;j<16;j++,t>>=2)
            temp=temp*3+(t&0x3);
    }
    return temp;
}

int mulka_rmd_fast_2c() {
uint p=0,k=0,i=0,i1=code_len/2,temp=0,x,y=0,numb=256,p1,p2,p3,p4,tt,cds=0,ptr=0,out4,fin=code_len-500,sh,j=3;
ULONG64 t;
    totali=0;
    for(i=0;i<code_len;i+=(j>>3)) {
        t=*((ULONG64*)(r_codes+i));
        j&=0x7;
        t>>=j;
        x=t&mask;
        sh=shift[x];
        //cd_len[1][sh]++;
        t>>=sh;
        for(j+=sh;j<64;t>>=sh) {
            outrmd[k++]=P[x];
            x=t&mask;
            sh=shift[x];
            /*cd_len[1][sh]++;
            if(sh==14)
                totali++;
            if(i%100000==0)
                cout<<i<<" ";*/
            j+=sh;
        }
        j-=sh;
        //cd_len[1][sh]--;
        //if(i%100==0)
            //cout<<i<<" ";
        /*for(int z=k-5>=0?k-5:0;z<k;z++)
            cout<<"z="<<z<<": "<<ranks[z]<<" "<<outrmd[z]<<endl;*/
    }
    return temp;
}

int mulka_rmd_fast_2c1() {
uint p=0,k=0,i=0,temp=0,x,y=0,numb=256,p1,p2,p3,p4,tt,cds=0,ptr=0,out4,fin=code_len-500,sh=3;
ULONG64 t;
    totali=0;
    for(i=0;i<code_len;i+=(sh>>3)) {
        t=*((ULONG64*)(r_codes+i));
        sh&=0x7;
        //t>>=j;
        x=(t>>sh)&mask;
        sh=shift[x];
        //cd_len[1][sh]++;
        //t>>=sh;
        while(sh<64) {
            outrmd[k++]=P[x];
            x=(t>>sh)&mask;
            sh+=(y=shift[x]);
            /*cd_len[1][sh]++;
            if(sh==14)
                totali++;
            if(i%100000==0)
                cout<<i<<" ";
            j+=sh;*/
        }
        sh-=y;
        //sh-=shift[x];
        //cd_len[1][sh]--;
        //if(i%100==0)
            //cout<<i<<" ";
        /*for(int z=k-5>=0?k-5:0;z<k;z++)
            cout<<"z="<<z<<": "<<ranks[z]<<" "<<outrmd[z]<<endl;*/
    }
    return temp;
}

int mulka_rmd_fast_2d() {
uint p=0,k=0,i=0,i1=code_len/2,temp=0,x,y=0,numb=256,p1,p2,p3,p4,tt,cds=0,ptr=0,out4,fin=code_len-500,sh,j;
ULONG64 t;
    total=0;
    for(i=0;i<code_len;i+=8) {
        t=*((ULONG64*)(r_codes+i));
        for(j=0;j<64;t>>=sh) {
            x=temp+(t&0x3fff);
            temp=Lptr[x];
            /*sh=temp&0xffff;
            temp>>=16;*/
            sh=shift[x];
            j+=sh;
         //if(!temp)
            outrmd[k++]=P[x];
           // }
            /*if(sh)
                outrmd[y++]=x;*/
        }

    }
    return temp;
}


int decode_rmd_fast_2am() {
uint p=0,k=0,i=0,i1=code_len/2,temp=0,x,y,numb=256,p1,p2,p3,p4,tt,cds=0,ptr=0,out4,fin=code_len-500;
//struct tabsrmd1 t={0,0,0,0,0,(struct tabsrmd1*) T235_1+256*11},*v1;
//unsigned char* u=(unsigned char*)codes,*end=(unsigned char*)codes+ilen,j=11;
    totali=total=0;
    for(i=0;i<code_len;i++) {
    //while(true) {
        //freqs[ptr&0x7f][*(codes+i)]++;
        ptr=experT[ptr&0x7f][*(codes+i)];
        tt=ptr>>7;
        cds=ptr&0xc0000000;
        if(!cds) {
            outrmd[k++]=p+tt&0x3fff;
            p=(tt>>14);

        }
            if(cds==0x40000000) {

                p+=tt;
            }

                if(cds==0x80000000) {
                    if(i>code_len-100)
                        outrmd[k++]=1;
                    outrmd[k++]=p+tt&0x3fff;
                    outrmd[k++]=(tt>>14)&0x1f;
                    p=(tt>>19)&0xf;
                }
                if(cds==0xc0000000) {

                    outrmd[k++]=p+tt&0x3fff;
                    outrmd[k++]=(tt>>14)&0x1f;
                    outrmd[k++]=(tt>>19)&0xf;
                    p=0;
        }

    }
    return 0;
}

int decode_rmd_fast_3a() {
uint p=0,k=0,i=0,temp=0,x,y,numb=256,p1,p2,p3,p4,tt,cds,ptr=0,out4;
//struct tabsrmd1 t={0,0,0,0,0,(struct tabsrmd1*) T235_1+256*11},*v1;
//unsigned char* u=(unsigned char*)codes,*end=(unsigned char*)codes+ilen,j=11;
int *w,*w1;
	for(i=0;i<code_len;i+=4) {
		cds=*(uint*)(codes+i);
		ptr=experT[ptr&0x7f][(uchar)cds];
		tt=ptr>>7;
        y=ptr&0xc0000000;
        if(!y) {
            outrmd[k++]=p+tt&0x3fff;
            p=(tt>>14);
        } else
            if(y<0x80000000)
                p+=tt;
            else
                if(y<0xc0000000) {
                    if(i>code_len-100)
                        outrmd[k++]=1;
                    outrmd[k++]=p+tt&0x3fff;
                    outrmd[k++]=(tt>>14)&0x1f;
                    p=(tt>>19)&0xf;
                } else {
                    /*if(i>fin)
                        break;*/
                    outrmd[k++]=p+tt&0x3fff;
                    outrmd[k++]=(tt>>14)&0x1f;
                    outrmd[k++]=(tt>>19)&0xf;
                    p=0;
                }


		ptr=experT[ptr&0x7f][(uchar)(cds>>8)];
		tt=ptr>>7;
		y=ptr&0xc0000000;
        if(!y) {
            outrmd[k++]=p+tt&0x3fff;
            p=(tt>>14);
        } else
            if(y<0x80000000)
                p+=tt;
            else
                if(y<0xc0000000) {
                    if(i>code_len-100)
                        outrmd[k++]=1;
                    outrmd[k++]=p+tt&0x3fff;
                    outrmd[k++]=(tt>>14)&0x1f;
                    p=(tt>>19)&0xf;
                } else {
                    /*if(i>fin)
                        break;*/
                    outrmd[k++]=p+tt&0x3fff;
                    outrmd[k++]=(tt>>14)&0x1f;
                    outrmd[k++]=(tt>>19)&0xf;
                    p=0;
                }
		ptr=experT[ptr&0x7f][(uchar)(cds>>16)];
		tt=ptr>>7;
		y=ptr&0xc0000000;
        if(!y) {
            outrmd[k++]=p+tt&0x3fff;
            p=(tt>>14);
        } else
            if(y<0x80000000)
                p+=tt;
            else
                if(y<0xc0000000) {
                    if(i>code_len-100)
                        outrmd[k++]=1;
                    outrmd[k++]=p+tt&0x3fff;
                    outrmd[k++]=(tt>>14)&0x1f;
                    p=(tt>>19)&0xf;
                } else {
                    /*if(i>fin)
                        break;*/
                    outrmd[k++]=p+tt&0x3fff;
                    outrmd[k++]=(tt>>14)&0x1f;
                    outrmd[k++]=(tt>>19)&0xf;
                    p=0;
                }

		ptr=experT[ptr&0x7f][(uchar)(cds>>24)];
		tt=ptr>>7;
		y=ptr&0xc0000000;
        if(!y) {
            outrmd[k++]=p+tt&0x3fff;
            p=(tt>>14);
        } else
            if(y<0x80000000)
                p+=tt;
            else
                if(y<0xc0000000) {
                    if(i>code_len-100)
                        outrmd[k++]=1;
                    outrmd[k++]=p+tt&0x3fff;
                    outrmd[k++]=(tt>>14)&0x1f;
                    p=(tt>>19)&0xf;
                } else {
                    /*if(i>fin)
                        break;*/
                    outrmd[k++]=p+tt&0x3fff;
                    outrmd[k++]=(tt>>14)&0x1f;
                    outrmd[k++]=(tt>>19)&0xf;
                    p=0;
                }


	}
	return p;
}


int decode_rmd_fast_2b() {
uint p=0,k=0,i=0,temp=0,x,y,numb=256,p1,p2,p3,p4,tt,cds=0,out4;
struct tabs_packed *ptr=(tabs_packed *)experT3,str;
//struct tabsrmd1 t={0,0,0,0,0,(struct tabsrmd1*) T235_1+256*11},*v1;
//unsigned char* u=(unsigned char*)codes,*end=(unsigned char*)codes+ilen,j=11;
int *w,*w1;
//    totali=totalh=0;
    for(i=0;i<code_len;i++) {
        //freqs[ptr&0x7f][*(codes+i)]++;
        str=ptr[*(codes+i)];
        ptr=str.ptr;
        if(!str.flag) {
            outrmd[k++]=p+str.p1;
            p=str.p2p3;
        } else
            if(str.flag<2)
                p+=str.p1;
            else
                if(str.flag<3) {
                    outrmd[k++]=p+str.p1;
                    outrmd[k++]=str.p2p3&0x1f;
                    p=(str.p2p3>>5)&0xf;
                } else {
                    outrmd[k++]=p+str.p1;
                    outrmd[k++]=str.p2p3&0x1f;
                    outrmd[k++]=(str.p2p3>>5)&0xf;
                    p=0;
        }
	}
	return 0;
}


int k[20]={0,1,21}; //numbers which are not delimiters, the last = biggest delimiter+1
int kmax=3,Lmin=3; //number of elements in the array k; length of the shortest codeword

// 1) Generating the Reverse Multi-Delimiter codewords in rmd array. 2) Filling nlk array for the decoding.
// The parameters m1,m2,... of a code are all integers not present in the array k
void gen_reverse() {
int L,n,i,j,L1_long=-1,L1_long_prev=-1,t=2;
uint seq=0,s=0;
	for(L=3,n=0;L<Lmax+8 && n<=max_rmd;L++) {
		nl[L]=n;
		for(i=0;L-k[i]>Lmin && i<kmax;i++) {
			if(i==kmax-1 && L1_long==-1)
				L1_long=n;
			nlk[L][i]=n-nl[L-k[i]-1];
			for(j=nl[L-k[i]-1];j<nl[L-k[i]] && n<=max_rmd;j++,n++) {
				seq=0xFFFF>>(16-k[i]);
				rmd[n]=(rmd[j]<<(k[i]+1))|seq;
			}
		}
		if(L1_long_prev>-1) {
			nlk[L][kmax]=n-L1_long_prev;
			for(j=L1_long_prev;j<nl[L] && n<=max_rmd;j++,n++)
				rmd[n]=(rmd[j]<<1)|1;
		}
		if(t<kmax && L-1!=k[t]) {
			rmd[n]=(1<<(L-1))-1;
			n++;
		} else
			t++;
		L1_long_prev=L1_long;
		L1_long=-1;
	}
	seq=1;
}

void write_to_file_rmd(int N,unsigned short* indices,char* fname) {
string word;
	ofstream file(fname),f("frequences.txt");
	for(int i=1;i<N;i++)
		file<<Dict_rmd[indices[i]]<<" ";
	file.close();
   /* for(int i=0;i<256;i++) {
        for(int j=0;j<256;j++)
            f<<freqs[i][j]<<" ";
        f<<endl;
    }
    f.close();*/
}

void write_to_file_rmd_long(int N,unsigned int* indices,char* fname) {
string word;
	ofstream file(fname),f("frequences.txt");
	for(int i=1;i<N;i++)
		file<<Dict_rmd[indices[i]]<<" ";
	file.close();
}

// Prints top 100 codewords of a code
void code_output(int n) {
  #define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
  #define BYTE_TO_BINARY(byte)  \
  (byte & 0x800000 ? '1' : '0'), \
  (byte & 0x400000 ? '1' : '0'), \
  (byte & 0x200000 ? '1' : '0'), \
  (byte & 0x100000 ? '1' : '0'), \
  (byte & 0x080000 ? '1' : '0'), \
  (byte & 0x040000 ? '1' : '0'), \
  (byte & 0x020000 ? '1' : '0'), \
  (byte & 0x010000 ? '1' : '0'), \
  (byte & 0x8000 ? '1' : '0'), \
  (byte & 0x4000 ? '1' : '0'), \
  (byte & 0x2000 ? '1' : '0'), \
  (byte & 0x1000 ? '1' : '0'), \
  (byte & 0x0800 ? '1' : '0'), \
  (byte & 0x0400 ? '1' : '0'), \
  (byte & 0x0200 ? '1' : '0'), \
  (byte & 0x0100 ? '1' : '0'), \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

    FILE* file=fopen("code.txt","w");
    for(int i=0;i<n;i++)
        //fprintf(file,"\n%d="BYTE_TO_BINARY_PATTERN,i,BYTE_TO_BINARY(rmd[i]));
        fprintf(file,"\n%d="BYTE_TO_BINARY_PATTERN,i,BYTE_TO_BINARY(ternary_code[i]));
    fclose(file);
}


void form_ternary() {
int i=0;
    for(int k=1,t=2;k<99000;k*=3,t+=2) {
        for(int j=0;j<k;j++)
            ternary[i++]=t;
    }

}

void createEncodedFile(string t, int ter_byte) {
    FILE* out = fopen(("ter"+t+".bmp").c_str(), "wb");
    for (int i = 0; i < ter_byte; i++){
        fputc(coded_t[i], out);
    }
    fclose(out);
}

int main() {
    unsigned int iter=10,t,i;
    double sums=0,sumi=0;
    generate_ternary(50000);
    for(int i=0;i<256;i++) {
        randarr_long[i]=rand();
        randarr[i]=rand()%2000;
        randarr1[i]=rand()%3;
        randarr2[i]=rand()%900;
    }

    cout << sums;
    gen_reverse(); //Generating the array of the Reverse MD-code codewords
    //code_output(50000);
    string num = "13";

    Image image;

    // direct way
    image.createBinaryCodeDiffs(num);
    Nwords=word_frequences(image); //Mapping the words of a text into integers, according to descending order of frequences
    t=new_code_size();
    cout<<"New code size"<<(float)(t)/Nwords<<endl;

    text_to_ranks(rmd_map_sorted,image,ranks); //Mapping the text into integers, according to word map (rmd_map_sorted)

    // snake
    /*image.getBytes(num);
    image.createBinaryCodeSnake();
    image.putToTxtSnake(num);
    Nwords=word_frequences(image); //Mapping the words of a text into integers, according to descending order of frequences
    t=new_code_size();
    cout<<"New code size"<<(float)(t)/Nwords<<endl;*/

    text_to_ranks(rmd_map_sorted,image,ranks); //Mapping the text into integers, according to word map (rmd_map_sorted)

    // rmd
    encode_rmd(Nwords,ranks); //Creates the Reverse MD-code in 'codes' array
    cout<<Nwords<<" words in text; "<<diff_words<<" different words\n"<<"Size of RMD code in bytes: "<<
    cur_byte<<", RMD average codeword length="<<(float)cur_byte*8/Nwords<<"; entropy="<<entropy<<"\n";


    int ter_byte=encode_ternary(Nwords,ranks); //Creates the Reverse MD-code in 'codes' array
    cout<<"Size of Ternary code in bytes: "<<ter_byte<<", Ternary average codeword length="<<(float)ter_byte*8/Nwords<<"; entropy="<<entropy<<"\n";
    createEncodedFile(num, ter_byte);
    //revert_bytes();
    //output the Reverse MD-code into file
    /*FILE *out_file=fopen("output.txt","wb");
    fwrite(codes,1,code_len,out_file);*/


//================ SCDC/RPBC codes ================
//    create_tables(); //Create tables for RPBC codes

    int scdc_L;
   /* for(int i=10;i<242;i++) {
        v1=i;
        v2=250-v1;
        v3=5;*/
//        scdc_L=rpbc_encode(Nwords,ranks,codes_scdc);
 //       cout<<"v1="<<v1<<" scdc_L="<<scdc_L<<" SCDC av codeword length: "<<(float)scdc_L*8/Nwords<<"\n";
    //}

//    cout<<" scdc_L= "<<scdc_L<<" SCDC av codeword length: "<<(float)scdc_L*8/Nwords<<"\n";

//================ Decoding ===============
/*    QueryPerformanceFrequency(&tfreq); // time count
    //formTrmd(); // Create decoding table
    //formTrmd3_rev();
    formTrmd1();
    cout<<"Decoding table prepared.";
    cout<<"code_len="<<code_len<<endl;
	for(int i=0;i<iter;i++) {
        QueryPerformanceCounter(&tstart);   // time count
            totali=rpbc_decode(codes_scdc,decoded_scdc,scdc_L);
            //mulka_rmd_fast_2b(); // decoding
            //decode_rmd_fast_2a();
        QueryPerformanceCounter(&tend); // time count
        sums+=(double)(tend.QuadPart - tstart.QuadPart)/tfreq.QuadPart; // time count
        QueryPerformanceCounter(&tstart);   // time count
            total=decode_ternary_long(coded_t,outrmd_long,ter_byte);
            for(i=0;i<ter_byte;i++) {
          //      cout<<ranks[i]<<endl;
          /*      if(ranks[i]!=outrmd_long[i])
                    cout<<"!!!!!!!!-"<<ranks[i]<<" "<<ranks[i+1]<<" "<<ranks[i+2];*/

                   //system("pause");
            }
            //total=decode_ternary8(coded_t,outrmd,ter_byte);
           // decode_rmd_fast_2a(outrmd);
/*        QueryPerformanceCounter(&tend); // time count
        sumi+=(double)(tend.QuadPart - tstart.QuadPart)/tfreq.QuadPart; // time count
		//decode_rmd_fast_orig();  // decoding

    }

   /* for(int i=0;i<30;i++)
        cout<<i<<": "<<cd_len[0][i]<<" "<<cd_len[1][i]<<endl;*/

    /*for(int j=0;j<Nwords;j++)
            if(outrmd[j+1]!=ranks[j]) {
                cout<<" j="<<j;
                system("pause");
            }*/
    // sumi=(float)(tend.QuadPart - tstart.QuadPart)/tfreq.QuadPart; // time count
    //write_to_file_rmd(Nwords,outrmd,"output.txt");
/*    write_to_file_rmd_long(Nwords,outrmd_long,"output.txt");
	cout<<" Ternary 10: "<<sumi/iter<<"; RPBC:"<<sums/iter<<" Ratio:"<<(double)sumi/sums<<"\n";
	cout<<"totali="<<totali<<" totalh="<<total;
	system("pause");
}


		/*for(int j=0;j<Nwords;j++)
            if(outrmd[j+1]!=ranks[j]) {
                cout<<" j="<<j;
                system("pause");
            }*/
