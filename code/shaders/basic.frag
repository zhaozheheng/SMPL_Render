#version 330 core

smooth in vec3 final_color_smooth;
flat in vec3 final_color_flat;

layout( location = 0 ) out vec4 FragColor;

uniform bool shading_mode;

void main(){
    if(shading_mode){
        FragColor = vec4(final_color_smooth,1.0f);
    }else{
        FragColor = vec4(final_color_flat,1.0f);
    }
}