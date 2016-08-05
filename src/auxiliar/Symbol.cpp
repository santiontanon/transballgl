#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "assert.h"

#include <list>
#include "debug.h"

#include "Symbol.h"


std::list<SymbolContainer *> *symbol_hash=0;


#ifndef WIN32
#include "ctype.h"
/*
#ifndef HAVE_STRLWR

char *strlwr(char *s)
{
	if (!s) return NULL;
	while ( *s ) { 
		*s = (char) tolower( *s ); 
		++s;
	}
	return s;
}
#endif
*/
#endif



void Symbol::reset(void)
{
	delete []symbol_hash;
	symbol_hash=0;
} /* Symbol::reset */ 

/*
Symbol::Symbol()
{
	int hv=0;
	
	if (symbol_hash==0) symbol_hash=new std::list<SymbolContainer *>[SYMBOL_HASH_SIZE];
	for(SymbolContainer *s:symbol_hash[hv]) {
		if (s->m_name==0) {
			s->m_reference_count++;
//			assert(s->m_reference_count<32768);
			sym=s;
			return;
		} // if
	} // while

	sym=new SymbolContainer(0, 0);
	sym->m_reference_count++;
	symbol_hash[hv].push_back(sym);
} // Symbol::Symbol
*/


Symbol::Symbol(const char *str)
{
	int hv=SymbolContainer::hash_function(str);

	if (symbol_hash==0) symbol_hash=new std::list<SymbolContainer *>[SYMBOL_HASH_SIZE];
	for(SymbolContainer *s:symbol_hash[hv]) {
		if (s->cmp(str)) {
			s->m_reference_count++;
//			assert(s->m_reference_count<32768);
			sym=s;
			return;
		} /* if */ 
	} /* while */ 

	sym=new SymbolContainer(str, hv);
	sym->m_reference_count++;
	symbol_hash[hv].push_back(sym);
} /* Symbol::Symbol */ 


Symbol::Symbol(Symbol &s)
{
	sym=s.sym;
	sym->m_reference_count++;
} /* Symbol::Symbol */ 


Symbol::Symbol(Symbol *s)
{
	sym=s->sym;
	sym->m_reference_count++;
} /* Symbol::Symbol */ 


Symbol::~Symbol()
{
	if (sym!=0) {
		assert(sym->m_reference_count>0);
		sym->m_reference_count--;
//		output_debug_message("freeing symbol: %s, count: %i\n", sym->m_name, sym->m_reference_count);
		if (sym->m_reference_count<=0) {
			symbol_hash[sym->m_hash].remove(sym);
//            symbol_hash[SymbolContainer::hash_function(sym->m_name)].remove(sym);
			delete sym;
		} /* if */ 
	} /* if */ 
	
} /* Symbol::~Symbol */ 


char *Symbol::get(void)
{
	if (sym!=0) return sym->get();
	return 0;
} /* Symbol::get */ 


void Symbol::set(const char *str)
{
	int hv=SymbolContainer::hash_function(str);

	if (sym!=0) {
		sym->m_reference_count--;
		if (sym->m_reference_count<=0) {
			symbol_hash[sym->m_hash].remove(sym);
//            symbol_hash[SymbolContainer::hash_function(sym->m_name)].remove(sym);
			delete sym;
		} /* if */ 
	} /* if */ 

	if (symbol_hash==0) symbol_hash=new std::list<SymbolContainer *>[SYMBOL_HASH_SIZE];
	for(SymbolContainer *s:symbol_hash[hv]) {
		if (s->cmp(str)) {
			s->m_reference_count++;
			sym=s;
			return;
		} /* if */ 
	} /* while */ 

	sym=new SymbolContainer(str, hv);
	sym->m_reference_count++;
	symbol_hash[hv].push_back(sym);
} /* Symbol::set */ 


bool Symbol::cmp(const char *str)
{
	if (sym==0) return false;
	if (str==sym->m_name) return true;
	if (str==0 || sym->m_name==0) return false;
	if (strcmp(str,sym->m_name)==0) return true;
	return false;
} /* Symbol::cmp */ 


bool Symbol::cmp(Symbol *s)
{
    if (s==0) return false;
	return sym==s->sym;
} /* Symbol::cmp */ 


bool Symbol::cmp(Symbol &s)
{
	return sym==s.sym;
} /* Symbol::cmp */ 


bool Symbol::operator==(Symbol &s)
{
	return sym==s.sym;
} /* Symbol::operator== */ 


void Symbol::arrange_string(char *str)
{
	int len;

	while(str[0]==' ' || str[0]=='\n' || str[0]=='\r' ||
		  str[0]=='\t') strcpy(str,str+1);

	len=(int)strlen(str);
	while(len>1 && 
		  (str[len-1]==' ' || str[len-1]=='\n' || str[len-1]=='\r' ||
		  str[len-1]=='\t')) {
		str[len-1]=0;
		len--;
	} /* while */ 
} /* Symbol::arrange_string */ 


int Symbol::stats_nSymbols(void)
{
	int i;
	int c=0;

	for(i=0;i<SYMBOL_HASH_SIZE;i++) {
		c+=symbol_hash[i].size();
	} /* for */ 

	return c;
} /* Symbol::stats_nSymbols */ 


int Symbol::stats_nSymbolReferences(void)
{
	int i;
	int c=0;

	for(i=0;i<SYMBOL_HASH_SIZE;i++) {
		for(SymbolContainer *s:symbol_hash[i]) {
			c+=s->m_reference_count;
		} /* while */ 
	} /* for */ 

	return c;
} /* Symbol::stats_nSymbolReferences */ 


void Symbol::stats_printSymbolStats(void)
{
	int i;
	int c=0;

	for(i=0;i<SYMBOL_HASH_SIZE;i++) {
		for(SymbolContainer *s:symbol_hash[i]) {
			printf("Symbol \"%s\" -> %i\n",s->get(),s->m_reference_count);
			c+=s->m_reference_count;
		} /* while */ 
	} /* for */ 

	printf("Total: %i\n",c);
} /* Symbol::stats_printSymbolStats */ 




/* ---- SymbolCotnainer ---------------------------- */ 

SymbolContainer::SymbolContainer(const char *str, bool cleanString)
{
	if (str==0) {
		m_reference_count=0;
		m_name=0;
		return;
	} // if

    if (cleanString) {
        // Clean the string (remove starting and trailing spaces)
        char *nstr;
        int state=0,i,j;
        char byte;

        nstr=new char[strlen(str)+1];
        i=j=0;
        do{
            byte=str[i++];
            switch(state) {
            case 0:
                if (byte!=' ' && byte!=10 && byte!=13) {
                    nstr[j++]=byte;
                    state=1;
                } // if
                break;
            case 1:
                if (byte==' ' || byte==10 || byte==13) {
                    nstr[j++]=byte;
                    state=0;
                } else {
                    nstr[j++]=byte;
                } // if
                break;
            } // switch
        }while(byte!=0);
        
        m_reference_count=0;
        m_name=new char[strlen(nstr)+1];
        m_hash=hash_function(nstr);
        strcpy(m_name,nstr);
        delete []nstr;
    } else {
        m_reference_count=0;
        m_name=new char[strlen(str)+1];
        m_hash=hash_function(str);
        strcpy(m_name,str);
    }
} // SymbolContainer::SymbolContainer


SymbolContainer::SymbolContainer(const char *str, int hash)
{
    if (str==0) {
        m_reference_count=0;
        m_name=0;
        return;
    } // if

    assert(hash == hash_function(str));

    m_reference_count=0;
    m_name=new char[strlen(str)+1];
    m_hash=hash;
    strcpy(m_name,str);
} // SymbolContainer::SymbolContainer


SymbolContainer::~SymbolContainer() 
{
	delete []m_name;
	m_name=0;
} /* SymbolContainer::~SymbolContainer */ 


char *SymbolContainer::get(void)
{
	assert(m_reference_count>0);
	return m_name;
} /* SymbolContainer::get */ 


bool SymbolContainer::cmp(const char *str)
{
	if (str==m_name) return true;
	if (str==0 || m_name==0) return false;
	if (strcmp(str,m_name)==0) return true;
	return false;
} /* SymbolContainer::cmp */ 


int SymbolContainer::hash_function(const char *str)
{
	int v=0;
	int l=0;

	if (str==0) return 0;
		
	l=(int)strlen(str);

	if (l>0) {
		v+=str[0]*3;
		if (l>1) {
			v+=str[1]*5;
			if (l>2) {
				v+=str[2]*7;
				if (l>3) {
					v+=str[3]*11;
					if (l>4) {
						v+=str[4]*13;
						if (l>5) {
							v+=str[5]*17;
							if (l>6) {
								v+=str[6]*19;
								if (l>7) {
									v+=str[7]*23;
									if (l>8) {
										v+=str[8]*29;
										if (l>9) {
											v+=str[9]*31;
											if (l>10) {
												v+=str[10]*37;
												if (l>11) {
													v+=str[11]*41;
													if (l>12) {
														v+=str[12]*43;
													} /* if */ 
												} /* if */ 
											} /* if */ 
										} /* if */ 
									} /* if */ 
								} /* if */ 
							} /*  if */ 
						} /* if */ 
					} /* if */ 
				} /* if */ 
			} /* if */ 
		} /* if */ 
	} /* If */ 
	return v%SYMBOL_HASH_SIZE;
} /* SymbolContainer */ 


int Symbol::hash_function(void)
{
	return sym->m_hash;
} /* Symbol::hash_function */ 

