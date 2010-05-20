/*
 * Copyright (c) 2009-2010 The Gear Team
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Gear nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// texture id that should be blurred
uniform sampler2D tex;
// with of texture in pixels
uniform int textureWidth;
// height of texture in pixels
uniform int textureHeight;
// blur radius
uniform int radius;
// blur angle (radians, CCW)
uniform float angle;

varying vec4 Color;
varying vec2 TexCoord;

vec2 toTextureCoords(in vec2 texCoords)
{
        return vec2(texCoords.x * float(textureWidth), texCoords.y * float(textureHeight));
}

vec2 toOpenGLCoords(in vec2 texCoords)
{
        return vec2(texCoords.x / float(textureWidth), texCoords.y / float(textureHeight));
}

vec4 bluredColor()
{
        vec2 screenCoords = toTextureCoords(TexCoord);
        float maxLen = length(vec2(radius/2, radius/2));
        float sum = 0.0;

        vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
        
        angle = -angle;
        float cosinus = cos(angle);
        float sinus = sin(angle);
        
        float x, y;

        for (int i = -radius; i <= radius; ++i) {
        
                x = float(i) * cosinus;
                y = float(i) * sinus;
        
                float weight = float(radius) - length(vec2(i, 0)) / 2.0;
                color += texture2D(tex, toOpenGLCoords(screenCoords + vec2(x, y))) * weight;

                sum += weight;
        }

        color /= sum;
        return color;
}

vec4 sharpColor()
{
        return texture2D(tex, TexCoord);
}

void main()
{
        if (radius > 0) {
                gl_FragColor = bluredColor();
        } else {
                gl_FragColor = sharpColor();
        }
}

