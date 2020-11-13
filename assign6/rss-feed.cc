/**
 * File: rss-feed.cc
 * -----------------
 * Provides implementation of the RSSFeed::parse method.
 */

#include "rss-feed.h"

#include <iostream>
#include <vector>
#include <cassert>
#include <sstream>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "rss-feed-exception.h"
#include "string-utils.h"

using namespace std;

static const int XML_PARSE_FLAGS = XML_PARSE_NOBLANKS | XML_PARSE_NOERROR | XML_PARSE_NOWARNING;

void RSSFeed::parse() throw (RSSFeedException) {
  xmlDocPtr doc = xmlReadFile(url.c_str(), /* encoding = */ NULL, XML_PARSE_FLAGS);
  if (doc == NULL) {
    // This is the only real user error we handle with any frequency, as it's
    // completely reasonable that the client more than occasionally specify a bogus URL.
    basic_ostringstream<char> oss;
    oss << "Error: unable to parse the RSS feed at \"" << url << "\".";
    throw RSSFeedException(oss.str());
  }
  
  xmlXPathContextPtr context = xmlXPathNewContext(doc);
  const xmlChar *expr = BAD_CAST "//item";
  xmlXPathObjectPtr items = xmlXPathEvalExpression(expr, context);
  xmlNodeSetPtr itemNodes = items->nodesetval;
  int numItems = itemNodes != NULL ? itemNodes->nodeNr : 0;
  for (int i = 0; i < numItems; i++) {
    Article a;
    context->node = itemNodes->nodeTab[i];
    const xmlChar *titleSubExpr = BAD_CAST "title";
    xmlXPathObjectPtr titles = xmlXPathEvalExpression(titleSubExpr, context);
    xmlChar *title =
      (titles->nodesetval == NULL || titles->nodesetval->nodeNr == 0) ? xmlCharStrdup("") : xmlNodeGetContent(titles->nodesetval->nodeTab[0]);
    a.title = (const char *) title;
    trim(a.title);
    xmlFree(title);
    xmlXPathFreeObject(titles);
    const xmlChar *linkSubExpr = BAD_CAST "link";
    xmlXPathObjectPtr links = xmlXPathEvalExpression(linkSubExpr, context);
    xmlChar *link =
      (links->nodesetval == NULL || links->nodesetval->nodeNr == 0) ? xmlCharStrdup("") : xmlNodeGetContent(links->nodesetval->nodeTab[0]);
    a.url = (const char *) link;
    trim(a.url);
    xmlFree(link);
    xmlXPathFreeObject(links);
    articles.push_back(a);
  }
  
  xmlXPathFreeObject(items);
  xmlXPathFreeContext(context); 
  xmlFreeDoc(doc);     
}
