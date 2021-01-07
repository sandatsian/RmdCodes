#include<iostream>
#include<cstdio>

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

void createBinaryCode(string t) {
  FILE *image;
  FILE *txt;
  int byte, bytes[8];
  image = fopen((t+".bmp").c_str(),"rb");
  txt = fopen(("out"+t+".txt").c_str(), "w");

  if (image != nullptr) {
    byte = fgetc(image);

    while (byte != EOF)
    {
        for(int i=0;i<8;i++) {
            bytes[i] = byte&(1<<(7-i));
            fputc(bytes[i], txt);
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

char byteFromText(int* text) {
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

void createImage(string t) {
    FILE* txt = fopen(("out"+t+".txt").c_str(), "r");
    FILE* image = fopen("res.bmp", "wb");
    int sym[8], c;
    if (txt != nullptr) {
        for (int j = 0; j < 8; j++){
            c = fgetc(txt);
            if (c == EOF)
                break;
            sym[j] = c;
        }

        while (c != EOF) {
            fputc(byteFromText(sym), image);
            for (int j = 0; j < 8; j++){
                c = fgetc(txt);
                if (c == EOF)
                    break;
                sym[j] = c;
            }
        }
    }
    fclose(image);
    fclose(txt);
}
int main()
{
    createBinaryCode("8");


}
