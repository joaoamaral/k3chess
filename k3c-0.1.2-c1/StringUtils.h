#ifndef __StringUtils_h
#define __StringUtils_h

#include <string>
#include <QString>

std::string uintToStr(unsigned value);
unsigned strToUint(const std::string& s, unsigned pos, unsigned& pos_parsed);
void skipSpace(const std::string& s, unsigned& pos);
void skipNonSpace(const std::string& s, unsigned& pos);
unsigned countChars(const std::string& s, char c);
bool startsWith(const std::string& s, const char *prefix);
std::string trim(const std::string& s);

void splitFilePath(const QString& path, QString& folderPath, QString& fileName,
                   QString& extension);
QString extractFileName_NoExt(const QString& path);
QString extractFolderPath(const QString& path);

#endif
