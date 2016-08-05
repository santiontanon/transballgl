//
//  XMLwriter.h
//  A4Engine
//
//  Created by Santiago Ontanon on 7/5/15.
//  Copyright (c) 2015 Santiago Ontanon. All rights reserved.
//

#ifndef __A4Engine__XMLwriter__
#define __A4Engine__XMLwriter__

#include "stdio.h"

class XMLwriter {
public:
    virtual void openTag(const char *name) = 0;
    virtual void closeTag(const char *name) = 0;

    virtual void setAttribute(const char *name, const char *value) = 0;
    virtual void setAttribute(const char *name, int value) = 0;
    virtual void setAttribute(const char *name, float value) = 0;
    virtual void setAttribute(const char *name, double value) = 0;
    virtual void setAttribute(const char *name, bool value) = 0;
    
    virtual void addContent(const char *fmt, ...) = 0;
private:
    
};


class XMLfileWriter : public XMLwriter {
public:
    XMLfileWriter(const char *fileName, int tabsize);
    ~XMLfileWriter();
    
    virtual void openTag(const char *name);
    virtual void closeTag(const char *name);
    
    virtual void setAttribute(const char *name, const char *value);
    virtual void setAttribute(const char *name, int value);
    virtual void setAttribute(const char *name, float value);
    virtual void setAttribute(const char *name, double value);
    virtual void setAttribute(const char *name, bool value);
    
    virtual void addContent(const char *fmt, ...);
private:
    FILE *fp;
    
    int m_tabsize;
    int m_tabs;
    bool m_tagOpen;
    bool m_hasContent;
};

#endif /* defined(__A4Engine__XMLwriter__) */
