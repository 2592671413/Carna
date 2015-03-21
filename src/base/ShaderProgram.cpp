/*
 *  Copyright (C) 2010 - 2014 Leonid Kostrykin
 *
 *  Chair of Medical Engineering (mediTEC)
 *  RWTH Aachen University
 *  Pauwelsstr. 20
 *  52074 Aachen
 *  Germany
 *
 */

#include <Carna/base/glew.h>
#include <Carna/base/ShaderProgram.h>
#include <Carna/base/Shader.h>
#include <Carna/base/ShaderCompilationError.h>
#include <Carna/base/Log.h>

namespace Carna
{

namespace base
{



// ----------------------------------------------------------------------------------
// ShaderProgram :: Factory :: Details
// ----------------------------------------------------------------------------------

struct ShaderProgram::Factory::Details
{
    const Shader* vertShader;
    const Shader* geomShader;
    const Shader* fragShader;
    Details();
};


ShaderProgram::Factory::Details::Details()
    : vertShader( nullptr )
    , geomShader( nullptr )
    , fragShader( nullptr )
{
}



// ----------------------------------------------------------------------------------
// ShaderProgram :: Factory
// ----------------------------------------------------------------------------------

ShaderProgram::Factory::Factory()
    : pimpl( new Details() )
{
}


ShaderProgram::Factory::~Factory()
{
}


void ShaderProgram::Factory::setVertexShader( const Shader& shader )
{
    CARNA_ASSERT( shader.type == Shader::TYPE_VERTEX_SHADER );
    pimpl->vertShader = &shader;
}


void ShaderProgram::Factory::setGeometryShader( const Shader& shader )
{
    CARNA_ASSERT( shader.type == Shader::TYPE_GEOMETRY_SHADER );
    pimpl->geomShader = &shader;
}


void ShaderProgram::Factory::setFragmentShader( const Shader& shader )
{
    CARNA_ASSERT( shader.type == Shader::TYPE_FRAGMENT_SHADER );
    pimpl->fragShader = &shader;
}


ShaderProgram* ShaderProgram::Factory::create() const
{
    CARNA_ASSERT_EX( pimpl->vertShader != nullptr, "No vertex shader set!" );
    CARNA_ASSERT_EX( pimpl->fragShader != nullptr, "No fragment shader set!" );
    
    ShaderProgram* const shaderProgram = new ShaderProgram();
    try
    {
        /* Compose the shader program.
         */
        const unsigned int id = shaderProgram->id;
        glAttachShader( id, pimpl->vertShader->id );
        glAttachShader( id, pimpl->fragShader->id );
        if( pimpl->geomShader != nullptr )
        {
            glAttachShader( id, pimpl->geomShader->id );
        }
        
        /* Link the shader program.
         */
        glLinkProgram( id );
        return shaderProgram;
    }
    catch( ... )
    {
        delete shaderProgram;
        throw;
    }
}



// ----------------------------------------------------------------------------------
// ShaderProgram
// ----------------------------------------------------------------------------------

ShaderProgram::ShaderProgram()
    : id( glCreateProgram() )
{
}


void ShaderProgram::checkErrors() const
{
    GLint nInfoLogLength;
    glGetProgramiv( id, GL_INFO_LOG_LENGTH, &nInfoLogLength );
    if( nInfoLogLength > 1 )
    {
        const std::unique_ptr< GLchar > buf( new GLchar[ nInfoLogLength ] );
        glGetProgramInfoLog( id, nInfoLogLength, 0, buf.get() );
        const std::string err( buf.get() );
        if( err != "No errors." )
        {
            throw ShaderCompilationError( err );
        }
    }
}


ShaderProgram::~ShaderProgram()
{
    if( id )
    {
        glDeleteProgram( id );
    }
}



}  // namespace Carna :: base

}  // namespace Carna
