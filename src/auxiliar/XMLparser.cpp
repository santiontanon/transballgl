#ifdef WIN32
#include "windows.h"
#else
#include <sys/time.h>
#include <time.h>
#endif
#include <assert.h>
#include "stdlib.h"
#include "string.h"
#include "stdarg.h"
#include "math.h"
#include <list>
#include <vector>
#include "debug.h"
#include "auxiliar.h"

#include "Symbol.h"

#include "XMLparser.h"
#include "XMLwriter.h"


XMLNode *XMLNode::from_file(const char *fileName)
{
	FILE *fp = fopen(fileName,"r+");
	if (fp==0) return 0;
	XMLNode *node = from_file(fp);
	fclose(fp);
	return node;
}


XMLNode *XMLNode::from_file(FILE *fp)
{
	int bufferSize = 1024;
	char *buffer = new char[bufferSize];
	std::list<XMLNode *> stack;
	XMLNode *node,*first_node=0;
	int open;   // 0 : open tag, 2 : close tag, 1: self-contained tag
	char c;
	bool look_for_first_character=true;

	do {
		node=read_tag_from_file(fp,&open,look_for_first_character);

		if (first_node==0) first_node=node;

		if (node!=0) {
			if (open==0 || open==1) {
                if (!stack.empty()) stack.front()->m_children.push_back(node);
                if (open==0) stack.push_front(node);
                if (stack.empty() && open!=0) {
                    output_debug_message("XMLNode::from_file: error 1!\n");
                    exit(1);
                }
                
				// get the value of the XML node:
				{
					int i=0;

					do{
						buffer[i++]=c=fgetc(fp);
						if (i>=bufferSize) {
							char *buffer2 = new char[bufferSize*2];
							for(int j = 0;j<bufferSize;j++) buffer2[j]=buffer[j];
							delete []buffer;
							buffer = buffer2;
							bufferSize*=2;
						}
					}while(c!='<' && !feof(fp));

					if (i>0) i--;
					buffer[i]=0;
//					Symbol::arrange_string(buffer);
					if (strlen(buffer)>0) {
                        assert(node->m_value==0);
						node->m_value=new char[strlen(buffer)+1];
						strcpy(node->m_value,buffer);
					}
					if (c=='<') look_for_first_character=false;
						   else look_for_first_character=true;
				}
			
			} else {
				if (!stack.front()->m_type->cmp(node->m_type)) printf("XMLNode::from_file: Warning! XML mismatch: %s -> %s\n",
																stack.front()->m_type->get(),
																node->m_type->get());
				stack.pop_front();
                assert(node != first_node);
				delete node;
				look_for_first_character=true;
			} // if 
		
		} // if 

	}while(node!=0);
	delete []buffer;
	return first_node;
} /* XMLNode::from_file */ 


XMLNode *XMLNode::from_string(char *str,int *ppos)
{
	int bufferSize = 1024;
	char *buffer = new char[bufferSize];
	std::list<XMLNode *> stack;
	XMLNode *node,*first_node=0;
	int open;
	char c;
	int pos=*ppos;

	do {
		*ppos=pos;
		node=read_tag_from_string(str,ppos,&open);
		pos=*ppos;

		if (first_node==0) first_node=node;

		if (node!=0) {
			if (open==0 || open==1) {
				if (!stack.empty()) stack.front()->m_children.push_back(node);
				if (open==0) stack.push_front(node);

				// get the value of the XML node:
				{
					int i=0;

					do{
						buffer[i++]=c=str[pos++];
						if (i>=bufferSize) {
							char *buffer2 = new char[bufferSize*2];
							for(int j = 0;j<bufferSize;j++) buffer2[j]=buffer[j];
							delete []buffer;
							buffer = buffer2;
							bufferSize*=2;
						}
					}while(c!='<' && str[pos]!=0);

					if (i>0) i--;
					buffer[i]=0;
//					Symbol::arrange_string(buffer);
					if (strlen(buffer)>0) {
                        assert(node->m_value==0);
						node->m_value=new char[strlen(buffer)+1];
						strcpy(node->m_value,buffer);
					}
					if (c=='<') pos--;
				}
			
			} else {
				if (!stack.front()->m_type->cmp(node->m_type)) printf("XMLNode::from_file: Warning! XML mismatch: %s -> %s\n",
																stack.front()->m_type->get(),
																node->m_type->get());
				stack.pop_front();
                assert(node != first_node);
				delete node;
			} // if 
		} // if 

	}while(!stack.empty());
	delete []buffer;
	*ppos=pos;
	return first_node;
} /* XMLNode::from_string */ 


XMLNode *XMLNode::read_tag_from_file(FILE *fp,int *open,bool look_for_first_character)
{
	XMLNode *tmp;
	char buffer[1024];
	int i;
	char c;
	bool end_of_tag=false;

	*open=0;

	if (look_for_first_character) {
		// Look for the '<':
		do{
			c=fgetc(fp);
		}while(c!='<' && !feof(fp));
		if (feof(fp)) return 0;
	} // if 
	
	i=0;
	c=fgetc(fp);
	if (c=='/') {
		*open=2; 
	} else {
		buffer[i++]=c;
	} // if 

	tmp=new XMLNode();

	do{
		c=fgetc(fp);
		if (!end_of_tag) {
			buffer[i++]=c;
			if (c==' ') {
				end_of_tag=true;
				// look for attributes:
				buffer[i-1] = 0;
				if (buffer[0]=='!' && buffer[1]=='-' && buffer[2]=='-') {
					// comment:
                    int last = -1;
                    int lastlast = -1;
					do {
                        if (feof(fp)) {
                            delete tmp;
                            return 0;
                        }
                        lastlast = last;
                        last = c;
						c = fgetc(fp);
					} while(lastlast!='-' || last!='-' || c!='>');
					delete tmp;
					return read_tag_from_file(fp, open, true);
				} 
                if (strcmp(buffer,"?xml")==0) {
                    do {
                        c = fgetc(fp);
                    } while(c!='>');
                    delete tmp;
                    return read_tag_from_file(fp, open, true);
                } 
			}
		}
		if (end_of_tag) {
			if (c==' ') {
				int j=0;
				char buffer2[1024];				
				while(c==' ') c=fgetc(fp);
				if (c!='>' && c!='/') {
					buffer2[j++]=c;
					while(c!=' ' && c!='=') buffer2[j++]=c=fgetc(fp);

					if (j>0) j--;
					buffer2[j]=0;
					tmp->m_attribute_names.push_back(new Symbol(buffer2));
					j=0;

					while(c!='\"') c=fgetc(fp);
					buffer2[j++]=c=fgetc(fp);
					while(c!='\"') buffer2[j++]=c=fgetc(fp);
					if (j>0) j--;
					buffer2[j]=0;
                    char *buffer3 = replaceQuotes(buffer2);
					tmp->m_attribute_values.push_back(buffer3);
				}
			} // if 
		} // if 	
	}while(c!='>' && c!='/' && !feof(fp));
    if (feof(fp)) {
        delete tmp;
        return 0;
    }
	if (c=='/') {
		fgetc(fp);	// get the '>'
		*open = 1;
	}
	if (i>0) i--;
	buffer[i]=0;

	tmp->m_type=new Symbol(buffer);

	return tmp;
} /* XMLNode::read_tag_from_file */ 



XMLNode *XMLNode::read_tag_from_string(const char *str,int *ppos,int *open)
{
	XMLNode *tmp;
	char buffer[1024];
	int i;
	char c;
	bool end_of_tag=false;
	int pos=*ppos;

	*open=0;

	{
		// Look for the '<':
		do{
			c=str[pos++];
		}while(c!='<' && str[pos]!=0);
		if (str[pos]==0) {
			*ppos=pos;
			return 0;
		} // if 
	} // if 
	
	i=0;
	c=str[pos++];
	if (c=='/') {
		*open=2; 
	} else {
		buffer[i++]=c;
	} // if 

	tmp=new XMLNode();

	do{
		c=str[pos++];
		if (!end_of_tag) {
			buffer[i++]=c;
			if (c==' ') {
				end_of_tag=true;
			}
		}
		if (end_of_tag) {
			if (c==' ') {			
				// look for attributes:
				int j=0;
				char buffer2[1024];				
				while(c==' ') c=str[pos++];
				if (c!='>' && c!='/') {
					buffer2[j++]=c;
					while(c!=' ' && c!='=') buffer2[j++]=c=str[pos++];

					if (j>0) j--;
					buffer2[j]=0;
					tmp->m_attribute_names.push_back(new Symbol(buffer2));
					j=0;

					while(c!='\"') c=str[pos++];
					buffer2[j++]=c=str[pos++];
					while(c!='\"') buffer2[j++]=c=str[pos++];
					if (j>0) j--;
					buffer2[j]=0;
                    char *buffer3 = replaceQuotes(buffer2);
					tmp->m_attribute_values.push_back(buffer3);
				}
			} // if 
		} // if 	
	}while(c!='>' && c!='/' && str[pos]!=0);
	if (c=='/') {
		pos++; // read the '>'
		*open = 1;
	}
	if (str[pos]==0) {
		*ppos=pos;
        delete tmp;
		return 0;
	} // if
	if (i>0) i--;
	buffer[i]=0;

	tmp->m_type=new Symbol(buffer);

	*ppos=pos;
	return tmp;
} /* XMLNode::read_tag_from_string */ 


XMLNode::XMLNode(XMLNode *xml)
{
    m_type = new Symbol(xml->m_type);
    if (xml->m_value==0) {
        m_value = 0;
    } else {
        m_value = new char[strlen(xml->m_value)+1];
        strcpy(m_value, xml->m_value);
    }
    std::vector<Symbol *>::iterator i1 = xml->m_attribute_names.begin();
    std::vector<char *>::iterator i2 = xml->m_attribute_values.begin();
    for(;i1!=xml->m_attribute_names.end();i1++,i2++) {
        m_attribute_names.push_back(new Symbol(*i1));
        char *tmp = new char[strlen(*i2)+1];
        strcpy(tmp,*i2);
        m_attribute_values.push_back(tmp);
    }
    for(XMLNode *n:xml->m_children) {
        m_children.push_back(new XMLNode(n));
    }
}


XMLNode::XMLNode(void)
{
	m_type=0;
	m_value=0;

} /* XMLNode::XMLNode */ 


XMLNode::~XMLNode()
{
	if (m_type!=0) delete m_type;
	if (m_value!=0) delete m_value;
	for(Symbol *s:m_attribute_names) delete s;
	m_attribute_names.clear();
	for(char *s:m_attribute_values) delete s;
	m_attribute_values.clear();
	for(XMLNode *c:m_children) delete c;
	m_children.clear();

} /* XMLNode::~XMLNode */ 


Symbol *XMLNode::get_type(void)
{
	return m_type;
} /* XMLNode::get_type */ 


char *XMLNode::get_value(void)
{
	return m_value;
} /* XMLNode::get_value */ 


char *XMLNode::get_attribute(Symbol *name)
{
	std::vector<Symbol *>::iterator pos1 = m_attribute_names.begin();
	std::vector<char *>::iterator pos2 = m_attribute_values.begin();

	while(!(pos1==m_attribute_names.end()) && !(*pos1)->cmp(name)) {
		pos1++;
		pos2++;
	}

	if (pos1==m_attribute_names.end()) return 0;
	return *pos2;
} /* XMLNode::get_attribute */ 


char *XMLNode::get_attribute(const char *name)
{
	Symbol *n=new Symbol(name);
	char *res=get_attribute(n);
	delete n;
	return res;
} /* XMLNode::get_attribute */ 


void XMLNode::set_attribute(const char *name, const char *value)
{
	std::vector<Symbol *>::iterator pos1 = m_attribute_names.begin();
	std::vector<char *>::iterator pos2 = m_attribute_values.begin();

	while(!(pos1==m_attribute_names.end()) && !(*pos1)->cmp(name)) {
		pos1++;
		pos2++;
	}

	if (pos1==m_attribute_names.end()) {
		m_attribute_names.push_back(new Symbol(name));
		char *tmp = new char[strlen(value)+1];
		strcpy(tmp,value);
		m_attribute_values.push_back(tmp);
	} else {
		char *tmp = *pos2;
		delete tmp;
		tmp = new char[strlen(value)+1];
		strcpy(tmp,value);
		*pos2 = tmp;
	}
}


std::vector<XMLNode *> *XMLNode::get_children(void)
{
	return &m_children;
} /* XMLNode::get_children */ 


std::vector<XMLNode *> *XMLNode::get_children(Symbol *type)
{
	std::vector<XMLNode *> *tmp = new std::vector<XMLNode *>();
	for(XMLNode *c:m_children) {
		if (c->get_type()->cmp(type)) tmp->push_back(c);
	}
	return tmp;
}


std::vector<XMLNode *> *XMLNode::get_children(const char *type)
{
    Symbol *tmp_symbol = new Symbol(type);
	std::vector<XMLNode *> *tmp = get_children(tmp_symbol);
    delete tmp_symbol;
    return tmp;
}


void XMLNode::cleanValues() {
	if (m_value!=0) {
		int start = 0;
		for(int i = 0;m_value[i]!=0;i++, start++) {
			if (m_value[i]!=' ' && m_value[i]!='\t' &&
				m_value[i]!='\n' && m_value[i]!='\r') break;
		}

		int len = (int)strlen(m_value+start);

		if (len==0) {
			delete m_value;
			m_value = 0;
		} else {
			char *tmp = new char[len+1];
			strcpy(tmp,m_value+start);
			delete m_value;
			m_value = tmp;
		}
	}

	for(XMLNode *n:m_children) n->cleanValues();
}

void XMLNode::print(int tabs)
{
	int i;

	for(i=0;i<tabs;i++) printf("  ");
	printf("<%s",m_type->get());

	std::vector<Symbol *>::iterator pos1 = m_attribute_names.begin();
	std::vector<char *>::iterator pos2 = m_attribute_values.begin();

	while(!(pos1==m_attribute_names.end())) {
		printf(" %s = \"%s\"",(*pos1)->get(),*pos2);
		pos1++;
		pos2++;
	} // while 
	printf(">\n");

	if (m_value!=0) {
		for(i=0;i<tabs+1;i++) printf("  ");
		printf("%s\n",m_value);
	} // if 

	for(XMLNode *n:m_children) n->print(tabs+1);

	for(i=0;i<tabs;i++) printf("  ");
	printf("</%s>\n",m_type->get());
} /* XMLNode::print */ 


void XMLNode::writeToFile(FILE *fp, int tabs)
{
	int i;

	for(i=0;i<tabs;i++) fprintf(fp," ");
	fprintf(fp,"<%s",m_type->get());

	std::vector<Symbol *>::iterator pos1 = m_attribute_names.begin();
	std::vector<char *>::iterator pos2 = m_attribute_values.begin();

	while(!(pos1==m_attribute_names.end())) {
		fprintf(fp," %s=\"",(*pos1)->get());
		writeAttributeValueToFile(fp,*pos2);
		fprintf(fp,"\"");
		pos1++;
		pos2++;
	} // while 	
	bool reindent = false;
	if (m_value!=0) {
		fprintf(fp,">");
		if (m_children.size()==0) {
			fprintf(fp,"%s",m_value);
		} else {
			fprintf(fp,"\n");
			for(i=0;i<tabs+1;i++) fprintf(fp," ");
			fprintf(fp,"%s\n",m_value);			
			for(XMLNode *n:m_children) n->writeToFile(fp,tabs+1);
			reindent = true;
		}
		if (reindent) for(i=0;i<tabs;i++) fprintf(fp," ");
		fprintf(fp,"</%s>\n",m_type->get());	
	} else {
		if (m_children.size()==0) {
			fprintf(fp,"/>\n");
		} else {
			fprintf(fp,">\n");
			for(XMLNode *n:m_children) {
				n->writeToFile(fp,tabs+1);
				reindent = true;
			}
			if (reindent) for(i=0;i<tabs;i++) fprintf(fp," ");
			fprintf(fp,"</%s>\n",m_type->get());	
		}
	}
}

void XMLNode::writeAttributeValueToFile(FILE *fp, char *value)
{
	for(int i = 0;value[i]!=0;i++) {
		if (value[i]=='\"') {
			fprintf(fp,"&quot;");
        } else if (value[i]=='\'') {
            fprintf(fp,"&#39;");
		} else {
			fprintf(fp,"%c",value[i]);
		}
	}
}


XMLNode *XMLNode::get_child(Symbol *type)
{
	for(XMLNode *c:m_children) {
		if (c->get_type()->cmp(type)) return c;
	} // while 

	return 0;
} /* XMLNode::get_child */ 


XMLNode *XMLNode::get_child(const char *type)
{
	for(XMLNode *c:m_children) {
		if (c->get_type()->cmp(type)) return c;
	} // while 

	return 0;
} /* XMLNode::get_child */ 


// Note: this function is not very efficient, but should only be called after loading from disk,
//       so, no big deal.
char *XMLNode::replaceQuotes(const char *orig)
{
    char *tmp = replaceString(orig, "&quot;", "\"");
    char *result = replaceString(orig, "&#39;", "\'");
    delete tmp;
    return result;
}


void XMLNode::saveToXML(XMLwriter *w)
{
    w->openTag(m_type->get());

    std::vector<Symbol *>::iterator pos1 = m_attribute_names.begin();
    std::vector<char *>::iterator pos2 = m_attribute_values.begin();
    while(!(pos1==m_attribute_names.end())) {
        w->setAttribute((*pos1)->get(), *pos2);
        pos1++;
        pos2++;
    } // while

    if (m_value!=0) w->addContent(m_value);
    for(XMLNode *n:m_children) n->saveToXML(w);
    w->closeTag(m_type->get());
}

