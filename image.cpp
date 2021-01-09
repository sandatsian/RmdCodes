#include<iostream>
#include<cstdio>
#include "image.h"
#include <bitset>
using namespace std;

/*void getPalette(){
    Graphics::Bitmap->LoadFromFile("1.bmp");
    int m = Bitmap->Canvas->Pixels.size();
    int n = Bitmap->Canvas->Pixels[0].size();
    FILE* txt = fopen("out.txt", "w");
    ofstream out("out.txt");
    out.open("out.txt");
    for (int x = 0; x < m; x++){
        for (int y = 0; y < n; y++){
            TColor PixelColor= Bitmap->Canvas->Pixels[x][y];
        int R = GetRValue(PixelColor);
        int G = GetGValue(PixelColor);
        int B = GetBValue(PixelColor);
            out << R << ' ' << G << ' ' << B << endl;
        }
    }
}*/
void image::putToTxt(int byte, FILE* txt){
    //cout << bitset<sizeof(byte) * CHAR_BIT>(byte) << " ";
    for(int i=0;i<8;i++) {
            fputc((byte&(1<<(7-i)) ? 1 : 0) + '0', txt);
        }
    fputc(' ', txt);
}
// using diffs
void image::createBinaryCodeDiffs(string t) {
  FILE *image;
  FILE *txt;
  int byte, am = 0;
  image = fopen((t+".bmp").c_str(),"rb");
  txt = fopen(("out"+t+".txt").c_str(), "w");

  if (image != nullptr) {
    byte = fgetc(image);
    while (byte != EOF && am < shift)
    {
        am++;
        putToTxt(byte, txt);
        byte = fgetc(image);
    }
    //int r[MAX_SIZE], g[MAX_SIZE], b[MAX_SIZE];
    int r=fgetc(image),g=fgetc(image), b=fgetc(image);
    putToTxt(r, txt);
    putToTxt(g, txt);
    putToTxt(b, txt);
    int r1= fgetc(image), b1=fgetc(image), g1=fgetc(image);
    while (!(r1 == EOF && g1== EOF && b1==EOF)) {
        if (r1 != EOF)
            putToTxt(r - r1, txt);
        if (g1 != EOF)
            putToTxt(g - g1, txt);
        if (b1 != EOF)
            putToTxt(b - b1, txt);
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

char image::byteFromTextDiffs(int* text, int num) {
    char result=0;
      for(int i=0;i<8;i++)
      {
        if(text[i]=='1')
        {
          result |= (1<< (7-i) );
        }
      }
      return result + num;
}

void image::createImageDiffs(string t) {
    FILE* txt = fopen(("out"+t+".txt").c_str(), "r");
    FILE* image = fopen(("res"+t+".bmp").c_str(), "wb");
    int sym[8], c, num = 0;
    if (txt != nullptr) {
        for (int j = 0; j < 8; j++){
            c = fgetc(txt);
            if (c == EOF)
                break;
            sym[j] = c;
        }
        c = fgetc(txt);
        while (c != EOF) {
            num = byteFromTextDiffs(sym, num);
            fputc(num, image);
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

char image::byteFromText(int* text) {
    char result=0;
      for(int i=0;i<8;i++)
      {
        if(text[i]=='1')
        {
          result |= (1<< (7-i) );
        }
      }
      return result;
}
