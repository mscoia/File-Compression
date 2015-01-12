#include <iostream>
#include <queue>
#include <map>
#include <climits>
#include "bstream.h"
#include <cstdlib>

using namespace std;

/* Type: ext_char
* A type representing a character, a pseudo-eof, or nothing. */
typedef int ext_char;

typedef map<ext_char, int> FreqMap;
typedef map<ext_char, string> CodeMap;

/* Constant: PSEUDO_EOF
* A constant representing the PSEUDO_EOF marker that you will
* write at the end of your Huffman-encoded file.
*/
const ext_char PSEUDO_EOF = 256;

/* Constant: NOT_A_CHAR
* A constant representing an extended character that does not
* actually hold a value.	 When you are constructing your Huffman
* encoding tree, you should set the characters in each internal
* node to this value to explicitly mark that they are not being
* used.
*/
const ext_char NOT_A_CHAR = 257;

/* Struct used to store all the informantion a huffman node needs*/
struct HuffmanNode
{
	ext_char data;
	int weight;
	HuffmanNode *left; // Left subtree ( corresponding code to '0' )
	HuffmanNode *right; // Left subtree ( corresponding code '1' )
	//constructor for leaves
	HuffmanNode(ext_char dat, int wgt) :data(dat), weight(wgt), left(NULL), right(NULL){}
	// constructor for internal nodes
	HuffmanNode(HuffmanNode *l, HuffmanNode *r) :data(NOT_A_CHAR), weight(l->weight + r->weight), left(l), right(r) {}

};


/* Structure used for comparing two huffman Node. Used by priority queue*/
struct compareHuffmanNode
{
	bool operator()(const HuffmanNode *first, const HuffmanNode *second)
	 {
		return first->weight  > second->weight;
	}
};


// Get a line from the user. 
string getLine(string str)
{
	bool tryAgain = true;
	string outString;
	while (tryAgain)
	{
		cout << str << endl;
		getline(cin, outString);
		if (outString == "")
		{
			cout << "Enter a valid string\n";
		}
		else
		{
			tryAgain = false;
		}
	}
	return outString;
}

/* Function: openFile
* --------------------------------------------------------
* Prompts the user for the name of a file to open, opening
* the indicated stream object to that file.
*/
template <typename FileStream>
void openFile(FileStream& file, string filename ) {
	//while (true) {

		file.open(filename.c_str());

		if (file.is_open()) return;

		cout << "Sorry, I couldn't open that file." << endl;
		file.clear();
		
	//}
}



/* Function: compareFiles
* --------------------------------------------------------
* Compares two files byte-by-byte to determine whether or
* not they are equivalent to one another.
*/
void compareFiles(string firstFile, string secondFile) {
	/* Get the two files to compare. */
	ifstream one, two;
	openFile(one, firstFile);
	openFile(two, secondFile);

	/* Read the contents of the files into a stringstream for
	* comparison.
	*/
	ostringstream oneContents, twoContents;
	oneContents << one.rdbuf();
	twoContents << two.rdbuf();

	const string oneString = oneContents.str(), twoString = twoContents.str();

	/* Check lengths are the same. */
	if (oneString.length() != twoString.length()) {
		cout << "Files differ!" << endl;
		cout << "File one has length " << oneString.length() << "." << endl;
		cout << "File two has length " << twoString.length() << "." << endl;
	}
	else {
		/* Compare the two sequences to find a mismatch. */
		pair<string::const_iterator, string::const_iterator> diff =
			mismatch(oneString.begin(), oneString.end(), twoString.begin());
		if (diff.first != oneString.end()) {
			cout << "Files differ!" << endl;
			ptrdiff_t offset = diff.first - oneString.begin();
			cout << "Bytes differ at offset " << offset << "." << endl;
			cout << "File one has value " << *diff.first << endl;
			cout << "File two has value " << *diff.second << endl;
		}
		else {
			/* Files match! */
			cout << "Files match!" << endl;
		}
	}
}

void gen_CodeMap(string s, HuffmanNode* huff, CodeMap& cm){
   		
	if(huff->data == NOT_A_CHAR){
		if(huff->left != NULL){
			s += "0";
			gen_CodeMap(s, huff->left, cm);
			s = s.substr(0, s.size()-1);
		}
		if(huff->right != NULL){
			s += "1";
			gen_CodeMap(s, huff->right, cm);
			s = s.substr(0, s.size()-1);
		}
	}else{	
		cm[huff->data] = s;
	}
}

/* Function: runEncoding
* --------------------------------------------------------
* Harness code to run your compression routine. */
void runEncoding()
{

	string inputFileName = getLine("Enter the name of file to encode ");
	string outputFileName = getLine("Enter the encoded file name ");
	string freqFileName = getLine("Enter the freq. file name");
  /* Your implementation */
	ifbstream in;
	ofbstream out;
	ofbstream fr;
		
	FreqMap map1;
	priority_queue<HuffmanNode* , vector<HuffmanNode*>, compareHuffmanNode> pq;
	openFile(in, inputFileName);
	openFile(out,outputFileName);
	openFile(fr,freqFileName);
	for(long w = 0; w < in.size(); w++){
		int t = int(in.get());
		++map1[t];
	}
	++map1[PSEUDO_EOF];

	for(FreqMap::const_iterator itr=map1.begin(); itr != map1.end(); itr++){

		fr << itr->first << " " << itr->second << endl;
		HuffmanNode* temp = new HuffmanNode(itr->first, itr->second);
		pq.push(temp);

	}
	while(pq.size() > 1){
		HuffmanNode* temp1 = pq.top();
		pq.pop();
		HuffmanNode* temp2 = pq.top();
		pq.pop();
		pq.push(new HuffmanNode(temp1, temp2));
	}

	string s;
	CodeMap cm;

	gen_CodeMap(s, pq.top(), cm);

	in.rewind();

	for(long e = 0; e < in.size(); e++){
		int u = int(in.get());
		string m = cm[u];
		for(unsigned int k=0; k<m.length(); k++){
			if(m[k]=='0'){
				out.writeBit(0);
			}else {
				out.writeBit(1);
			}
		}
	}
	string eof = cm[256];
	for(unsigned int y=0; y<eof.length(); y++){
		if(eof[y]=='0'){
			out.writeBit(0);
		}else {
			out.writeBit(1);
		}
	}
	
	in.close();			
	out.close();
	fr.close();		
}

/* Function: runDecoding
* --------------------------------------------------------
* Harness code to run your decompression routine. */
void runDecoding()
{
	string inEncodeFileName = getLine("Enter the name of file to decode ");
	string outEncodeFileName = getLine("Enter the  file name to save after decoding");
	string freqFileName = getLine("Enter the freq. file name");
  /* Your implementation */
	ifbstream in;
	ofbstream out;
	ifbstream fr;
	priority_queue<HuffmanNode*, vector<HuffmanNode*>, compareHuffmanNode> pq;
	openFile(in, inEncodeFileName);
	openFile(out, outEncodeFileName);
	openFile(fr, freqFileName);
	FreqMap fm;

	while(fr.good()){
		string s1, s2;
		int i, j;
		stringstream ss1, ss2;
		fr >> s1;
		fr >> s2;
		ss1.str(s1);
		ss1 >> i;
		ss2.str(s2);
		ss2 >> j;
		fm[i] = j;
	}

	for(FreqMap::const_iterator it = fm.begin(); it != fm.end(); it++){
		HuffmanNode* temp = new HuffmanNode(it->first, it->second);
		pq.push(temp);
	}
	while(pq.size() > 1){
		HuffmanNode* temp1 = pq.top();
		pq.pop();
		HuffmanNode* temp2 = pq.top();
		pq.pop();
		pq.push(new HuffmanNode(temp1, temp2));
	}

	string s;
	CodeMap cm;
	gen_CodeMap(s, pq.top(), cm);
	
	stringstream ss3;
	string st, strc;
	char r;

	for(long u = 0; u<in.size()*8; u++){
		int rb = in.readBit();
		ss3 << rb;
		st += ss3.str();
		ss3.str("");

		for(CodeMap::const_iterator itrs = cm.begin(); itrs != cm.end(); itrs++){
			if(itrs->second==st){
				if(itrs->first==256){
					return;
				}
				else{
					r = (char)itrs->first;
					out << r;
					st = "";
				}
			}
		}
	}
	in.close();
	out.close();
	fr.close();				
}

/* Function: runCompare
* --------------------------------------------------------
* Harness code to compare two files */
void runCompare()
{
	string firstFile = getLine("Enter the name of first file ");
	string secondFile = getLine("Enter the name of second file ");
	compareFiles(firstFile, secondFile);
}


// Entry point
int main()
{
  while (1)
  {
    cout << "Enter your choice \n"
      " 1. Huffman Encoding \n"
      " 2. Huffman Decoding \n"
      " 3. Compare Files\n" 
      " 4. Exit" << endl;
    int choice;
    cin >> choice;
    cin.clear(); cin.ignore(INT_MAX, '\n');
    if (choice == 1)
    {
      cout << " Encoding " << endl;
      runEncoding();
      cout << " Encoding done " << endl;
    }
    else if (choice == 2)
    {
      cout << " Decoding " << endl;
      runDecoding();
      cout << " Decoding done " << endl;
    }
    else if (choice == 3)
    {
      cout << "Comparing two files" <<endl;
      runCompare();
      cout << "Comparing done" <<endl;
    }
    else if( choice == 4)
    {
      cout <<"Exiting" <<endl;
      return 0;
    }
    else
    {
      cout << "Wrong choice. Try again \n";
    }
  }
  return 0;
}

int test()
{
	string name = "a.txt";
	ofbstream outfile;
	outfile.open(name.c_str());
	if (outfile.fail()) error("Can't open output file!");
	outfile << 134;
	char c = 'C';

	cout << sizeof('C') << c << " "<< int(c)<< endl;
	outfile.put('C');
	outfile.writeBit(0);
	outfile.writeBit(1);
	outfile.close();
	ifbstream infile;
	infile.open(name.c_str());
	if (infile.fail()) error("Can't open input file!");
	int num;
	infile >> num;
	cout << "read " << num;
	char dd = infile.get();
	cout << " " << dd << " and "
		<< infile.readBit() << infile.readBit() << endl;
	infile.close();
	getchar();
	getchar();
	return 0;
}
