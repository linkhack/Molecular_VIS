#pragma once
#include <GL/glew.h>
#include <vector>
/*!
 * \brief Class for handling render to texture using framebuffer objects.
 * 
 */
class FBO {
public:
	/*!
	 * Creates a FBO with at most two color textures and one depth texture
	 * 
	 * \param useTextureColor if a color texture is used
	 * \param useTextureDepth if a depth texture is used
	 * \param multiSample if multisampling should be enabled
	 * \param width width of the framebuffer and textures
	 * \param height height of the framebuffer and textures
	 * \param addSecondColorBuffer if an additional color texture should be attached.
	 */
	FBO(bool useTextureColor, bool useTextureDepth, bool multiSample, unsigned int width, unsigned int height, bool addSecondColorBuffer = false);
	
	/*!
	 * Creates a FBO with a arbitrary number of color textures.
	 * 
	 * \param nrColorTexture nr of color textures attached.
	 * \param useDepthTexture if a depth texture is used
	 * \param useMultisampling if multisampling should be enabled
	 * \param width width of the framebuffer and textures
	 * \param height height of the framebuffer and textures
	 */
	FBO(unsigned int nrColorTexture, bool useDepthTexture, bool useMultisampling, unsigned int width, unsigned int height);
	~FBO();

	/*!
	 * Copys fbo data from one fbo to an other.
	 * 
	 * \param from where to copy from
	 * \param to where to copy to
	 * \param bufferMask what to copy. See openGL documentaiton of glBlitFramebuffer
	 */
	static void copyFBO(const FBO& from, const FBO& to, unsigned int bufferMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*! 
	 * \overload 
	 */
	static void copyFBO(const FBO& from, unsigned int to, unsigned int bufferMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*!
	* \overload
	*/
	static void copyFBO(unsigned int from, const FBO& to, unsigned int bufferMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*!
	 * Gets the handle of the i-th color texture, counted from 0. Handle can then be used in glBindTexture etc.
	 * The texture is either a GL_TEXTURE_2D or GL_TEXTURE_2D_MULTISAMPLE, depending on if multisampling is enabled or not.
	 * 
	 * \param i index of the texture
	 * \return handle of texture at index 0
	 */
	const unsigned int getColorTexture(unsigned int i) const { return colorTextures.at(i); }
	/*!
	 * Convinience method for getColorTexture(0)
	 * 
	 * \return handle of first color texture
	 */
	const unsigned int getColorTexture() const { return colorTextures.at(0); }
	/*!
	 * convinience method for getColorTexture(1)
	 *
	 * \return handle of first color texture
	 */
	const unsigned int getSecondaryColorTexture() const { return colorTextures.at(1); }
	/*!
	 * Gets the handle of the depth texture. Handle can then be used in glBindTexture etc.
	 * The texture is either a GL_TEXTURE_2D or GL_TEXTURE_2D_MULTISAMPLE, depending on if multisampling is enabled or not.
	 * 
	 * \return handle of depth texture
	 */
	const unsigned int getDepthTexture() const { return depthTexture; }
	/*!
	 * Checks if FBO has multisampling enabled
	 * 
	 */
	const bool isMultiSample() const { return multiSample; };

	/*!
	 * Prepares FBO for rendering. All draw commands after calling this output to attached textures.
	 * 
	 */
	void setActive();
	
	/*!
	 * Multisample count if multisampling is enebled.
	 * 
	 */
	static unsigned int MULTI_SAMPLE_COUNT;
private:
	unsigned int fbo;
	unsigned int width;
	unsigned int height;
	unsigned int colorTexture; //not needed
	unsigned int colorTexture2; //not needed
	unsigned int depthTexture;



	bool useTextureDepth;
	bool useTextureColor;
	std::vector<unsigned int> colorTextures;
	std::vector<unsigned int> multiSampleTextures;
	bool hasSecondaryColorBuffer;
	bool multiSample;

	void createFrameBuffers(unsigned int nrColorTexture, bool useDepthTexture, bool useMultisampling, unsigned int width, unsigned int height);

	
};

