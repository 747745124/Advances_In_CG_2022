#ifndef WATER_H
#define WATER_H

#include <glad/glad.h>

class Water
{
private:
	float size;
	int index_x;
	int index_z;
	float height;
	unsigned int VBO, EBO;
	unsigned int vertex_count;
	unsigned int draw_count;

public:
	unsigned int VAO;
	Water(int grid_index_x, int grid_index_z, float water_height = -0.5f, float chunk_size = 32.0f)
	{
		this->size = chunk_size;
		this->index_x = grid_index_x;
		this->index_z = grid_index_z;
		this->height = water_height;
		setUp(grid_index_x, grid_index_z, chunk_size, water_height);
	}
	Water(int grid_index_x, int grid_index_z, unsigned int vertex_count, float water_height = -0.5f, float chunk_size = 32.0f)
	{
		this->size = chunk_size;
		this->index_x = grid_index_x;
		this->index_z = grid_index_z;
		this->vertex_count = vertex_count;
		this->height = water_height;
		setUpDense(grid_index_x, grid_index_z, chunk_size, water_height, vertex_count);
	}
	float getHeight()
	{
		return this->height;
	}
	void Draw(Shader &shader)
	{
		shader.use();
		glBindVertexArray(VAO);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (draw_count > 6)
			glDrawElements(GL_TRIANGLES, draw_count, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

private:
	// Draw a quad in XZ-plane
	void setUp(int grid_index_x, int grid_index_z, float chunk_size, float water_height)
	{
		// boundaries of the water chunk
		float Left = -chunk_size / 2.0f + grid_index_x * chunk_size;
		float Right = -chunk_size / 2.0f + (grid_index_x + 1) * chunk_size;
		float Up = -chunk_size / 2.0f + grid_index_z * chunk_size;
		float Down = -chunk_size / 2.0f + (grid_index_z + 1) * chunk_size;
		float vertices[] = {
			Left, water_height, Up,	  // UpLeft
			Left, water_height, Down, // DownLeft
			Right, water_height, Up,  // UpRight
			Right, water_height, Up,  // UpRight
			Left, water_height, Down, // DownLeft
			Right, water_height, Down // DownRight
		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
		glBindVertexArray(0);
		draw_count = 6;
	}
	// set up dense vertices for bump mapping
	void setUpDense(int grid_index_x, int grid_index_z, float chunk_size, float water_height, int vertex_count = 256)
	{
		std::vector<glm::vec3> vertices;
		vector<unsigned int> indices;
		for (int i = 0; i < vertex_count; i++)
			for (int j = 0; j < vertex_count; j++)
			{
				float x = index_x * size + ((float)j - vertex_count / 2 + 0.5) / ((float)vertex_count - 1) * size;
				float z = index_z * size + ((float)i - vertex_count / 2 + 0.5) / ((float)vertex_count - 1) * size;
				float y = height;
				vertices.emplace_back(glm::vec3(x, y, z));
			}
		for (int gz = 0; gz < vertex_count - 1; gz++)
			for (int gx = 0; gx < vertex_count - 1; gx++)
			{
				int topLeft = ((gz)*vertex_count) + gx;
				int topRight = topLeft + 1;
				int bottomLeft = ((gz + 1) * vertex_count) + gx;
				int bottomRight = bottomLeft + 1;
				indices.push_back(topLeft);
				indices.push_back(bottomLeft);
				indices.push_back(topRight);
				indices.push_back(topRight);
				indices.push_back(bottomLeft);
				indices.push_back(bottomRight);
			}

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
		glBindVertexArray(0);
		draw_count = indices.size();
	}
};

#endif