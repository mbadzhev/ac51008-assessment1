// Minimal fragment shader
// Iain Martin 2018

#version 420 core

in vec4 fcolour;
out vec4 outputColor;
void main()
{
	outputColor = fcolour;
}