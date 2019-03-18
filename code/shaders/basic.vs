#version 330 core

struct Dir_Light{
    bool status;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Point_Light{
    bool status;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct Material{
    vec3 object_color;
    float shininess;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform float z_near;
uniform float z_far;

uniform vec3 camera_pos;

uniform Material main_object;
uniform Dir_Light dir_light;
uniform Point_Light point_light;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

smooth out vec3 final_color_smooth;
flat out vec3 final_color_flat;

vec3 cal_dir_light(Dir_Light light, vec3 normal, vec3 view_dir){
    vec3 light_dir = normalize(-light.direction);
    float diffuse_param = max(dot(normal,light_dir),0.0f);

    vec3 halfway_dir = normalize(light_dir + view_dir);
    float specular_param = pow(max(dot(halfway_dir,normal),0.0f), main_object.shininess);

    vec3 ambient = light.ambient;
    vec3 diffuse = diffuse_param * light.diffuse;
    vec3 specular = specular_param * light.specular;
    vec3 res = ambient + diffuse + specular;
    return res;
}

vec3 cal_point_light(Point_Light light, vec3 normal,vec3 frag_pos, vec3 view_dir){
    vec3 light_dir = normalize(light.position - frag_pos);
    float diffuse_param = max(dot(light_dir,normal),0.0f);

    vec3 halfway_dir = normalize(light_dir + view_dir);
    float specular_param = pow(max(dot(halfway_dir,normal),0.0f), main_object.shininess);

    float distance = length(light.position - frag_pos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    vec3 ambient = attenuation * light.ambient;
    vec3 diffuse = attenuation * diffuse_param * light.diffuse;
    vec3 specular = attenuation * specular_param * light.specular;
    
    vec3 res = ambient + diffuse + specular;
    return res;
}

void main(){
    gl_Position = projection * view * model * vec4(position, 1.0f);

    vec3 frag_pos = vec3(model * vec4(position, 1.0f));

    vec3 normal_v = normalize(normal);
    vec3 view_dir = normalize(camera_pos - frag_pos);

    vec3 final_color = vec3(0.0f,0.0f,0.0f);

    if(dir_light.status){
        vec3 dir_light_color = cal_dir_light(dir_light,normal_v,view_dir);
        final_color += dir_light_color * main_object.object_color;
    }
    if(point_light.status){
        vec3 point_light_color = cal_point_light(point_light,normal_v,frag_pos,view_dir);
        final_color += point_light_color * main_object.object_color;
    }
    
    final_color_smooth = final_color;
    final_color_flat = final_color;
}