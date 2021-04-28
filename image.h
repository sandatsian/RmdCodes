using namespace std;
#include <vector>

class Image {
private:
    int shift = 54;
    int MAX_SIZE = 10E8;
    int h;
    int w;
    int fullSize;
    vector<int> header;
    vector<int> r;
    vector<int> g;
    vector<int> b;

public:
    // standard
    void createBinaryCode(string t);
    void createImage(string t);
    int byteFromText(int* text);
    void putToTxt(int, FILE*);
    void putToTxtBinary(int byte, FILE* txt, bool);
    // with diffs
    int byteFromTextDiffs(int* text, int num);
    void createBinaryCodeDiffs(string t);
    void createImageDiffs(string t); // puts values to out.txt
    void putToTxtDiffs(int, FILE*, bool);
    // walk by diagonals
    void getBytes(string t);
    void createBinaryCodeSnake();
    void putToTxtSnake(string t);
    // using decoding values
    void createImageDecoding(string t, unsigned char* codes_scdc, int Nwords);

    // vector values
    vector<int> rv;
    vector<int> gv;
    vector<int> bv;
};
