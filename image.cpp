#include<iostream>
#include<cstdio>
#include "image.h"
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

void image::createBinaryCode(string t) {
  FILE *image;
  FILE *txt;
  int bytes[8];
  int byte;
  image = fopen((t+".bmp").c_str(),"rb");
  txt = fopen(("out"+t+".txt").c_str(), "w");

  if (image != nullptr) {
    byte = fgetc(image);
    while (byte != EOF)
    {
        for(int i=0;i<8;i++) {
            bytes[i] = byte&(1<<(7-i)) ? 1 : 0;
            fputc(bytes[i] + '0', txt);
        }
        fputc(' ', txt);
        byte = fgetc(image);
    }
  }
  else {
    cout << "Cannot open file\n";
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


