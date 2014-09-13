#pragma once
class ModelVBO
{
	public:
		GLuint vertexbuffer;
		GLuint uvbuffer;
		GLuint normalbuffer;
		GLuint tangentbuffer;
		GLuint bitangentbuffer;
		GLuint elementbuffer;

	public:
		ModelVBO(void);
		~ModelVBO(void);
		void Load(ModelOBJ obj);
		void BindBuffers(ModelOBJ obj);
		void UnbindBuffers();
};

