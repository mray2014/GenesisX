#include "ShaderManager.h"



ShaderManager::ShaderManager()
{
	regProgram = 0;
	skyProgram = 0;
	geoProgram = 0;
	ligProgram = 0;
}


ShaderManager::~ShaderManager()
{

}

GLuint ShaderManager::GetProgram() const
{
	return geoProgram;
}

bool ShaderManager::LoadShaders(const char * vFile, const char * fFile, GLuint &program)
{
	GLuint vertexShader = LoadShader(vFile, GL_VERTEX_SHADER);
	GLuint fragShader = LoadShader(fFile, GL_FRAGMENT_SHADER);

	program = glCreateProgram();
	glAttachShader(program,vertexShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	GLint result;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if(result == 0)
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &result);
		GLchar* debug = new GLchar[result];

		glGetProgramInfoLog(program, result, 0, debug);
		printf(debug);
		glDeleteProgram(program);
		delete[] debug; debug = nullptr;
		return false;
	}
	return true;
}

bool ShaderManager::LoadLightShader(const char * fFile, GLuint & program)
{
	GLuint fragShader = LoadShader(fFile, GL_FRAGMENT_SHADER);

	program = glCreateProgram();
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	GLint result;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (result == 0)
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &result);
		GLchar* debug = new GLchar[result];

		glGetProgramInfoLog(program, result, 0, debug);
		printf(debug);
		glDeleteProgram(program);
		delete[] debug; debug = nullptr;
		return false;
	}
	return true;
}

GLuint ShaderManager::LoadShader(const char * file, GLenum shaderType)
{
	using namespace std;
	ifstream inFile(file, ios::binary);
	GLuint prog = 0;

	if(inFile.is_open())
	{
		inFile.seekg(0, ios::end);
		int lenght = (int)inFile.tellg();

		inFile.seekg(0, ios::beg);
		char* fileContent = new char[lenght + 1];

		inFile.read(fileContent, lenght);
		fileContent[lenght] = 0;

		inFile.close();

		prog = glCreateShader(shaderType);
		glShaderSource(prog, 1, &fileContent,0);

		glCompileShader(prog);

		GLint result;
		glGetShaderiv(prog, GL_COMPILE_STATUS, &result);

		if(result == 0)
		{
			
			glGetShaderiv(prog, GL_INFO_LOG_LENGTH, &result);
			GLchar* debug = new GLchar[result];

			glGetShaderInfoLog(prog, result, 0, debug);
			printf(debug);
			glDeleteShader(prog);
			delete[] debug; debug = nullptr;
		}
		if (fileContent) { delete fileContent; fileContent = nullptr; }
	}	
	return prog;
}
