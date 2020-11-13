/**
 * File: rss-feed-list.cc
 * ----------------------
 * Presents the implementation of RSSFeedList::parse, which relies
 * on libxml2 to pull the original feed list URL, parse it, and compile
 * a article-URL-to-article-title map.
 */

#include "rss-feed-list.h"

#include <iostream>
#include <sstream>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "rss-feed-list-exception.h"

using namespace std;

static const int XML_PARSE_FLAGS = XML_PARSE_NOBLANKS | XML_PARSE_NOERROR | XML_PARSE_NOWARNING;

void RSSFeedList::parse() throw (RSSFeedListException) {
  xmlDocPtr doc = xmlReadFile(url.c_str(), /* encoding = */ NULL, XML_PARSE_FLAGS);
  if (doc == NULL) {
    // This is the only real user error we handle with any frequency, as it's
    // completely reasonable that the client more than occasionally specifies a bogus URL.
    basic_ostringstream<char> oss;
    oss << "Error: unable to parse the RSS feed list at \"" << url << "\".";
    throw RSSFeedListException(oss.str());
  }
  
  xmlXPathContextPtr context = xmlXPathNewContext(doc);
  const xmlChar *expr = BAD_CAST "//item";
  xmlXPathObjectPtr items = xmlXPathEvalExpression(expr, context);
  xmlNodeSetPtr itemNodes = items->nodesetval;
  int numItems = itemNodes != NULL ? itemNodes->nodeNr : 0;
  for (int i = 0; i < numItems; i++) {
    context->node = itemNodes->nodeTab[i];
    const xmlChar *titleSubExpr = BAD_CAST "title";
    xmlXPathObjectPtr titles = xmlXPathEvalExpression(titleSubExpr, context);
    xmlChar *title =
      (titles->nodesetval == NULL || titles->nodesetval->nodeNr == 0) ? xmlCharStrdup("") : xmlNodeGetContent(titles->nodesetval->nodeTab[0]);
    xmlXPathFreeObject(titles);
    const xmlChar *linkSubExpr = BAD_CAST "link";
    xmlXPathObjectPtr links = xmlXPathEvalExpression(linkSubExpr, context);
    xmlChar *link =
      (links->nodesetval == NULL || links->nodesetval->nodeNr == 0) ? xmlCharStrdup("") : xmlNodeGetContent(links->nodesetval->nodeTab[0]);
    feeds[(const char *) link] = (const char *) title;
    xmlFree(title);
    xmlFree(link);
    xmlXPathFreeObject(links);
  }
  
  xmlXPathFreeObject(items);
  xmlXPathFreeContext(context); 
  xmlFreeDoc(doc);     
}
