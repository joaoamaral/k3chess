#include "StringUtils.h"

std::string uintToStr(unsigned value)
{
   std::string s(' ', 32);
   int i = s.length()-1;
   //
   while(true)
   {
      s[i] = value%10 + '0';
      value /= 10;
      //
      if(value) --i;
      else break;
   }
   //
   return s.substr(i);
}

unsigned strToUint(const std::string& s, unsigned pos, unsigned& pos_parsed)
{
   unsigned value = 0;
   for(;pos<s.length();++pos)
   {
      char c = s[pos];
      //
      if(c>='0' && c<='9')
      {
         value *= 10;
         value += c - '0';
      }
      else
      {
         // non-digit encountered, stop converting
         break;
      }
   }
   pos_parsed = pos;
   return value;
}

void skipSpace(const std::string& s, unsigned& pos)
{
   for(;pos<s.length();++pos)
   {
      char c = s[pos];
      if(c!=' ' && c!='\t') break;
   }
}

void skipNonSpace(const std::string& s, unsigned& pos)
{
   for(;pos<s.length();++pos)
   {
      char c = s[pos];
      if(c==' ' || c=='\t') break;
   }
}

unsigned countChars(const std::string& s, char c)
{
   unsigned count = 0;
   std::string::const_iterator it = s.begin(), itEnd = s.end();
   for(;it!=itEnd;++it)
   {
      if(*it==c) ++count;
   }
   return count;
}

bool startsWith(const std::string& s, const char *prefix)
{
   std::string::const_iterator it = s.begin(), itEnd = s.end();
   for(;it!=itEnd;++it,++prefix)
   {
      if(*prefix==0) return true;
      if(*it!=*prefix) return false;
   }
   return false;
}

std::string trim(const std::string& s)
{
   size_t ibeg = 0;
   size_t iend = s.length();
   while(ibeg<iend)
   {
      if(s[ibeg]==' ') ++ibeg;
      else if(s[iend]==' ') --iend;
      else break;
   }
   return s.substr(ibeg, iend-ibeg);
}

void splitFilePath(const QString& path, QString& folderPath, QString& fileName,
                   QString& extension)
{
   int idx = path.length()-1;
   bool metNonSep = false;
   int idxStart = 0, idxEnd = path.length();
   while(idx>0)
   {
      switch(path[idx].toAscii())
      {
         case '/':
         case '\\':
         case ':':
            if(metNonSep)
            {
               idxStart= idx+1;
            }
            else
            {
               idxEnd = idx;
            }
            break;
         default:
            metNonSep = true;
      }
      if(idxStart>0) break;
      --idx;
   }
   //
   fileName = path.mid(idxStart, idxEnd-idxStart);
   folderPath = path.left(idxStart-1);
   //
   idx = fileName.length()-1;
   //
   extension.clear();
   //
   while(idx>0)
   {
       if(fileName[idx]=='.')
       {
           extension = fileName.mid(idx+1);
           break;
       }
       --idx;
   }
}

QString extractFileName_NoExt(const QString& path)
{
    QString folderPath, fileName, extension;
    splitFilePath(path, folderPath, fileName, extension);
    //
    return fileName;
}

QString extractFolderPath(const QString& path)
{
    QString folderPath, fileName, extension;
    splitFilePath(path, folderPath, fileName, extension);
    //
    return folderPath;
}
