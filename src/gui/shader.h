/*
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * Copyright (c) 2022, Saveliy Pototskiy (SavaLione) (savalione.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/**
 * @file
 * @brief OpenGL shader representation
 * @author Saveliy Pototskiy (SavaLione)
 * @date 16 Dec 2022
 */
#ifndef GUI_SHADER_H
#define GUI_SHADER_H

// clang-format off
#include <CL/cl_gl.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
// clang-format on

#include <string>

enum shader_type
{
    fragment = 0x8B30,
    vertex   = 0x8B31
};

class shader
{
public:
    /* Create and compile shader */
    shader(shader_type const &type, std::string const &source)
    {
        _id             = glCreateShader(type);
        char const *src = source.c_str();

        glShaderSource(_id, 1, &src, nullptr);
        glCompileShader(_id);

        int result;

        glGetShaderiv(_id, GL_COMPILE_STATUS, &result);

        if(result == GL_FALSE)
        {
            int length;
            glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &length);
            char *message = (char *)malloc(length);
            glGetShaderInfoLog(_id, length, &length, message);
            std::string _err = "Shader compilation error: ";
            _err += message;
            glDeleteShader(_id);
            throw std::runtime_error(_err);
        }
    }

    ~shader();

    /* Get shader id */
    GLuint const &id() const
    {
        return _id;
    }

private:
    /* Shader id */
    GLuint _id = 0;
};

#endif // GUI_SHADER_H