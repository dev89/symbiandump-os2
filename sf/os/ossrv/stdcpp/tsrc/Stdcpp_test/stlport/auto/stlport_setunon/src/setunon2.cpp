 
// STLport regression testsuite component.
// To compile as a separate example, please #define MAIN.

#include <algorithm>
#include <iostream>
#include <cstring>
#include <iterator>
#include <functional>

#ifdef MAIN 
#define setunon2_test main
#endif

#if !defined (STLPORT) || defined(__STL_USE_NAMESPACES)
using namespace std;
#endif
int setunon2_test(int, char**)
{
 int failures=0;
  cout<<"Results of setunon2_test:"<<endl;
char* word1 = "ABCDEFGHIJKLMNO";
char* word2 = "LMNOPQRSTUVWXYZ";
char word3[27];
  ostream_iterator <char> iter(cout, " ");
  cout << "word1: ";
 
 if(strcmp(word1,"ABCDEFGHIJKLMNO"))
    failures++;
  copy(word1, word1 + ::strlen(word1), iter);
  cout << "\nword2: ";
  
  if(strcmp(word2,"LMNOPQRSTUVWXYZ"))
    failures++;
  
  copy(word2, word2 + ::strlen(word2), iter);
  cout << endl;
  set_union(word1, word1 + ::strlen(word1),
             word2, word2 + ::strlen(word2),
             word3,
             less<char>());

  word3[26]='\0';
  if(strcmp(word3,"ABCDEFGHIJKLMNOPQRSTUVWXYZ"))
     failures++;
  copy(word3,word3+ ::strlen(word3),iter);  
  cout << endl;
  
  if(failures)
    return 1;
  else  
    return 0;
}

