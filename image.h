using namespace std;

class image {
private:
    int shift = 54;
    int MAX_SIZE = 10E8;
public:
    // standard
    void createBinaryCode(string t);
    void createBinaryCodeDiffs(string t);
    int byteFromText(int* text);
    // with diffs
    int byteFromTextDiffs(int* text, int num);
    void createImage(string t);
    void createImageDiffs(string t);
    void putToTxt(int, FILE*);
    void putToTxtDiffs(int, FILE*);
    // using decoding values
    void createImageDecoding(string t, unsigned char* codes_scdc, int Nwords);
};
