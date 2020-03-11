#include <stdio.h>
#include <stdlib.h>
#include <simplexml.h>


int main(void)
{
	char s[1024];
	simplexml *root;
	simplexml *ptr;

	sprintf(s,"
<notes>
	<doe name=\"bambi\">
		<fawn>
			<!-- ignore me -->
			child of a doe
		</fawn>
	</doe>
	<ray>a drop of golden sun</ray>
	<me>a name I call myself</me>
	<fa>a long, long way to run</fa>
</notes>
");

	root = new simplexml(s);
	printf("first\n");

	ptr = root->child("doe");
	printf("doe:name = %s\n",ptr->property("name"));
	printf("fawn = %s\n",ptr->child("fawn")->value());

	delete root;

	root = new simplexml(s);
	printf("second\n");
	delete root;

	root = new simplexml(s);
	printf("third\n");
	delete root;

	exit(0);
}
