#include <windows.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <CGTFile.h>

#include "shader_compiler/SL_ASTCreator.h"
#include "shader_compiler/SL_ASTNodes.h"
#include "shader_compiler/SL_ASTVisitor.h"
#include "shader_compiler/stdout_visitor.h"

#include "scene/scene.h"

using namespace std;

int main(int argc, char **argv)
{
	CGTFile f;
	bool cgtLoaded = f.load("./src/shadervm/crtsl.cgt");

	ifstream fileIn("./Shaders/sample.crtsl");
	fileIn.seekg(0, ios::end);
	int size = fileIn.tellg();
	fileIn.seekg(0, ios::beg);
	char *buffer = new char[size+1];
	memset(buffer, 0, size+1);
	fileIn.read(buffer, size);

	DFA *scanner = f.getScanner();
	bool scanSuccess = scanner->scan(buffer);
	vector<Token*>& tokens = scanner->getTokens();

	LALR *parser = f.getParser();
	parser->init(tokens);
	Symbol *sym = parser->parse(tokens, true, false);
	//parser->printReductionTree(sym, 10);

	SL_ASTCreator astCreator;
	FileRootNode *root = (FileRootNode*)(astCreator.createTree(*sym));

	StdoutVisitor v;
	v.visit(*root);
	
	delete buffer;
	return 0;

	/*string scnFile = "ScnSamples/specular_shaded_sphere.crtscn";

	Scene scn;
	bool scnLoaded = scn.loadScnFile(scnFile);
	if (scnLoaded)
	{
		cout << endl << "Parsed \"" << scnFile << "\" successfully!" << endl;
		cout << "Start rendering..." << endl;
		__int64 begin = timeGetTime();
		scn.render();
		__int64 end = timeGetTime();
		cout << "Finished rendering, time: " << end - begin << " ms" << endl;
	}
	else
		cout << "Loading " << scnFile << " failed!" << endl;
	
	cout << endl << "Press return to quit..." << endl;
	cin.get();

	return !scnLoaded;*/
}