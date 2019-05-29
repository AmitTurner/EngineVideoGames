#version 330

attribute vec3 position;
attribute vec3 color;
attribute vec3 normal;
attribute vec3 weights;
attribute vec2 texCoords;

out vec2 texCoord0;
out vec3 normal0;
out vec3 color0;

uniform mat2x4 Quaternion;
uniform mat2x4 lastQuaternion;
uniform mat2x4 nextQuaternion;
uniform mat4 Normal;

void main()
{
	color0 = weights;	
	texCoord0 = texCoords;
	normal0 = (Normal * vec4(normal, 0.0)).xyz;
	
	vec4 dq0;
	vec4 dqe;
	dq0 = weights.x * lastQuaternion[0] + weights.y * Quaternion[0] + weights.z * nextQuaternion[0];
	dq0 = normalize(dq0);
	dqe = weights.x * nextQuaternion[1] + weights.y * nextQuaternion[1] + weights.z * nextQuaternion[1];
	dqe = normalize(dqe);

	float t0 = 2*(-dqe[3]*dq0[0] + dqe[0]*dq0[3] - dqe[1]*dq0[2] + dqe[2]*dq0[1]);
	float t1 = 2*(-dqe[3]*dq0[1] + dqe[0]*dq0[2] + dqe[1]*dq0[3] - dqe[2]*dq0[0]);
	float t2 = 2*(-dqe[3]*dq0[2] - dqe[0]*dq0[1] + dqe[1]*dq0[0] + dqe[2]*dq0[3]);
		
	mat4x3 M;
	M[0] = vec4(1-2*dq0[1]*dq0[1]-2*dq0[2]*dq0[2], 2*dq0[0]*dq0[1]-2*dq0[3]*dq0[2], 2*dq0[0]*dq0[2]-2*dq0[3]*dq0[1], t0);
	M[1] = vec4(2*dq0[0]*dq0[1]+2*dq0[3]*dq0[2], 1 - 2*dq0[0]*dq0[0]-2*dq0[2]*dq0[2], 2*dq0[1]*dq0[2]-2*dq0[3]*dq0[0], t1);
	M[2] = vec4(2*dq0[0]*dq0[2]-2*dq0[3]*dq0[1], 2*dq0[1]*dq0[2]+2*dq0[3]*dq0[1], 1 - 2*dq0[0]*dq0[0]-2*dq0[1]*dq0[1], t2);
	
	vec3 pos = M*vec4(position, 1.0);
	gl_Position = vec4(pos, 1.0);
}

 