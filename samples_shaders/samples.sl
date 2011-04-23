surface my_constant()
{
	Oi = Os;
	Ci = Os * Cs;
}

surface surfaceNormals()
{
	vec3 Nf = faceforward(N, I);
	Oi = Os;
	Ci = Os * (Nf + 1) * 0.5;
}

surface diffuse_sample()
{
	Oi = Os;
	Ci = Os * Cs * diffuse(N);
}

surface specular_sample(real roughness=0.1;)
{
	Oi = Os;
	Ci = Os * Cs * (diffuse(N) * 0.5 + 0.5 * specular(N, -normalize(I), roughness));
}

surface reflect_sample(
						real Ka = 1;
						real Kd = 0.5;
						real Ks = 0.5;
						real roughness = 0.1;
						real reflectStrengh = 1;)
{
	vec3 Nf = faceforward(normalize(N), I);
	vec3 V = -normalize(I);
	vec3 r = reflect(I, Nf);
	color lighting = Ka * ambient() + Kd * diffuse(Nf) + Ks * specular(Nf, V, roughness);
	Oi = Os;
	Ci = Os * Cs * ((1 - reflectStrengh) * lighting + trace(P, r) * reflectStrengh);
}

surface matte_opa(real Ka = 1;
				real Kd = 1;)
{
	vec3 Nf = faceforward(N, I);
	Oi = Os;
	Ci = Os * Cs * (Kd * diffuse(N));
}

surface smoothstep_sample(real rim_width = 0.5;)
{
	vec3 nf = faceforward(N, I);
	vec3 i = normalize(-I);
	real dot = 1 - nf.i;
	real diffusecolor = smoothstep(1 - rim_width, 1, dot);
	
	Oi = Os;
	Ci = Oi * Cs * diffusecolor;
}

surface mix_sample()
{
	color top = color(0.878, 0.996, 0.250);
	color low = color(0.250, 0.878, 0.988);
	Oi = Os;
	Ci = Oi * Cs * (diffuse(N) + ambient()) * mix(top, low, t);
}
