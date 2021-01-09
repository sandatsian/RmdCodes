using namespace std;

class image {
private:
    int shift = 1078;
public:
    void createBinaryCode(string t);
    char byteFromText(int* text);
    void createImage(string t);
};
