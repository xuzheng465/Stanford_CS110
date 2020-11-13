/**
 * File: html-document.cc
 * ----------------------
 * Presents the implementation for the HTMLDocument class,
 * which relies on libxml2's ability to parse a single document 
 * and extract the textual content from the body tag.
 */

#include <iostream>
#include <vector>
#include <cassert>
#include <sstream>

#include <libxml/tree.h>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "html-document.h"
#include "html-document-exception.h"
#include "stream-tokenizer.h"

using namespace std;

static const int kHTMLParseFlags = 
  HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET;
static const string kDelimiters = " \t\n\r\b!@#$%^&*()_-+=~`{[}]|\\\"':;<,>.?/";

void HTMLDocument::parse() throw (HTMLDocumentException) {
  htmlDocPtr doc = htmlReadFile(url.c_str(), /* encoding = */ NULL, kHTMLParseFlags);
  if (doc == NULL) {
    // This is the only real user error we handle with any frequency, as it's
    // completely reasonable that the client more than occasionally specify a bogus URL.
    ostringstream oss;
    oss << "Error: unable to parse the document at \"" << url << "\".";
    throw HTMLDocumentException(oss.str());
  }
  
  xmlXPathContextPtr context = xmlXPathNewContext(doc);
  const xmlChar *expr = BAD_CAST "//body";
  xmlXPathObjectPtr bodies = xmlXPathEvalExpression(expr, context);
  xmlNodeSetPtr bodyNodes = bodies->nodesetval;
  int numBodyTags = bodyNodes != NULL ? bodyNodes->nodeNr : 0;
  for (int i = 0; i < numBodyTags; i++) { // should only be one body tag, but whatever
    xmlChar *rawContent = xmlNodeGetContent(bodyNodes->nodeTab[i]);
    string bodyContent = (const char *) rawContent;
    xmlFree(rawContent);
    istringstream iss(bodyContent);
    StreamTokenizer st(iss, kDelimiters, /* skipDelimiters = */ true);
    while (st.hasMoreTokens()) {
      string token = st.nextToken();
      tokens.push_back(token);
    }
  }
  
  xmlXPathFreeObject(bodies);
  xmlXPathFreeContext(context); 
  xmlFreeDoc(doc);     
}
