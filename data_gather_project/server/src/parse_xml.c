#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include "link.h"

/**
 ** example3Func:
 ** @content: the content of the document
 ** @length: the length in bytes
 **
 ** Parse the in memory document and free the resulting tree
 **/
static int
example3Func(const char *content, int length, struct rx_info *node) {

	xmlDocPtr doc;    /* the resulting document tree */
	xmlNodePtr cur;
	xmlChar *key;

	/*
	 ** The content being in memory, it have no base per RFC 2396,
	 ** and the "noname.xml" argument will serve as its base.
	 **/

	doc = xmlReadMemory(content, length, "noname.xml", NULL, 0);
	if (doc == NULL) {
		fprintf(stderr, "Failed to parse document\n");
		return;
	}

	cur = xmlDocGetRootElement(doc);

	if (cur == NULL)
	{
		fprintf(stderr,"(*)hjw: empty document\n");
		xmlFreeDoc(doc);
		return 0;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "sensor"))
	{
		fprintf(stderr,"(*)hjw: document of the wrong type, root node != dtv");
		xmlFreeDoc(doc);
		return 0;
	}

	cur = cur->xmlChildrenNode;
	while (cur != NULL)
	{
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"name")))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			strcpy(node->name,(char *)key);
			xmlFree(key);
		}   
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"vol")))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			node->vol = atof((char *)key);
			xmlFree(key);
		}   
		else if ((!xmlStrcmp(cur->name, (const xmlChar *)"time")))
		{
			key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			strcpy(node->time,(char *)key);
			xmlFree(key);
		}   

		cur = cur->next;
	}

	xmlFreeDoc(doc);

	return 1;
}

int parse_xml(const char *buf,struct rx_info *node) {
	/*
	 ** this initialize the library and check potential ABI mismatches
	 ** between the version it was compiled for and the actual shared
	 ** library used.
	 **/
	LIBXML_TEST_VERSION

		example3Func(buf,strlen(buf),node);

	/*
	 * * Cleanup function for the XML library.
	 * */

	xmlCleanupParser();
	/*
	 ** this is to debug memory for regression tests
	 **/

	xmlMemoryDump();

	return(0);
}
