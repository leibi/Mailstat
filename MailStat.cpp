#include <map>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <cstdio>        
#include "stdio.h"
#include <ctime>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#define BUFSIZE 8192

// this is a test comment
// oh, another comment

static double bDEBUG = false;

struct tMyDate
{
        int Day;
        int Month;
        int Year;
};

struct eqstring {
    bool operator()(std::string a, std::string b) const {
      return a == b;
    }
};
   
//typedef map<string,int,eqstring> tMap;
typedef map<string,int> tMap;
typedef map<int,string> tReverseMap;

class Mail
{
public:

        Mail(){};
        ~Mail(){};

        enum eSpecifier
        {
                eFrom,
                eTo
        };

        void AddMailAdress(const string& iString, const eSpecifier iSpec);

//private: 
        string          m_MessageID;
        string          m_From;
        vector<string>  m_To;
        string          m_Subject;
        tMyDate         m_Date;

};


void Mail::AddMailAdress(const string& iString, const eSpecifier iSpec)
{
        //first look for <> around the adress
        if(bDEBUG)
                cout << "Input is " << "\"" << iString << "\"" << endl;
        string Adress = iString;
        size_t n = iString.find("<<");
        if(string::npos != n)
        {
                //cout << "< found " << iString << endl;
                size_t p = iString.find(">>");
                Adress = iString.substr(n+2,p-n-2);
                //cout << "substr Adress: " << "\"" << Adress <<"\""<< endl;
        }
        else
        {
                n = iString.find("<");
                if(string::npos != n)
                {
                        //cout << "< found " << iString << endl;
                        size_t p = iString.find(">");
                        Adress = iString.substr(n+1,p-n-1);
                        //cout << "substr Adress: " << "\"" << Adress <<"\""<< endl;
                }
        }


        // remove whitespaces

        n = Adress.find(" ");
        while(0 == n)
        {
          //cout << "Adress1: " << "\"" << Adress << "\"" << endl;
          Adress = Adress.substr(1);   
          n = Adress.find(" ");
        }
        
        while(string::npos != n)
        {
          //cout << "Adress2: " << Adress << endl;
          Adress = Adress.substr(0,Adress.length()-1);
          n = Adress.find(" ");
        }
        
        //what if there are multiple addresses seperated by ","?
        
        //cout << "Adress: " << "\"" << Adress <<"\""<< endl;
        switch(iSpec)
        {
           case eFrom: m_From = Adress;break;
           case eTo:{
                        n = Adress.find(",");
                        if(string::npos == n)
                        {
                                if(bDEBUG) 
                                  cout << "Adding address : " << Adress<< endl;
                                m_To.push_back(Adress);
                        }
                        else
                        {
                                while(string::npos != n && 0 != n)
                                {
                                        //cout << "current Adress: " << Adress <<
                                        //"with n " << n << endl;
                                        string sub = Adress.substr(n);
                                        if(bDEBUG) 
                                                cout << "Adding address : " <<  sub << endl;
                                        m_To.push_back(sub);
                                        Adress.erase(0,n);
                                        n = Adress.find(",");
                                }
                        }
                        break;
                    }
        };
}


class Mailfile
{
public:
        Mailfile(){};
        Mailfile(const string& iFilename){int ret = ParseMailFile(iFilename);
        if(ret) cout << "ParseMailfile returned with " << ret << endl;};
        ~Mailfile(){};

        size_t GetNbMails(){return m_Mails.size();}
        
        size_t CreateStatistics();

//protected:
        int ParseMailFile(const string iFilename);

//private:
        vector<Mail> m_Mails;

        tMap m_FromStat;
        tMap  m_ToStat;
        
};


int Mailfile::ParseMailFile(const string iFilename)
{
        char buffer[BUFSIZE] = "";
      
        cout.setf(ios::fixed, ios::floatfield);
        cout.setf(ios::showpoint);


        cout << "Opening File " << endl;
        ifstream iFile (iFilename.c_str(),ios::binary|ifstream::in);
        cout << "File open" << endl;
        
        if(!iFile.good()) 
         return 1;
        
        //get line count
       
         
        cout << "Calculating Line count" << endl;
        size_t line_count = 1+std::count(std::istreambuf_iterator<char>(iFile),
                            std::istreambuf_iterator<char>(),'\n');

        cout << "Line count calculated" << endl;
        iFile.seekg(0, ios::beg);

        size_t cur_line_count =0;
        
        // Read one line at a time
        size_t counter =0;
        string line;
        while(iFile.good())
        { 
          /**
           * We check for the following fields: 
           * - message-id
           * - From:
           * - To:
           * - Subject:
           * - Date:
           *
           * The body is omitted at the moment
           */

           ++cur_line_count;
           //cout << "The Current Line is: " << cur_line_count << "(" << line_count << ")" <<"\n" ;
           //cout << "The Current Line is: " << cur_line_count << endl << flush;
           double x = (double(cur_line_count*100)/double(line_count));
           cout << setprecision(2)<< x << " %" << "\r" << flush;;
          
           size_t n = line.find("From "); //indicates new mail
           if(string::npos == n || n != 0 )
           {
            if(bDEBUG)
                cout << "strange line: " << line << endl;
            iFile.getline(buffer,BUFSIZE);
            line=buffer;
           }
           if(bDEBUG)
                cout << "Main: Current line: " << "\"" << line << "\"" << "(" <<
                buffer << ")" << endl;
           if (n != string::npos && 0 == n)
           {
               if(bDEBUG)
                cout << "New Mail Nr: " << counter << endl;
               counter++;
               Mail NewMail;
               iFile.getline(buffer,BUFSIZE);
               line = buffer; 
               while(iFile.good() && 0 != line.find("From "))
               {
                 ++cur_line_count;
                 if(bDEBUG)
                        cout << "Sub: Current line: " << line << endl;
                 //A new mail starts here
                 n = line.find("Message-ID:");
                 if(0== n && n != string::npos)
                 {
                    n = line.find(" "); 
                    if(string::npos != n)
                    NewMail.m_MessageID = line.substr(n);
                    //cout << "Adding Message ID " << NewMail.m_MessageID<<endl;
                 }
                 n = line.find("From:");
                 if(0 == n && n != string::npos )
                 {
                    n = line.find(" "); 
                    
                    if(string::npos != n)
                    {
                      NewMail.AddMailAdress(line.substr(n),Mail::eFrom);      
                      //cout << "Adding From " << NewMail.m_From<<endl;
                      if(m_FromStat.end() == m_FromStat.find(NewMail.m_From))
                      {
                        m_FromStat[NewMail.m_From] = 1;
                      }
                      else
                      {
                        ++m_FromStat[NewMail.m_From];
                      }
                    }
                 }
                 n = line.find("To:");
                 if(0 == n && n != string::npos)
                 {
                    n = line.find(" "); 
                    if(string::npos != n)
                    {
                      //NewMail.m_To=line.substr(n);      
                      NewMail.AddMailAdress(line.substr(n),Mail::eTo);      
                      //cout << "Adding To " << NewMail.m_To << "(" << line << ")" <<endl;
                      vector<string>::iterator pV = NewMail.m_To.begin();
                      for(pV = NewMail.m_To.begin();pV != NewMail.m_To.end();++pV)
                      {
                         if(m_ToStat.end() == m_ToStat.find(*pV))
                         {
                           m_ToStat[*pV] = 1;
                         }
                         else
                         {
                           ++m_ToStat[*pV];
                         }
                      }
                    }
                 }
                 n = line.find("Subject:");
                 if(0 ==n && n != string::npos)
                 {
                    n = line.find(" "); 
                    if(string::npos != n)
                      NewMail.m_Subject=line.substr(n);      
                    //cout << "Adding Subject " << NewMail.m_Subject<<endl;
                 }
                 if(bDEBUG)
                 {
                    if(!iFile.good())
                        cout << "file no longer good before getline" << endl;
                 }
                 iFile.getline(buffer,BUFSIZE);
                 if(bDEBUG)
                 {
                    if(!iFile.good())
                        cout << "file no longer good after getline" << endl;
                 }
                 line = buffer;
                 if(bDEBUG)
                         cout << "Sub: Current Next line: " << line << endl;
               }
               //cout << "pushing back Mail" << endl;
               m_Mails.push_back(NewMail);
           }
           else
           {
                //cout << "No New Mail for Line: " << line << " as n is " << n << endl;
           }
        }//end while(file.good()) 


        cout <<"100.00 %" << endl; // 100% per definition

        return 0;
}


size_t Mailfile::CreateStatistics()
{

 return 0;
}



int main(int argc, char** argv)
{
   string mboxpath="/home/leibi/Mail/spam";

#if 0
   cout << "argc: " << argc << endl;
   int c=0;
   for(c=0; c<argc;++c)
   {
        cout << c << ":" << argv[c] << endl;
   }
#endif

   if(2 == argc)
     mboxpath=argv[1];
   if (3 == argc)
   {
        if(0 == strncmp(argv[2],"-d",2))
            bDEBUG = true;
   }

   if(bDEBUG)
        cout << "Debug mode enabled " << endl;
   
   cout << "Opening Inbox-file: " << mboxpath << endl;

   Mailfile mbox(mboxpath);
   cout << mbox.GetNbMails() << " Mails read" << endl;

   //mbox.CreateStatistics();
   tMap::const_iterator pIter =   mbox.m_FromStat.begin();
   tMap::const_iterator pEnd =   mbox.m_FromStat.end();

   //TODO: command line arguments

#if 0
    while(pIter != pEnd)
    {
        cout << "From: " << pIter->first << ": " << pIter->second << endl;
        ++pIter;

    }
#endif
   pIter =   mbox.m_ToStat.begin();
   pEnd =   mbox.m_ToStat.end();

        //TODO: Command line arguments
   tReverseMap rMap;
   tReverseMap::iterator pR = rMap.begin();
   while(pIter != pEnd)
   {
       //cout << "To: " << pIter->first << ": " << pIter->second << endl;
       pR = rMap.find(pIter->second);
       if(rMap.end() == pR)
       {
        rMap[pIter->second] = pIter->first;
       }
       else
       {
        pR->second = pR->second + "," + pIter->first;
       }
       ++pIter;
   }


   cout << endl << "The statistics part " << endl << endl;
   pR = rMap.end();
   while(rMap.begin() != pR)
   {
        --pR;
        cout <<  pR->first << "\t(" <<
        (double)(((double)pR->first/(double)mbox.GetNbMails())*100) << " %)" <<
        ": " << "\t"<< pR->second << endl;
   }


   
   return 0;
}




