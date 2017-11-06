#version 330

precision mediump float;  
in mediump vec4 fscolor;

void main()
{
    gl_FragColor = fscolor;
}

