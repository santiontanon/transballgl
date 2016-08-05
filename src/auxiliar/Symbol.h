#ifndef __TEXT_SYMBOL
#define __TEXT_SYMBOL

#include "stdio.h"

#define SYMBOL_HASH_SIZE	1024
#define MAXSYMBOLLENGTH	1024

/*
#ifndef WIN32
#ifndef HAVE_STRLWR
char *strlwr(char *s);
#endif
#endif
*/

/* This class must only be used from 'Symbol': */ 

class SymbolContainer {
	
	friend class Symbol;

public:
	SymbolContainer(const char *str, bool cleanString);
    SymbolContainer(const char *str, int hash);
	~SymbolContainer();

	char *get(void);

	bool cmp(const char *str);

    static int hash_function(const char *str);
private:

	int m_reference_count;
	int m_hash;
	char *m_name;
}; /* SymbolContainer */ 



class Symbol {
public:
//	Symbol();
	Symbol(const char *str);
	Symbol(Symbol &s);
	Symbol(Symbol *s);
	~Symbol();

	static void reset(void);

	char *get(void);
	void set(const char *str);

	bool cmp(const char *str);
	bool cmp(Symbol *s);
	bool cmp(Symbol &s);
	bool operator==(Symbol &s);

	static void arrange_string(char *str);
	static int stats_nSymbols(void);
	static int stats_nSymbolReferences(void);
	static void stats_printSymbolStats(void);

	int hash_function(void);

private:
	SymbolContainer *sym;

}; /* Symbol */ 


#endif
