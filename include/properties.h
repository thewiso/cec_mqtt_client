#include <map>
#include <string>
#include <fstream>

class Properties{
    typedef std::map<std::string, std::string*> String2StringPointerMap;

    public:
        void readFile(std::string filePath);

    protected:
        void parseFile(std::ifstream &fileStream);
        virtual void fillPropertyMap() = 0;
        
        String2StringPointerMap propertyKey2Value;
};