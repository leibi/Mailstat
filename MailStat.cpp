#include <map>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <cstdio>        
#include "stdio.h"
#include <ctime>
#include <iostream>
#include <fstream>
using namespace std;

#define BUFSIZE 8192

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

class Mail
{
public:

        Mail(){};
        ~Mail(){};

//private: 
        string  m_MessageID;
        string  m_From;
        string  m_To;
        string  m_Subject;
        tMyDate m_Date;

};


class Mailfile
{
public:
        Mailfile(){};
        Mailfile(const string& iFilename){ParseMailFile(iFilename);};
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
        
        ifstream iFile (iFilename.c_str(),ios::binary|ifstream::in);
        
        if(!iFile.good()) 
         return 1;
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

           size_t n = line.find("From "); //indicates new mail
           if(string::npos == n || n != 0 )
           {
            //cout << "strange line: " << line << endl;
            iFile.getline(buffer,BUFSIZE);
            line=buffer;
           }
           //cout << "Main: Current line: " << line << endl;
           if (n != string::npos && 0 == n)
           {
               //cout << "New Mail Nr: " << counter << endl;
               counter++;
               Mail NewMail;
               iFile.getline(buffer,BUFSIZE);
               line = buffer; 
               while(iFile.good() && 0 != line.find("From "))
               {
                 //cout << "Sub: Current line: " << line << endl;
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
                      NewMail.m_From=line.substr(n);      
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
                     NewMail.m_To=line.substr(n);      
                     //cout << "Adding To " << NewMail.m_To << "(" << line << ")" <<endl;
                     if(m_ToStat.end() == m_ToStat.find(NewMail.m_To))
                     {
                         m_ToStat[NewMail.m_To] = 1;
                     }
                     else
                     {
                         ++m_ToStat[NewMail.m_To];
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
                 //if(!iFile.good())
                    //cout << "file no longer good before getline" << endl;
                 iFile.getline(buffer,BUFSIZE);
                 //if(!iFile.good())
                    //cout << "file no longer good after getline" << endl;
                 line = buffer;
                 //cout << "Sub: Current Next line: " << line << endl;
               }
               //cout << "pushing back Mail" << endl;
               m_Mails.push_back(NewMail);
           }
           else
           {
                //cout << "No New Mail for Line: " << line << " as n is " << n << endl;
           }
           /*
           if(!iFile.good())
             cout << "The file is no longer good " << endl;
           if(iFile.eof())
             cout << " we have reached eof" << endl;

           if(iFile.fail())
             cout << "fail is true" << endl;
             */
        }//end while(fgets()) 

        return 0;
}


size_t Mailfile::CreateStatistics()
{

 return 0;
}



int main(int argc, char** argv)
{
   string mboxpath="/home/leibi/Mail/spam";
   cout << "Opening Inbox-file: " << mboxpath << endl;

   Mailfile mbox(mboxpath);
   cout << mbox.GetNbMails() << " Mails read" << endl;

   //mbox.CreateStatistics();
   tMap::const_iterator pIter =   mbox.m_FromStat.begin();
   tMap::const_iterator pEnd =   mbox.m_FromStat.end();

    while(pIter != pEnd)
    {
        cout << "From: " << pIter->first << ": " << pIter->second << endl;
        ++pIter;

    }

   pIter =   mbox.m_ToStat.begin();
   pEnd =   mbox.m_ToStat.end();

    while(pIter != pEnd)
    {
        cout << "To: " << pIter->first << ": " << pIter->second << endl;
        ++pIter;

    }

    return 0;
}




