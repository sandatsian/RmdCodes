using namespace std;

class image {
private:
    int shift = 54;
    int MAX_SIZE = 10E8;
public:
    void createBinaryCode(string t);
    void createBinaryCodeDiffs(string t);
    int byteFromText(int* text);
    int byteFromTextDiffs(int* text, int num);
    void createImage(string t);
    void createImageDiffs(string t);
    void putToTxt(int, FILE*);
    void putToTxtDiffs(int, FILE*);
};
