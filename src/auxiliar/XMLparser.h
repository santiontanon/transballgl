#ifndef XML_PARSER
#define XML_PARSER

#include"stdio.h"
#include<vector>

class XMLNode {
public:
	static XMLNode *from_file(const char *fileName);
	static XMLNode *from_file(FILE *fp);
	static XMLNode *from_string(char *str,int *pos);
    static char *replaceQuotes(const char *str);

    XMLNode(XMLNode *xml);  // clone
	~XMLNode();

	class Symbol *get_type(void);
	char *get_value(void);

	char *get_attribute(Symbol *name);
	char *get_attribute(const char *name);
	void set_attribute(const char *name, const char *value);
	std::vector<XMLNode *> *get_children(void);	// note: do not delete the return value of this particular function
	std::vector<XMLNode *> *get_children(Symbol *type);
	std::vector<XMLNode *> *get_children(const char *type);
	XMLNode *get_child(Symbol *type);
	XMLNode *get_child(const char *type);

	void cleanValues();	// removes spaces/tabs, etc. from values (sets to 0 if only spaces/tabs/\r/\n)

	void print(int tabs);
	void writeToFile(FILE *fp, int tabs);
	void writeAttributeValueToFile(FILE *fp, char *value);
    
    void saveToXML(class XMLwriter *w);

private:
	XMLNode(void);
    
	static XMLNode *read_tag_from_file(FILE *fp,int *open,bool look_for_first_character);
	static XMLNode *read_tag_from_string(const char *str,int *pos,int *open);

	Symbol *m_type;
	char *m_value;
	std::vector<Symbol *> m_attribute_names;
	std::vector<char *> m_attribute_values;
	std::vector<XMLNode *> m_children;
};

#endif