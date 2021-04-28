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

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;

extern const int L=30000000;
extern unsigned short outrmd[L*2];
extern unsigned int outrmd_long[L*2];

int cur_t_byte,cur_t_bit;
uint ternary_code[1000000],segments[20];
uchar coded_t[L];


struct ternary {
    ULONG64 out_numbers;
    uchar n,mult;
    unsigned short transfer,flag;
};

const int Tbyte_size=8,Tsize=1<<Tbyte_size,cws16=32,Tshort_size=4,Tshort=1<<Tshort_size,Tmask=Tsize-1;
struct ternary ternary_decT[Tsize];

ULONG64 _numbers[Tsize];
uint _numbers4[Tshort],long_flag[Tsize];
uchar _n[Tsize],_mult4[Tshort];
ushort _transfer[Tsize],_transfer4[Tshort],_flag[Tsize],_mult[Tsize];


void form_ter_table() {
ULONG64 p;
int numb,k,k_prev;
uint x,y;
    for(uint y=0;y<Tsize;y++){
        x=y;
        p=numb=0;
        _numbers[y]=_n[y]=_transfer[y]=0; _flag[y]=0xffff; long_flag[y]=0xffffff;
        k=1; k_prev=0;
        for(int i=0;i<Tbyte_size;i+=2,k=k_prev?k:(k?k*3:1),x>>=2) {
            if((x&0x3)==3) {
                _numbers[y]|=(p<<(numb*cws16));
                numb++;
                p=0;
                _flag[y]=long_flag[y]=0;
                k_prev=1;
            } else
                p=p*3+(x&0x3)+1;
        }
        _n[y]=numb;
        _transfer[y]=p;
        _mult[y]=k;
    }
}

void form_ter_table4() {
ULONG64 p;
int numb,k,k_prev;
uint x,y;
    for(uint y=0;y<Tshort;y++){
        x=y;
        p=numb=0;
        _numbers4[y]=_transfer4[y]=0;
        k=1; k_prev=0;
        for(int i=0;i<Tshort_size;i+=2,k=k_prev?k:(k?k*3:1),x>>=2) {
            if((x&0x3)==3) {
                _numbers4[y]|=(p<<(numb*cws16));
                numb++;
                p=0;
                k_prev=1;
            } else
                p=p*3+(x&0x3)+1;
        }
        _transfer4[y]=p;
        _mult4[y]=k;
    }
}

void recursive_ternary(int cur,int cur3, int limit) {
    if(cur<limit) {
        for(int i=0;i<3;i++)
            for(int j=0;j<cur3;j++)
                ternary_code[cur+cur3*i+j]=(ternary_code[cur-cur3+j]<<2)|i;
        recursive_ternary(cur+cur3*3,cur3*3,limit);
    }
}

void generate_ternary(int limit) {
    ternary_code[0]=3;
    recursive_ternary(1,1,limit);
    form_ter_table();
    form_ter_table4();
}

void bit_step() {
        if(cur_t_bit<6)
            cur_t_bit+=2;
        else {
            cur_t_bit=0;
            cur_t_byte++;
        }

}

/*void flush_to_byte_ternary(uint x) {
int i,j;
    for(i=-2,j=3;(x&j)!=j;j<<=2,i+=2);
    while(i>=0) {
        coded_t[cur_t_byte]|=((x>>i)&3)<<cur_t_bit;
        bit_step();
        i-=2;
    }
    coded_t[cur_t_byte]|=3<<cur_t_bit;
    bit_step();
}*/

void flush_to_byte_ternary(uint x) {
int i,j;
    //for(i=-2,j=3;(x&j)!=j;j<<=2,i+=2);
    while((x&3)!=3) {
        coded_t[cur_t_byte]|=(x&3)<<cur_t_bit;
        bit_step();
        x>>=2;
    }
    coded_t[cur_t_byte]|=3<<cur_t_bit;
    bit_step();
}

/*void revert_bytes_ternary(int max_rank){
uchar t,res,j;
int k;
	for(int i=0;i<max_rank;i++) {
	    t=coded_t[i];
	    coded_t[i]=((t&0xc0)>>6)+((t&0x30)>>2)+((t&0xc)<<2)+((t&0x3)<<6);
	}
}*/

int encode_ternary(int max_rank,uint* ranks) {
int i;
    for(i=0;i<L;i+=8)
        *(ULONG64*)(coded_t+i)=0;
    cur_t_byte=cur_t_bit=0;
	for(i=0;i<max_rank;i++) {
        if(i==123)
            i=123;
		flush_to_byte_ternary(ternary_code[ranks[i]]); // write current codeword to the end of a code bitstream
	}
	//revert_bytes_ternary(max_rank);
	return cur_t_byte;
}

int decode_ternary(uchar* code, unsigned short* out,int Len) {
ULONG64 t;
ushort M[4]={1,3,9,27};
int temp=0,k=0,l,tr=0;
struct ternary s;
unsigned short x,z;
//uchar flag;
unsigned short u;
    for(int i=0;i<Len;i+=8) {
        t=*((ULONG64*)(code+i));
        for(int j=0;j<7;j++,t>>=Tbyte_size) {
            x=t&0x3ff;
            s=ternary_decT[x];
            z=tr*s.mult;
            *(ULONG64*)(out+k)=s.out_numbers+z;
            k+=s.n;
            tr=s.transfer+(z&s.flag);
        }
    }
    return temp;
}

int decode_ternary1(uchar* code, unsigned short* out,int Len) {
ULONG64 t;
//unsigned __int128 t;
ushort M[4]={1,3,9,27};
int temp=0,k=0,l,tr=0;
//struct ternary s;
unsigned short x,z;
//uchar flag;
unsigned short u;
    for(int i=0;i<Len;i+=8) {
        t=*((ULONG64*)(code+i));
        for(int j=0;j<5;j++,t>>=Tbyte_size) {
            x=t&Tmask;
            z=tr*_mult[x];
            *(ULONG64*)(out+k)=_numbers[x]+z;//+u;
            k+=_n[x];
            tr=_transfer[x]+(z&_flag[x]);
        }
        z=tr*_mult4[t];
        *(uint*)(out+k)=_numbers4[t]+z;//+u;
        k+=_n[t];
        tr=_transfer4[t]+(z&_flag[t]);
    }
    return temp;
}

int decode_ternary_long(uchar* code, unsigned int* out,int Len) {
ULONG64 t;
//unsigned __int128 t;
ushort M[4]={1,3,9,27};
int temp=0,k=0,l,tr=0;
//struct ternary s;
unsigned int x,z;
//uchar flag;
unsigned short u;
    for(int i=0;i<Len;i+=8) {
        t=*((ULONG64*)(code+i));
        for(int j=0;j<8;j++,t>>=Tbyte_size) {
            x=t&Tmask;
            z=tr*_mult[x];
            /*if(k>122)
                temp=1;*/
            *(ULONG64*)(out+k)=_numbers[x]+z;//+u;
            k+=_n[x];
            tr=_transfer[x]+(z&long_flag[x]);
        }
    }
    return temp;
}

int decode_ternary8(uchar* code, unsigned short* out,int Len) {
ULONG64 t;
//unsigned __int128 t;
ushort M[4]={1,3,9,27};
int temp=0,k=0,l,tr=0;
//struct ternary s;
unsigned short x,z;
//uchar flag;
unsigned short u;
    for(int i=0;i<Len;i+=8) {
        t=*((ULONG64*)(code+i));
        for(int j=0;j<8;j++,t>>=Tbyte_size) {
            x=t&Tmask;
            z=tr*_mult[x];
            *(ULONG64*)(out+k)=_numbers[x]+z;//+u;
/*            if(_n[x]>2)
                temp=2;*/
            k+=_n[x];
            tr=_transfer[x]+(z&_flag[x]);
        }
    }
    return temp;
}

int decode_ternary16(uchar* code, unsigned short* out,int Len) {
ULONG64 t;
//unsigned __int128 t;
ushort M[4]={1,3,9,27};
int temp=0,k=0,l,tr=0;
//struct ternary s;
unsigned short x,z;
//uchar flag;
unsigned short u;
    for(int i=0;i<Len;i+=8) {
        t=*((ULONG64*)(code+i));
        for(int j=0;j<4;j++,t>>=Tbyte_size) {
            x=t&Tmask;
            z=tr*_mult[x];
            *(ULONG64*)(out+k)=_numbers[x]+z;//+u;
            k+=_n[x];
            tr=_transfer[x]+(z&_flag[x]);
        }
    }
    return temp;
}

    /*for(i=0;i<code_len;i+=8) {
        t=*((ULONG64*)(codes+i));
        for(int j=0;j<8;j++,t>>=8) {
            x=t&0xff;
            p=randarr_long[x];
            p+=(p&0xff)*randarr2[i]+tail;
            *(ULONG64*)(outrmd+i)=p;
            tail=randarr[x];
            i+=randarr1[x];
        }

    }*/

