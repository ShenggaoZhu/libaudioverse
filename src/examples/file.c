#include <libaudioverse/libaudioverse.h>
#include <libaudioverse/private_all.h> //our cross-platform sleep.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** args) {
	if(argc != 2) {
		printf("Syntax: %s <path>", args[0]);
		return 0;
	}

	char* path = args[1];
	LavNode *node;
	LavGraph *graph;
	Lav_createGraph(44100, &graph);
	LavError err = Lav_createFileNode(graph, path, &node);
	if(err != Lav_ERROR_NONE) {
		printf("Error: %d", err);
		return 1;
	}


	Lav_graphSetOutputNode(graph, node);
	void* th;
	createAudioOutputThread(graph, 1024, 3, &th);
	char pause[100];
	fgets(pause, 100, stdin);
	stopAudioOutputThread(th);
	return 0;
}