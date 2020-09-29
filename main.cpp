#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iomanip> 

using namespace std;






//numero de vertices 
int nv = 0;

//numero de caras 
int nc = 0;

int nn = 0;


vector<float> _x;
vector<float> _y;
vector<float> _z;
vector<float> _v1;
vector<float> _v2;
vector<float> _v3;
vector<float> _n3;

void LeerArchivo() {
	// variables
	string numVer, numCar, numNose, x, y, z, v1, v2, v3, n3;

	ifstream coeff("helice.off");
	if (coeff.is_open())
	{
		//ignnorar
		string line;
		getline(coeff, line);

		//segunda linea
		getline(coeff, numVer, ' ');
		nv = stof(numVer);
		getline(coeff, numCar, ' ');
		nc = stof(numCar);
		getline(coeff, numNose, '\n');
		nn = stof(numNose);

		for (int i = 0; i < nv; i++) {
			getline(coeff, x, ' ');
			_x.push_back(stof(x)/3);
			getline(coeff, y, ' ');
			_y.push_back(stof(y)/3);
			getline(coeff, z, '\n' );
			_z.push_back(stof(z)/3);
		}

		for (int i = 0; i < nc; i++) {
			getline(coeff, n3, ' ');
			_n3.push_back(stof(n3));
			getline(coeff, v1, ' ');
			_v1.push_back(stof(v1));
			getline(coeff, v2, ' ');
			_v2.push_back(stof(v2));
			getline(coeff, v3, '\n');
			_v3.push_back(stof(v3));
		}

		coeff.close();

	}
	else cout << "Unable to open file";
}



void framebuffer_tamanho_callback(GLFWwindow* ventana, int ancho, int alto) {
	glViewport(0, 0, ancho, alto);
}
void procesarEntrada(GLFWwindow* ventana) {
	if (glfwGetKey(ventana, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(ventana, true);
}

const unsigned int ANCHO = 600;
const unsigned int ALTO = 600;

class CProgramaShaders {
	GLuint idPrograma;
public:
	CProgramaShaders(string rutaShaderVertice, string rutaShaderFragmento) {
		//Variables para leer los archivos de cÃ³digo
		string strCodigoShaderVertice;
		string strCodigoShaderFragmento;
		ifstream pArchivoShaderVertice;
		ifstream pArchivoShaderFragmento;
		//Mostramos excepciones en caso haya
		pArchivoShaderVertice.exceptions(ifstream::failbit | ifstream::badbit);
		pArchivoShaderFragmento.exceptions(ifstream::failbit | ifstream::badbit);
		try {
			//Abriendo los archivos de cÃ³digo de los shader
			pArchivoShaderVertice.open(rutaShaderVertice);
			pArchivoShaderFragmento.open(rutaShaderFragmento);
			//Leyendo la informaciÃ³n de los archivos
			stringstream lectorShaderVertice, lectorShaderFragmento;
			lectorShaderVertice << pArchivoShaderVertice.rdbuf();
			lectorShaderFragmento << pArchivoShaderFragmento.rdbuf();
			//Cerrando los archivos
			pArchivoShaderVertice.close();
			pArchivoShaderFragmento.close();
			//Pasando la informaciÃ³n leida a string
			strCodigoShaderVertice = lectorShaderVertice.str();
			strCodigoShaderFragmento = lectorShaderFragmento.str();
		}
		catch (ifstream::failure) {
			cout << "ERROR: Los archivos no pudieron ser leidos correctamente.\n";
		}
		const char* codigoShaderVertice = strCodigoShaderVertice.c_str();
		const char* codigoShaderFragmento = strCodigoShaderFragmento.c_str();
		//Asociando y compilando los cÃ³digos de los shader
		GLuint idShaderVertice, idShaderFragmento;
		//Shader de VÃ©rtice
		idShaderVertice = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(idShaderVertice, 1, &codigoShaderVertice, NULL);
		glCompileShader(idShaderVertice);
		verificarErrores(idShaderVertice, "VÃ©rtice");
		//Shader de Fragmento
		idShaderFragmento = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(idShaderFragmento, 1, &codigoShaderFragmento, NULL);
		glCompileShader(idShaderFragmento);
		verificarErrores(idShaderVertice, "Fragmento");
		//Programa de Shaders
		this->idPrograma = glCreateProgram();
		glAttachShader(this->idPrograma, idShaderVertice);
		glAttachShader(this->idPrograma, idShaderFragmento);
		glLinkProgram(this->idPrograma);
		//Ahora ya podemos eliminar los programas de los shaders
		glDeleteShader(idShaderVertice);
		glDeleteShader(idShaderFragmento);
	}
	~CProgramaShaders() {
		glDeleteProgram(this->idPrograma);
	}
	void usar() const {
		glUseProgram(this->idPrograma);
	}
	void setVec3(const string& nombre, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(this->idPrograma, nombre.c_str()), x, y, z);
	}
	void setVec3(const string& nombre, const glm::vec3& valor) const {
		glUniform3fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, &valor[0]);
	}
	void setMat4(const string& nombre, const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
private:
	void verificarErrores(GLuint identificador, string tipo) {
		GLint ok;
		GLchar log[1024];

		if (tipo == "Programa") {
			glGetProgramiv(this->idPrograma, GL_LINK_STATUS, &ok);
			if (!ok) {
				glGetProgramInfoLog(this->idPrograma, 1024, NULL, log);
				cout << "Error de enlace con el programa: " << log << "\n";
			}
		}
		else {
			glGetShaderiv(identificador, GL_COMPILE_STATUS, &ok);
			if (!ok) {
				glGetShaderInfoLog(identificador, 1024, nullptr, log);
				cout << "Error de compilaciÃ³n con el Shader de " << tipo << ": " << log << "\n";
			}
		}
	}
};

int main() {
	//Inicializar glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creando la ventana
	GLFWwindow* ventana = glfwCreateWindow(ANCHO, ALTO, "Compu Grafica", NULL, NULL);
	if (ventana == NULL) {
		cout << "Problemas al crear la ventana\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(ventana);
	glfwSetFramebufferSizeCallback(ventana, framebuffer_tamanho_callback);

	//Cargar Glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Problemas al cargar GLAD\n";
		return -1;
	}

	CProgramaShaders programa_shaders = CProgramaShaders("GLSL/codigo.vs", "GLSL/codigo.fs");


	LeerArchivo();

	float vertices[100000] = {

	};

	int cont = 0;

	for (int i = 0; i < nv; i++) {

		vertices[cont] = _x[i];
		cont++;
		vertices[cont] = _y[i];
		cont++;
		vertices[cont] = _z[i];
		cont++;

		vertices[cont] = 1.0;
		cont++;
		vertices[cont] = 1.0;
		cont++;
		vertices[cont] = 1.0;
		cont++;

	}

	unsigned int indices[100000] = {

	};

	 cont = 0;
	for (int i = 0; i < nc; i++) {

		indices[cont] = _v1[i];
		cont++;
		indices[cont] = _v2[i];
		cont++;
		indices[cont] = _v3[i];
		cont++;

	}

	
	//Para leer sin indices
	if (nc == 0)
	{
		for (int i = 0; i < nv; i++)
		{
			indices[i] = i;
		}
		nc = nv * 3;
	}

	//Enviando la geometrÃ­a al GPU: Definiendo los buffers (Vertex Array Objects y Vertex Buffer Objects)
	unsigned int id_array_vertices, id_array_buffers, id_element_buffer;
	glGenVertexArrays(1, &id_array_vertices);
	glGenBuffers(1, &id_array_buffers);
	glGenBuffers(1, &id_element_buffer);

	//anexando los buffers para su uso en la tarjeta grÃ¡fica
	glBindVertexArray(id_array_vertices);

	//Anexando buffers y cargandolos con los datos
	glBindBuffer(GL_ARRAY_BUFFER, id_array_buffers);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Indicando las especificaciones de los atributos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(ventana)) {
		procesarEntrada(ventana);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glm::mat4 Model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 Projection = glm::perspective(45.0f, 1.0f * ANCHO / ALTO, 0.1f, 100.0f);
		glm::mat4 View = glm::lookAt(glm::vec3(4, 3, -3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		glm::mat4 transformacion = Projection * View * Model;
		programa_shaders.setMat4("transformacion", transformacion);



		programa_shaders.usar();
		//programa_shaders.setVec3("traslacion", glm::vec3(0.0, 0.0, 0.0));
		glBindVertexArray(id_array_vertices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_element_buffer);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawElements(GL_TRIANGLES, nc*3 , GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(ventana);
		glfwPollEvents();
	}
	//Liberando memoria
	glDeleteVertexArrays(1, &id_array_vertices);
	glDeleteBuffers(1, &id_array_buffers);
	glfwTerminate();
	return 0;
}