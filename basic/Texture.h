#ifndef TEXTURE_H
#define TEXTURE_H

#include <stb_image.h>
#include <glad/glad.h>
#include <mesh.h>
#include <string>

class TextureManager
{
private:
	int warp_s;
	int warp_t;
	int min_filter;
	int mag_filter;

public:
	TextureManager(int warp_s, int warp_t, int min_filter, int mag_filter) : warp_s(warp_s), warp_t(warp_t), min_filter(min_filter), mag_filter(mag_filter)
	{
	}
	Texture LoadTexture(std::string texture_path, std::string type)
	{
		unsigned int textureID;
		textureID = LoadFromFile(texture_path);

		return Texture{
			textureID, type, texture_path};
	}
	std::vector<Texture> LoadTexture(std::vector<std::string> texture_paths, std::vector<std::string> types)
	{
		assert(texture_paths.size() == types.size());
		std::vector<Texture> textures;
		for (int i = 0; i < texture_paths.size(); i++)
		{
			textures.push_back(
				Texture{
					LoadFromFile(texture_paths[i]), types[i], texture_paths[i]});
		}
		return textures;
	}

private:
	unsigned int LoadFromFile(std::string texture_path)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(texture_path.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warp_s);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warp_t);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << texture_path << std::endl;
			stbi_image_free(data);
		}
		return textureID;
	}
};

#endif // !TEXTURE_H
