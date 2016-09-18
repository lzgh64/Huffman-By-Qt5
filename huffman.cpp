#include "huffman.h"
#include <sstream>
#include <cctype>

// test
#include <iostream>

// test
void Huffman::test()
{
    compression();

    for (int i = 0; i <= CODES_END; i++) {
        std::cout << i + CHAR_MIN << "  " << huffmanCodes[i] << std::endl;
    }
}

/**********
 * 构造&析构
 *********/

Huffman::Huffman()
    : huffmanTree(NULL), searchTree(NULL), charSize(sizeof(char))
{
    init();
}

Huffman::~Huffman()
{
    if (inFile.is_open()) {
        inFile.close();
    }
    if (outFile.is_open()) {
        outFile.close();
    }
    if (huffmanTree != NULL) {
        delete huffmanTree;
    }
    if (searchTree != NULL) {
        delete searchTree;
    }
}

/************
 * Public部分
 ***********/

/************************************************************************************************
 * 压缩文件保存规则：
 *     压缩文件开端保存编码字符个数，接着保存huffman编码信息，编码信息以字符形式保存，每组编码首个字
 * 符为对应ascii码字符，接着是字符'0'、'1'组成的huffman编码，各组huffman编码以'#'作为起始标志，最
 * 后一组为文件结尾码，以'$'标志开始和结束，结尾码后保存原文件大小，以'$'结束，文件末尾以结尾符编码结
 * 束，若最终不足1bit数据，以0补满。形式表示如下：
 * CharCount # C1 Code1 # C2 Code2 # ... # Cn Coden $ EndCode $ FileSize $ data+EndCode+Some0
 ***********************************************************************************************/
void Huffman::compression()
{
    // 产生huffman编码表
    CountChar();
    GenerateHuffmanTree();
    GenerateHuffmanCodes();

    // 写入编码字符个数
    std::stringstream ss;                           // 利用字符串流将整型数转换为字符串
    ss << charCount;
    std::string tmpStr = ss.str();
    outFile.write(tmpStr.data(), tmpStr.length() * charSize);

    // 写入huffman编码表
    for (int i = 0; i < CODES_END; i++) {
        if (!huffmanCodes[i].empty()) {
            char c = CHAR_MIN + i;
            outFile.write("#", charSize);
            outFile.write(&c, charSize);
            outFile.write(huffmanCodes[i].data(), huffmanCodes[i].length() * charSize);
        }
    }

    // 写入结尾符编码和文件大小
    outFile.write("$", charSize);
    outFile.write(huffmanCodes[CODES_END].data(), huffmanCodes[CODES_END].length() * charSize);
    outFile.write("$", charSize);
    ss << fileSize;                        // 再次利用字符串流将整型数转换为字符串
    tmpStr = ss.str();
    outFile.write(tmpStr.data(), tmpStr.length() * charSize);
    outFile.write("$", charSize);

    // 写入压缩数据
    unsigned long readedSize = 0;
    std::string tempString = "";

    while (!inFile.eof()) {
        char c;
        inFile.read(&c, charSize);
        readedSize++;
        tempString.append(huffmanCodes[c-CHAR_MIN]);
        writeBuffer(tempString);

        if (readedSize%102400 == 0) {
            emit progressChanged((readedSize*100) / fileSize);        // 发出进度改变的信号，信号为百分比的整数部分
        }
    }

    // 写入文件尾标识符编码
    tempString.append(huffmanCodes[CODES_END]);
    writeBuffer(tempString);
    if (!tempString.empty()) {                                    // 最后不足8bits用0补足
        while (tempString.length() < 8*charSize) {
            tempString.push_back('0');
        }
        writeBuffer(tempString);
    }

    emit compressionComplete();
}

void Huffman::decompression()
{
    // 读取编码表至

}

void Huffman::setInFile(std::string in)
{
//    init();
    inFile.open(in, std::ios_base::binary);
}

void Huffman::setOutFile(std::string out)
{
//    init();
    outFile.open(out, std::ios_base::binary);
}

/*************
 * private部分
 ************/

void Huffman::init()
{
    charAmount = 0;
    nodeAmount = 0;
    fileSize   = 0;

    if (inFile.is_open()) {
        inFile.close();
    }
    if (outFile.is_open()) {
        outFile.close();
    }
    if (huffmanTree != NULL) {
        delete huffmanTree;
        huffmanTree = NULL;
    }
    if (searchTree != NULL) {
        delete searchTree;
        searchTree = NULL;
    }

    int i;
    for (i = 0; i < CODES_END; i++) {
        huffmanCodes[i] = "";
        charCount[i] = 0;
    }
    huffmanCodes[i] = "";
}

// 压缩所需函数

void Huffman::writeBuffer(std::string &str)
{
    char buffer = '\0';
    // str数据足以填充一个字符的所有位，就将其写入目标文件
    while (str.length() >= 8*charSize) {
        for (unsigned i = 0; i < 8*charSize; i++) {
            char tempChar = str[i] - '0';
            buffer |= tempChar << i;
        }
        str.erase(0, 8);
        outFile.write(&buffer, charSize);
    }
}

void Huffman::CountChar()
{
    char c;
    charAmount =0 ;
    while (!inFile.eof()) {
//        inFile >> c;                      // 以文本方式无法读取某些字符
        inFile.read(&c, sizeof(c));         // 以二进制方式读取
        if (0 == charCount[c-CHAR_MIN]) {
            charAmount++;                   // 记录总字符种类数
        }
        fileSize++;                         // 记录文件总字符数，即文件大小
        charCount[c-CHAR_MIN]++;            // 记录每个字符出现的次数
    }
    charAmount++;                           // 增加一个文件尾标识符
    inFile.clear();                         // 清除eof后的错误状态
    inFile.seekg(0, std::ios::beg);         // 重置文件指针到文件首
}

void Huffman::GenerateHuffmanTree()
{
    int i, j;

    // 产生空的huffman树
    nodeAmount = 2 * charAmount - 1;
    huffmanTree = new huffman_node[nodeAmount];

    // 元素 0 ~ totalChar-1 为叶节点，即存有字符的节点，0存有结尾标识符
    huffmanTree[0].count = 1;
    huffmanTree[0].lchild = CHILD_OF_LEAF;
    huffmanTree[0].rchild = CHILD_OF_LEAF;
    for (i=0, j=1; i < CODES_END; i++) {
        if (0 != charCount[i]) {
            huffmanTree[j].character = (char)(CHAR_MIN+i);
            huffmanTree[j].count     = charCount[i];
            huffmanTree[j].lchild    = CHILD_OF_LEAF;
            huffmanTree[j].rchild    = CHILD_OF_LEAF;
            j++;
        }
    }

    // 初始化对非叶结点，即元素 totalChar ~ totalNode-1
    for (i = charAmount; i < nodeAmount; i++) {
        huffmanTree[i].count  = -1;
        huffmanTree[i].parent = -1;
        huffmanTree[i].lchild = -1;
        huffmanTree[i].rchild = -1;
    }

    // 连接各节点
    int point1 = -1, point2 = -1;
    for (i = charAmount; i < nodeAmount; i++) {
        point1 = getMinPointBigerThanLast(point2, i-1);
        point2 = getMinPointBigerThanLast(point1, i-1);

        huffmanTree[point1].parent = i;
        huffmanTree[point2].parent = i;

        huffmanTree[i].lchild = point1;
        huffmanTree[i].rchild = point2;
        huffmanTree[i].count  = huffmanTree[point1].count + huffmanTree[point2].count;
    }
}

void Huffman::GenerateHuffmanCodes()
{
    // 此函数内 huffmanTree::count 的意义：
    // 0表示未遍历 1表示已遍历左节点未遍历右节点 2表示已遍历双节点

    // 初始遍历状态
    for (int i = 0; i < nodeAmount; i++) {
        huffmanTree[i].count = 0;
    }

    // 遍历huffman树并填写huffman编码表
    int currentPoint = nodeAmount - 1;
    std::string tempString = "";

    while (currentPoint != -1) {
        huffman_node *currentNode = &huffmanTree[currentPoint];

        // 针对节点不同状态做不同处理

        // 结尾符节点
        if (currentPoint == 0) {
            huffmanCodes[CODES_END] = tempString;               // 码表末位元素保存结尾符

            currentPoint = currentNode->parent;                 // 回到父节点
            tempString.pop_back();

            continue;
        }

        // 字符节点
        if (currentPoint < charAmount) {
            huffmanCodes[currentNode->character-CHAR_MIN] = tempString;            // 保存当前编码到码表

            currentPoint = currentNode->parent;
            tempString.pop_back();

            continue;
        }

        // 未访问节点
        if (currentNode->count == 0) {
            currentPoint = currentNode->lchild;                 // 准备访问左子节点
            tempString.push_back('0');                          // 当前编码末位填0
            currentNode->count = 1;                             // 当前节点状态设为仅已遍历左节点
            continue;
        }

        // 已访问左子节点的节点
        if (currentNode->count == 1) {
            currentPoint = currentNode->rchild;                 // 准备访问右子节点
            tempString.push_back('1');                          // 当前编码末位填1
            currentNode->count = 2;                             // 当前节点状态设为已遍历左右节点
            continue;
        }

        // 已访问左右子节点的节点
        if (currentNode->count == 2) {
            currentPoint = currentNode->parent;                 // 准备遍历父节点
            if (!tempString.empty()){                           // 判断 防止从根节点回到其“父节点”时对空字符串进行pop
                tempString.pop_back();
            }
            continue;
        }
    }

    delete huffmanTree;                                         // 生成编码表后不再需要huffman树
    huffmanTree = NULL;
}

int Huffman::getMinPointBigerThanLast(int lastPoint, int end)
{
    int datum;
    int minPoint = 0;
    int minValue = INT_MAX;

    if (lastPoint == -1) {
        datum = 0;
    } else {
        datum = huffmanTree[lastPoint].count;
    }
    for (int i = 0; i <= end; i++) {
        int currentValue = huffmanTree[i].count;
        if (currentValue == datum && i > lastPoint) {
            minPoint = i;
            break;
        }
        if (currentValue > datum && currentValue < minValue) {
            minPoint = i;
            minValue = currentValue;
        }
    }

    return minPoint;
}

// 解压所需函数

bool Huffman::GenerateSearchTree()
{
    char c;

    // 读取编码字符个数
    charAmount = 0;
    while (inFile.read(&c, charSize), c!='#') {
        if (isdigit(c)) {
            charAmount = 10 * charAmount + (c - '0');
        } else {
            // error
            return false;
        }
    }
    nodeAmount = 2 * charAmount - 1;

    // 读取编码表并生成搜索树
    searchTree = new search_node[nodeAmount];
    for (int i = 0; i < nodeAmount-1; i++) {
        while (inFile.read(&c, charSize), c!='#') {
            if (c == '$') break;


        }
        if (c == '$') break;
    }

    return true;
}
