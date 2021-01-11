#include<iostream>
#include<cstdio>
#include "image.h"
#include <bitset>
using namespace std;

// using diffs
void image::putToTxtDiffs(int byte, FILE* txt){
    char sign = byte >=0 ? '0' : '1';
    fputc(sign, txt);
    byte = abs(byte);
    for(int i=0;i<8;i++) {
            fputc((byte&(1<<(7-i)) ? 1 : 0) + '0', txt);
        }
    fputc(' ', txt);
}

void image::createBinaryCodeDiffs(string t) {
  FILE *image;
  FILE *txt;
  int byte, am = 0;
  image = fopen((t+".bmp").c_str(),"rb");
  txt = fopen(("out"+t+".txt").c_str(), "w");

  if (image != nullptr) {
    byte = 0;
    while (byte != EOF && am < shift)
    {
        byte = fgetc(image);
        am++;
        putToTxtDiffs(byte, txt);
    }
    //int r[MAX_SIZE], g[MAX_SIZE], b[MAX_SIZE];
    int r=fgetc(image),g=fgetc(image), b=fgetc(image);
    putToTxtDiffs(r, txt);
    putToTxtDiffs(g, txt);
    putToTxtDiffs(b, txt);
    int r1= fgetc(image), b1=fgetc(image), g1=fgetc(image);
    while (!(r1 == EOF && g1== EOF && b1==EOF)) {
        if (r1 != EOF)
            putToTxtDiffs(r1 - r, txt);
        if (g1 != EOF)
            putToTxtDiffs(g1 - g, txt);
        if (b1 != EOF)
            putToTxtDiffs(b1 - b, txt);
        r = r1, b = b1, g = g1;
        r1 = fgetc(image);
        g1 = fgetc(image);
        b1 = fgetc(image);
    }
  }
  else {
    cout << "Cannot open file\n";
  }
  fclose(image);
  fclose(txt);
}

int image::byteFromTextDiffs(int* text, int num) {
    int result=0;
      for(int i=1;i<9;i++)
      {
        if(text[i]=='1')
        {
          result |= (1<< (8-i) );
        }
      }
      if (text[0] == '1')
        result *= (-1);
      return result + num;
}

void image::createImageDiffs(string t) {
    FILE* txt = fopen(("out"+t+".txt").c_str(), "r");
    FILE* image = fopen(("res"+t+".bmp").c_str(), "wb");
    int c = 0, r[9] = {}, g[9] = {}, b[9] = {}, num[3] = {0,0,0};
    if (txt != nullptr) {
        int am = 0;
        while(c != EOF && am < shift) {
            for (int j = 0; j < 9; j++){
                c = fgetc(txt);
                if (c == EOF)
                    break;
                r[j] = c;
            }
            c = fgetc(txt); // get space symbol
            fputc(byteFromTextDiffs(r, 0), image);
            am++;
        }
        am = 0;
        while (am < 3 && c != EOF) {
            for (int j = 0; j < 9; j++){
                c = fgetc(txt);
                if (c == EOF)
                    break;
                r[j] = c;
            }
            c = fgetc(txt); // get space symbol
            fputc(byteFromTextDiffs(r, 0), image);
            num[am] = byteFromTextDiffs(r, 0);
            am++;
        }
        cout << num[0] << num[1] << num[2] << endl;
        while (c != EOF) {
            for (int j = 0; j < 9; j++){
                c = fgetc(txt);
                if (c == EOF)
                    break;
                r[j] = c;
            }
            c = fgetc(txt); // get space symbol
            for (int j = 0; j < 9; j++){
                c = fgetc(txt);
                if (c == EOF)
                    break;
                g[j] = c;
            }
            c = fgetc(txt); // get space symbol
            for (int j = 0; j < 9; j++){
                c = fgetc(txt);
                if (c == EOF)
                    break;
                b[j] = c;
            }
            c = fgetc(txt); // get space symbol
            if (c == EOF)
                    break;
            num[0] = byteFromTextDiffs(r, num[0]);
            num[1] = byteFromTextDiffs(g, num[1]);
            num[2] = byteFromTextDiffs(b, num[2]);
            //cout << num[0] <<' ' << num[1] << ' ' << num[2]<<' ';
            fputc(num[0], image);
            fputc(num[1], image);
            fputc(num[2], image);
        }
    }
    fclose(image);
    fclose(txt);
}

void image::createBinaryCode(string t) {
FILE *image;
  FILE *txt;
  int byte, am = 0;
  image = fopen((t+".bmp").c_str(),"rb");
  txt = fopen(("out"+t+".txt").c_str(), "w");

  if (image != nullptr) {
    byte = fgetc(image);
    while (byte != EOF)
    {
        putToTxt(byte, txt);
        byte = fgetc(image);
    }
  }
  else {
    cout << "Cannot open file\n";
  }
  fclose(image);
  fclose(txt);
}
void image::createImage(string t) {
    FILE* txt = fopen(("out"+t+".txt").c_str(), "r");
    FILE* image = fopen(("res"+t+".bmp").c_str(), "wb");
    int sym[8], c;
    if (txt != nullptr) {
        for (int j = 0; j < 8; j++){
            c = fgetc(txt);
            if (c == EOF)
                break;
            sym[j] = c;
        }
        c = fgetc(txt);
        while (c != EOF) {
            fputc(byteFromText(sym), image);
            for (int j = 0; j < 8; j++){
                c = fgetc(txt);
                if (c == EOF)
                    break;
                sym[j] = c;
            }
            c = fgetc(txt);
        }
    }
    fclose(image);
    fclose(txt);
}

int image::byteFromText(int* text) {
    int result=0;
      for(int i=0;i<8;i++)
      {
        if(text[i]=='1')
        {
          result |= (1<< (7-i) );
        }
      }
      return result;
}
void image::putToTxt(int byte, FILE* txt){
    for(int i=0;i<8;i++) {
            fputc((byte&(1<<(7-i)) ? 1 : 0) + '0', txt);
        }
    fputc(' ', txt);
}
