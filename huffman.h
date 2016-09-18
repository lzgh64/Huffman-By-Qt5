#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <climits>
#include <QObject>
#include <fstream>

#define CHILD_OF_LEAF    -2
#define CHILD_OF_
#define ALL_CHAR_AMOUNT  (CHAR_MAX - CHAR_MIN + 1)
#define CODES_END        ALL_CHAR_AMOUNT

struct huffman_node{
    char character;
    int  count;
    int  parent;
    int  lchild;
    int  rchild;
};

struct search_node {
    char character;
    char digit;
    int  parent;
    int  lchild;
    int  rchild;
};

class Huffman : public QObject
{
    Q_OBJECT

public:
    Huffman();
    ~Huffman();

    void test();
    void init();

    void setInFile(std::string in);
    void setOutFile(std::string out);

public slots:
    void compression();
    void decompression();

signals:
    void progressChanged(int);
    void compressionComplete();
    void decompressionComplate();

private:

    std::ifstream inFile;
    std::ofstream outFile;
    unsigned long fileSize;
    int           charAmount;
    int           nodeAmount;
    int           charSize;

    // For compression
    void CountChar();
    void GenerateHuffmanTree();
    void GenerateHuffmanCodes();
    void writeBuffer(std::string &str);
    int  getMinPointBigerThanLast(int lastPoint, int end);

    huffman_node *huffmanTree;
    std::string   huffmanCodes[ALL_CHAR_AMOUNT+1];
    int           charCount[ALL_CHAR_AMOUNT];

    // For decompression
    bool GenerateSearchTree();

    search_node *searchTree;
};

#endif // HUFFMAN_H
