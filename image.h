using namespace std;
#include <vector>

class image {
private:
    int shift = 54;
    int MAX_SIZE = 10E8;
    int h;
    int w;
    vector<int> header;
    vector<int> r;
    vector<int> g;
    vector<int> b;
    vector<int> rv;
    vector<int> gv;
    vector<int> bv;
public:
    // standard
    void createBinaryCode(string t);
    void createImage(string t);
    int byteFromText(int* text);
    void putToTxt(int, FILE*);
    // with diffs
    int byteFromTextDiffs(int* text, int num);
    void createBinaryCodeDiffs(string t);
    void createImageDiffs(string t);
    void putToTxtDiffs(int, FILE*, bool);
    // walk by diagonals
    void getBytes(string t);
    void createBinaryCodeSnake();
    void putToTxtSnake(string t);
    // using decoding values
    void createImageDecoding(string t, unsigned char* codes_scdc, int Nwords);
};
