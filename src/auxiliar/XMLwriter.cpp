//
//  XMLwriter.cpp
//  A4Engine
//
//  Created by Santiago Ontanon on 7/5/15.
//  Copyright (c) 2015 Santiago Ontanon. All rights reserved.
//

#include "stdarg.h"
#include "string.h"
#include "XMLwriter.h"

XMLfileWriter::XMLfileWriter(const char *fileName, int tabsize)
{
    fp = fopen(fileName,"w+");

    m_tabsize = tabsize;
    m_tabs = 0;
    m_tagOpen = false;
    m_hasContent = false;
}


XMLfileWriter::~XMLfileWriter()
{
    fclose(fp);
    fp = 0;
}


void XMLfileWriter::openTag(const char *name)
{
    if (m_tagOpen) fprintf(fp,">\n");
    for(int i = 0;i<m_tabs;i++) fprintf(fp," ");
    m_tabs+=m_tabsize;
    fprintf(fp,"<%s",name);
    m_tagOpen = true;
    m_hasContent = false;
}


void XMLfileWriter::closeTag(const char *name)
{
    m_tabs-=m_tabsize;
    if (m_tagOpen) {
        fprintf(fp,"/>\n");
    } else {
        if (!m_hasContent) for(int i = 0;i<m_tabs;i++) fprintf(fp," ");
        fprintf(fp,"</%s>\n",name);
    }
    m_tagOpen = false;
    m_hasContent = false;
}


void XMLfileWriter::setAttribute(const char *name, const char *value)
{
    if (strchr(value,'\"')!=0 || strchr(value,'\'')!=0) {
        fprintf(fp," %s=\"",name);
        for(int i = 0;i<(int)strlen(value);i++) {
            if (value[i]=='\"') {
                fprintf(fp,"&quot;");
            } else if (value[i]=='\'') {
                fprintf(fp,"&#39;");
            } else {
                fprintf(fp,"%c",value[i]);
            }
        }
        fprintf(fp,"\"");
    } else {
        fprintf(fp," %s=\"%s\"",name,value);
    }
}


void XMLfileWriter::setAttribute(const char *name, int value)
{
    fprintf(fp," %s=\"%i\"",name,value);
}

void XMLfileWriter::setAttribute(const char *name, bool value)
{
    fprintf(fp," %s=\"%s\"",name,value ? "true":"false");
}

void XMLfileWriter::setAttribute(const char *name, float value)
{
    fprintf(fp," %s=\"%g\"",name,value);
}

void XMLfileWriter::setAttribute(const char *name, double value)
{
    fprintf(fp," %s=\"%g\"",name,value);
}


void XMLfileWriter::addContent(const char *fmt, ...)
{
    if (m_tagOpen) fprintf(fp,">");

    if (fmt == 0) return;
    char text[1024];
    va_list ap;

    va_start(ap, fmt);
    vsprintf(text, fmt, ap);
    va_end(ap);

    fprintf(fp, "%s", text);

    m_hasContent = true;
    m_tagOpen = false;
}

