#include<iostream>
#include<cstdio>
#include "image.h"
#include <bitset>
using namespace std;

// standard
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

// using diffs
void image::putToTxtDiffs(int byte, FILE* txt, bool space){
    char sign = byte >=0 ? '0' : '1';
    fputc(sign, txt);
    byte = abs(byte);
    for(int i=0;i<8;i++) {
            fputc((byte&(1<<(7-i)) ? 1 : 0) + '0', txt);
        }
    if (space)
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
        //putToTxtDiffs(byte, txt, false); // put header to file as one codeword
    }
    int r=fgetc(image),g=fgetc(image), b=fgetc(image);
    putToTxtDiffs(r, txt, true);
    putToTxtDiffs(g, txt, true);
    putToTxtDiffs(b, txt, true);
    int r1= fgetc(image), b1=fgetc(image), g1=fgetc(image);
    while (!(r1 == EOF && g1== EOF && b1==EOF)) {
        if (r1 != EOF)
            putToTxtDiffs(r1 - r, txt, true);
        if (g1 != EOF)
            putToTxtDiffs(g1 - g, txt, true);
        if (b1 != EOF)
            putToTxtDiffs(b1 - b, txt, true);
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

// snake
void image::getBytes(string t) {
    FILE *image;
    FILE *txt;
    int byte, am = 0;// width and height of image
    image = fopen((t+".bmp").c_str(),"rb");

    if (image != nullptr) {
        byte = 0, w = 0, h = 0;
        while (byte != EOF && am < shift)
        {
            byte = fgetc(image);
            header.push_back(byte);
            if (am > 17 && am < 22) {
                w += (byte << 8*(am-18));
            }
            if (am > 21 && am < 26) {
                h += (byte << 8*(am-22));
            }
            am++;
        }
        cout << "width " << w << " height " << h << endl;
        while (byte != EOF) {
            r.push_back(byte);
            byte = fgetc(image);
            g.push_back(byte);
            byte = fgetc(image);
            b.push_back(byte);
            byte = fgetc(image);
        }
        int k=0;

    }
    else {
        cout << "Cannot open file\n";
    }
    fclose(image);
}

void image::createBinaryCodeSnake() {
    rv.resize(h*w);
    gv.resize(h*w);
    bv.resize(h*w);
    int d = 0;
    for (int i=0;i <w+h-1; i++) {
        if (i%2){
            int x = i < w ? 0 : i-w+1;
            int y = i < w? i : w-1;
            while (x <h && y >= 0) {
                rv[d] = r[x*w+y];
                gv[d] = g[x*w+y];
                bv[d] = b[x*w+y];
                x++;
                y--;
                d++;
            }
        }
        else {
            int x = i < h ? i : h-1;
            int y = i < h ? 0 : i-h+1;
            while (x >= 0 && y < w) {
                rv[d] = r[x*w+y];
                gv[d] = g[x*w+y];
                bv[d] = b[x*w+y];
                x--;
                y++;
                d++;
            }
        }
    }
}

void image::putToTxtSnake(string t) {
    FILE* txt = fopen(("out"+t+".txt").c_str(), "w");
    /*for (int i=0; i<shift; i++) {
        putToTxtDiffs(header[i], txt, true);
    }*/
    putToTxtDiffs(rv[0], txt, true);
    putToTxtDiffs(gv[0], txt, true);
    putToTxtDiffs(bv[0], txt, true);

    for (int i=0; i<h; i++){
        for (int j=0; j<w; j++) {
            if (i*w+j < w*h-1){
                putToTxtDiffs(rv[i*w+j+1] - rv[i*w+j], txt, true);
                putToTxtDiffs(gv[i*w+j+1] - gv[i*w+j], txt, true);
                putToTxtDiffs(bv[i*w+j+1] - bv[i*w+j], txt, true);
            }
        }
        //fputc('\n', txt);
    }
}

// decoding (in process)
void image::createImageDecoding(string t, unsigned char* codes_scdc, int Nwords) {
    FILE* image = fopen(("res"+t+".bmp").c_str(), "wb");
    int num[3] = {0,0,0};
        int i = 0;
        while(i < shift) {
            fputc((int)codes_scdc[i], image);
            i++;
        }
        while (i < shift+3) {
            fputc((int)codes_scdc[i], image);
            num[i-shift] = (int)codes_scdc[i];
            i++;
        }
        while (i < Nwords) {
            num[0] += (int)codes_scdc[i];
            num[1] += (int)codes_scdc[i+1];
            num[2] += (int)codes_scdc[i+2];
            //cout << num[0] <<' ' << num[1] << ' ' << num[2]<<' ';
            fputc(num[0], image);
            fputc(num[1], image);
            fputc(num[2], image);
            i+=3;
        }
    fclose(image);
}
