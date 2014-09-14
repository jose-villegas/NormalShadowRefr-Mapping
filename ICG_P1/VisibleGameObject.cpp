#include "stdafx.h"
#include "VisibleGameObject.h"
#include "GraphicSettings.h"
#include "MainEngine.h"

VisibleGameObject::VisibleGameObject() : _isLoaded(false), _toBeDeleted(false), _typeTag(DEFAULT), _roughness(0.3),
    _useProgrammablePipeline(true)
{
}

VisibleGameObject::VisibleGameObject(string filename) : _isLoaded(false), _toBeDeleted(false), _typeTag(DEFAULT),
    _roughness(0.3), _useProgrammablePipeline(true)
{
    Load(filename);
    assert(IsLoaded());
}

VisibleGameObject::~VisibleGameObject()
{
}

void VisibleGameObject::Load(std::string filename)
{
    if (obj.import(filename.c_str(), false))
    {
        obj.normalize();
        cout << "Importing " << filename << endl;
        _isLoaded = true;
        _enableRender = true;
        _enableTextures = true;
        _filepath = filename;
        //Cargas Texturas si las hay
        const ModelOBJ::Material * pMaterial = 0;
        int textureId = 0;
        std::string::size_type offset = 0;
        std::string filenameTexture;

        for (int i = 0; i < obj.getNumberOfMaterials(); ++i)
        {
            pMaterial = &obj.getMaterial(i);

            // Look for and load any diffuse color map textures.

            if (pMaterial->colorMapFilename.empty())
            {
                continue;
            }

            // Try load the texture using the path in the .MTL file.
            textureId = LoadTexture(pMaterial->colorMapFilename.c_str());

            if (!textureId)
            {
                offset = pMaterial->colorMapFilename.find_last_of('\\');

                if (offset != std::string::npos)
                {
                    filenameTexture = pMaterial->colorMapFilename.substr(++offset);
                }
                else
                {
                    filenameTexture = pMaterial->colorMapFilename;
                }

                // Try loading the texture from the same directory as the OBJ file.
                textureId = LoadTexture((obj.getPath() + filenameTexture).c_str());
            }

            if (textureId)
            {
                _modelTextures[pMaterial->colorMapFilename] = textureId;
            }

            // Look for and load any normal map textures.

            if (pMaterial->bumpMapFilename.empty())
            {
                continue;
            }

            // Try load the texture using the path in the .MTL file.
            textureId = LoadTexture(pMaterial->bumpMapFilename.c_str());

            if (!textureId)
            {
                offset = pMaterial->bumpMapFilename.find_last_of('\\');

                if (offset != std::string::npos)
                {
                    filenameTexture = pMaterial->bumpMapFilename.substr(++offset);
                }
                else
                {
                    filenameTexture = pMaterial->bumpMapFilename;
                }

                // Try loading the texture from the same directory as the OBJ file.
                textureId = LoadTexture((obj.getPath() + filenameTexture).c_str());
            }

            if (textureId)
            {
                _modelTextures[pMaterial->bumpMapFilename] = textureId;
            }
        }

        // Finish
        cout << filename << " loaded.." << endl;
    }
    else
    {
        _isLoaded = false;
        _enableTextures = false;
        _filepath = "";
    }
}

void VisibleGameObject::Draw()
{
    if (_isLoaded && _enableRender)
    {
        // Rotations and Translations
        glPushMatrix();
        glTranslatef(_position.x, _position.y, _position.z);
        glMultMatrixf(&_rotationMatrix[0][0]);

        if (_useProgrammablePipeline)
        {
            DrawProgrammablePipeline();
        }
        else
        {
            DrawFixedPipeline();
        }

        glPopMatrix();
    }
}

void VisibleGameObject::SetPosition(float x, float y, float z)
{
    if (_isLoaded)
    {
        _position.x = x;
        _position.y = y;
        _position.z = z;
    }
}

GLuint VisibleGameObject::LoadTexture(const char * pszFilename)
{
    GLuint id = 0;
    Bitmap bitmap;
    WCHAR stra[256];
    MultiByteToWideChar(0, 0, pszFilename, 256, stra, 256);

    if (bitmap.loadPicture(stra))
    {
        // The Bitmap class loads images and orients them top-down.
        // OpenGL expects bitmap images to be oriented bottom-up.
        bitmap.flipVertical();
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        if (GraphicSettings::getAnisotrophyValue() > 1.0f)
        {
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, GraphicSettings::getAnisotrophyValue());
        }

        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, bitmap.width, bitmap.height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap.getPixels());
    }

    return id;
}

sf::Vector3f VisibleGameObject::GetPosition() const
{
    if (_isLoaded)
    {
        return _position;
    }

    return sf::Vector3f();
}

bool VisibleGameObject::IsLoaded() const
{
    return _isLoaded;
}

void VisibleGameObject::Update(float elapsedTime , sf::Event input)
{
}

void VisibleGameObject::RotateObject(float pitch, float yaw, float roll)
{
    // Conversion de Euler Angle a Quaternion
    glm::quat quaternion;
    glm::vec3 eulerAngle(pitch, yaw, roll);
    quaternion = glm::quat(eulerAngle);
    // Conversion de Quaternion a Matriz
    _rotationMatrix = glm::mat4_cast(quaternion);
}

void VisibleGameObject::RotateObject(float quat[4])
{
    // Conversion de Quaternion a Matriz
    _rotationMatrix = glm::mat4_cast(glm::quat(quat[0], quat[1], quat[2], quat[3]));
}

void VisibleGameObject::Scale(float scale)
{
    obj.normalize(scale, true);
}

float * VisibleGameObject::GetObjectRotation()
{
    glm::quat quaternion = glm::quat(_rotationMatrix);
    float rtrn[] = {quaternion.w, quaternion.x, quaternion.y, quaternion.z};
    return rtrn;
}

void VisibleGameObject::DrawFixedPipeline()
{
    const ModelOBJ::Mesh * pMesh = 0;
    const ModelOBJ::Material * pMaterial = 0;
    const ModelOBJ::Vertex * pVertices = 0;
    ModelTextures::const_iterator iter;

    for (int i = 0; i < obj.getNumberOfMeshes(); ++i)
    {
        pMesh = &obj.getMesh(i);
        pMaterial = pMesh->pMaterial;
        pVertices = obj.getVertexBuffer();
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pMaterial->ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pMaterial->diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pMaterial->specular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, pMaterial->shininess * 128.0f);

        if (_enableTextures)
        {
            iter = _modelTextures.find(pMaterial->colorMapFilename);

            if (iter == _modelTextures.end())
            {
                glDisable(GL_TEXTURE_2D);
            }
            else
            {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, iter->second);
            }
        }
        else
        {
            glDisable(GL_TEXTURE_2D);
        }

        if (obj.hasPositions())
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, obj.getVertexSize(), obj.getVertexBuffer()->position);
        }

        if (obj.hasTextureCoords())
        {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, obj.getVertexSize(), obj.getVertexBuffer()->texCoord);
        }

        if (obj.hasNormals())
        {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, obj.getVertexSize(), obj.getVertexBuffer()->normal);
        }

        glDrawElements(GL_TRIANGLES, pMesh->triangleCount * 3, GL_UNSIGNED_INT, obj.getIndexBuffer() + pMesh->startIndex);

        if (obj.hasNormals())
        {
            glDisableClientState(GL_NORMAL_ARRAY);
        }

        if (obj.hasTextureCoords())
        {
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        if (obj.hasPositions())
        {
            glDisableClientState(GL_VERTEX_ARRAY);
        }
    }
}

void VisibleGameObject::DrawProgrammablePipeline()
{
    const ModelOBJ::Mesh * pMesh = 0;
    const ModelOBJ::Material * pMaterial = 0;
    const ModelOBJ::Vertex * pVertices = 0;
    ModelTextures::const_iterator iter;
    GLuint texture = 0;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < obj.getNumberOfMeshes(); ++i)
    {
        pMesh = &obj.getMesh(i);
        pMaterial = pMesh->pMaterial;
        pVertices = obj.getVertexBuffer();
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pMaterial->ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pMaterial->diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pMaterial->specular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, pMaterial->shininess * 128.0f);

        if (pMaterial->bumpMapFilename.empty())
        {
            // Per fragment Blinn-Phong code path.
            if (MainEngine::_enableShader) { glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "bEnableBumpMapping"), 0); }

            // Bind the color map texture.
            texture = MainEngine::_nullTexture;

            if (_enableTextures)
            {
                iter = _modelTextures.find(pMaterial->colorMapFilename);

                if (iter != _modelTextures.end())
                {
                    texture = iter->second;
                }
            }

            glActiveTexture(GL_TEXTURE0);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texture);

            // Update shader parameters.
            if (MainEngine::_enableShader)
            {
                glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "colorMap"), 0);
                glUniform1f(glGetUniformLocation(MainEngine::shaders["MainShader"], "materialAlpha"), pMaterial->alpha);
            }
        }
        else
        {
            // Normal mapping code path.
            if (MainEngine::_enableShader) { glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "bEnableBumpMapping"), 1); }

            // Bind the normal map texture.
            iter = _modelTextures.find(pMaterial->bumpMapFilename);

            if (iter != _modelTextures.end())
            {
                glActiveTexture(GL_TEXTURE1);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, iter->second);
            }

            // Bind the color map texture.
            texture = MainEngine::_nullTexture;

            if (_enableTextures)
            {
                iter = _modelTextures.find(pMaterial->colorMapFilename);

                if (iter != _modelTextures.end())
                {
                    texture = iter->second;
                }
            }

            glActiveTexture(GL_TEXTURE0);
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texture);

            // Update shader parameters.
            if (MainEngine::_enableShader)
            {
                glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "colorMap"), 0);
                glUniform1i(glGetUniformLocation(MainEngine::shaders["MainShader"], "normalMap"), 1);
                glUniform1f(glGetUniformLocation(MainEngine::shaders["MainShader"], "materialAlpha"), pMaterial->alpha);
            }
        }

        // Render mesh.
        if (obj.hasPositions())
        {
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, obj.getVertexSize(), obj.getVertexBuffer()->position);
        }

        if (obj.hasTextureCoords())
        {
            glClientActiveTexture(GL_TEXTURE0);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(2, GL_FLOAT, obj.getVertexSize(), obj.getVertexBuffer()->texCoord);
        }

        if (obj.hasNormals())
        {
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, obj.getVertexSize(), obj.getVertexBuffer()->normal);
        }

        if (obj.hasTangents())
        {
            glClientActiveTexture(GL_TEXTURE1);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(4, GL_FLOAT, obj.getVertexSize(), obj.getVertexBuffer()->tangent);
        }

        glDrawElements(GL_TRIANGLES, pMesh->triangleCount * 3, GL_UNSIGNED_INT, obj.getIndexBuffer() + pMesh->startIndex);

        if (obj.hasTangents())
        {
            glClientActiveTexture(GL_TEXTURE1);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        if (obj.hasNormals())
        {
            glDisableClientState(GL_NORMAL_ARRAY);
        }

        if (obj.hasTextureCoords())
        {
            glClientActiveTexture(GL_TEXTURE0);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        if (obj.hasPositions())
        {
            glDisableClientState(GL_VERTEX_ARRAY);
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_BLEND);
}
