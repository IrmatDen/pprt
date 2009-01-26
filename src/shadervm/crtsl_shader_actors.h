#ifndef CRT_CRTSL_SHADER_ACTORS_H
#define CRT_CRTSL_SHADER_ACTORS_H

// IMPORTANT: this file can't be used alone, it's only to be included in crtslparser.h which
// contains the necessary includes and typedefs!

#include <vector>
typedef std::vector<std::string> symbolTable;

struct newShader_a
{
	void operator()(const iterator_t &first, const iterator_t &last) const
	{
		std::cout << "Shader found:\t\t" << std::string(first, last) << std::endl;
	}
};

struct shaderType_a
{
	void operator()(const iterator_t &first, const iterator_t &last) const
	{
		std::cout << "Shader type:\t\t" << std::string(first, last) << std::endl;
	}
};

struct funcName_a
{
	void operator()(const iterator_t &first, const iterator_t &last) const
	{
		std::cout << "Function name:\t\t" << std::string(first, last) << std::endl;
	}
};

struct addVar_a
{
	void operator()(const iterator_t &first, const iterator_t &last) const
	{
		std::cout << "Variable:\t\t" << std::string(first, last) << std::endl;
	}
};

struct assignOp_a
{
	void operator()(const iterator_t &first, const iterator_t &last) const
	{
		std::cout << "POP" << std::endl;
	}
};

struct procCall_a
{
	void operator()(const iterator_t &first, const iterator_t &last) const
	{
		std::cout << "CALL <??>" << std::endl;
	}
};

struct pushArg_a
{
	void operator()(const iterator_t &first, const iterator_t &last) const
	{
		std::cout << "PUSH " << std::string(first, last) << std::endl;
	}

	template<typename ValueT>
	void operator()(const ValueT &v) const
	{
		std::cout << "PUSH " << v << std::endl;
	}
};


#endif
