#ifndef textureHolder_h
#define textureHolder_h
#include <SFML/Graphics.hpp>
#include <map>

using namespace sf;

class TextureHolder
{
public:
	TextureHolder();
	static Texture& GetTexture(std::string const& filename);

private:
	std::map<std::string, Texture > m_Textures;
	static TextureHolder* m_s_instance;
};


#endif // !textureHolder_h
