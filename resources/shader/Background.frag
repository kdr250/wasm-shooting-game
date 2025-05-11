#version 330

precision highp float;
precision highp int;

uniform vec2 uWindowSize;
uniform vec3 uColor;
uniform float uTime;

out vec4 fragColor;

const float PI = 3.14159265359;
const float TAU = 6.2831853;

// start rotation
vec2 rotation2(vec2 p, float t) {
    return vec2(cos(t) * p.x - sin(t) * p.y, sin(t) * p.x + cos(t) * p.y);
}

vec3 rotationX(vec3 p, float t) {
    return vec3(p.x, rotation2(p.yz, t));
}

vec3 rotationY(vec3 p, float t) {
    return vec3(p.y, rotation2(p.zx, t)).zxy;
}

vec3 rotationZ(vec3 p, float t) {
    return vec3(rotation2(p.xy, t), p.z);
}

vec3 euler(vec3 p, vec3 t) {
    return rotationZ(rotationY(rotationX(p, t.x), t.y), t.z);
}
// end rotation

float sphereSDF(vec3 p, vec3 c, float r) {
    return length(p - c) - r;
}

float sceneSDF(vec3 p) {
    vec3 center = vec3(0.0);
    float scale = 0.1;
    return sphereSDF(fract(p + 0.5) - 0.5, center, scale);
}

vec3 gradSDF(vec3 p) {
    float eps = 0.001;
    return normalize(vec3(
        sceneSDF(p + vec3(eps, 0.0, 0.0)) - sceneSDF(p - vec3(eps, 0.0, 0.0)),
        sceneSDF(p + vec3(0.0, eps, 0.0)) - sceneSDF(p - vec3(0.0, eps, 0.0)),
        sceneSDF(p + vec3(0.0, 0.0, eps)) - sceneSDF(p - vec3(0.0, 0.0, eps))
    ));
}

void main() {
    vec2 p = (gl_FragCoord.xy * 2.0 - uWindowSize) / min(uWindowSize.x, uWindowSize.y);

    vec3 t = vec3(uTime * 0.1);
    
    vec3 cameraPos = euler(vec3(0.0, 0.0, 2.0), t);
    vec3 cameraDir = euler(vec3(0.0, 0.0, -1.0), t);
    vec3 cameraUp = euler(vec3(0.0, 1.0, 0.0), t);
    vec3 cameraSide = cross(cameraDir, cameraUp);

    float targetDepth = 1.0;

    vec3 lightDir = vec3(1.0);

    vec3 ray = cameraSide * p.x + cameraUp * p.y + cameraDir * targetDepth;
    vec3 rayPos = ray + cameraPos;
    ray = normalize(ray);

    vec3 resultColor = vec3(0.0);
    for (int i = 0; i < 50; i++) {
        if (sceneSDF(rayPos) > 0.001) {
            rayPos += sceneSDF(rayPos) * ray;
        } else {
            float amb = 0.1;
            float diff = 0.9 * max(dot(normalize(lightDir), gradSDF(rayPos)), 0.0);
            resultColor = (diff + amb) * uColor;
            break;
        }
    }

    fragColor = vec4(resultColor, 1.0);
}
